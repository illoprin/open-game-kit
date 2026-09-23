#include "game/fps_controller.hpp"

#include "core/clock.hpp"
#include "core/input.hpp"
#include "core/window.hpp"
#include "glm/geometric.hpp"
#include "scene/camera.hpp"
#include <algorithm>
#include <cmath>

static glm::vec3 getDirection(float deg) {
  glm::vec3 dir = {std::cos(glm::radians(deg)), 0, std::sin(glm::radians(deg))};
  if (glm::length(dir) > 0.01f) dir = glm::normalize(dir);
  return dir;
}

// Initializes the controller with given config, position, and orientation
FPSController::FPSController(Config cfg, glm::vec3 start_pos, float start_yaw)
    : Cfg(cfg) {
  body.position = start_pos;
  SetYaw(start_yaw);
}

// Gathers movement input and handles jump flag state
void FPSController::ApplyMovementInput(float dt) {
  glm::vec3 right = glm::normalize(glm::cross(front, {0, 1, 0}));

  input_dir = glm::vec3{0};
  if (Input::GetKeyDown(GLFW_KEY_W)) input_dir += front;
  if (Input::GetKeyDown(GLFW_KEY_S)) input_dir -= front;
  if (Input::GetKeyDown(GLFW_KEY_D)) input_dir += right;
  if (Input::GetKeyDown(GLFW_KEY_A)) input_dir -= right;

  if (glm::length(input_dir) > 0.0f) input_dir = glm::normalize(input_dir);

  want_jump = Input::GetKeyPressed(GLFW_KEY_SPACE);
}

// Updates camera view angles based on mouse movement delta
void FPSController::UpdateLook(Camera3D& camera) {
  double dx, dy;
  Input::GetCursorDelta(dx, dy);

  camera.Rotation.y += static_cast<float>(dx) * Cfg.mouse_sens;
  camera.Rotation.x -= static_cast<float>(dy) * Cfg.mouse_sens;
  camera.Rotation.x  = std::clamp(camera.Rotation.x, -89.0f, 89.0f);

  front = getDirection(camera.Rotation.y);
}

// Processes directional movement and jump input
void FPSController::ProcessInput() {
  glm::vec3 right = glm::normalize(glm::cross(front, {0, 1, 0}));

  input_dir = glm::vec3{0};
  if (Input::GetKeyDown(GLFW_KEY_W)) input_dir += front;
  if (Input::GetKeyDown(GLFW_KEY_S)) input_dir -= front;
  if (Input::GetKeyDown(GLFW_KEY_D)) input_dir += right;
  if (Input::GetKeyDown(GLFW_KEY_A)) input_dir -= right;

  if (glm::length(input_dir) > 0.0f) input_dir = glm::normalize(input_dir);

  want_jump = Input::GetKeyDown(GLFW_KEY_SPACE);
}

// Sets the world position of the character body
void FPSController::SetPosition(const glm::vec3& pos) noexcept {
  body.position = pos;
}

// Sets the controller's horizontal facing direction by yaw angle in degrees
void FPSController::SetYaw(float yaw_degrees) noexcept {
  front = getDirection(yaw_degrees);
}

// Updates physics simulation including gravity, movement, jumping, and
// collision resolution
void FPSController::UpdatePhysics(
  float                           dt,
  std::span<const StaticCollider> colliders
) {
  // 1. Apply gravity
  body.velocity.y += Cfg.gravity * dt;

  // 2. Calculate horizontal movement with sprint and drag
  float speed = Cfg.move_speed;
  if (Input::GetKeyDown(GLFW_KEY_LEFT_SHIFT)) speed *= Cfg.sprint_mult;

  glm::vec3 target_h_vel = input_dir * speed;
  float     drag         = body.is_grounded ? Cfg.ground_drag : Cfg.air_drag;

  // Smooth horizontal velocity interpolation
  glm::vec3 h_vel(body.velocity.x, 0, body.velocity.z);
  h_vel           += (target_h_vel - h_vel) * std::min(drag * dt, 1.0f);
  body.velocity.x  = h_vel.x;
  body.velocity.z  = h_vel.z;

  // 3. Handle jumping
  if (want_jump && body.is_grounded) {
    body.velocity.y  = Cfg.jump_force;
    body.is_grounded = false;
    want_jump        = false;  // Reset jump flag after processing
  }

  // 4. Position integration using Semi-implicit Euler
  body.position += body.velocity * dt;

  // 5. Resolve world collisions
  ResolveCollisions(colliders);
}

void FPSController::ResolveCollisions(
  std::span<const StaticCollider> colliders
) {
  body.is_grounded = false;

  // Разрешаем коллизии за несколько итераций (для корректной обработки углов)
  for (int iteration = 0; iteration < 4; ++iteration) {
    AABB player_aabb = body.GetAABB();

    for (const auto& col : colliders) {
      const AABB& box = col.BoxBounds;

      // Быстрая проверка Broadphase
      if (!Collision::Intersect(player_aabb, box)) continue;

      // Вычисляем наложение (penetration depth) по всем 3 осям
      float overlap_x1 = box.max.x - player_aabb.min.x;
      float overlap_x2 = player_aabb.max.x - box.min.x;
      float overlap_x  = (overlap_x1 < overlap_x2) ? overlap_x1 : -overlap_x2;

      float overlap_y1 = box.max.y - player_aabb.min.y;
      float overlap_y2 = player_aabb.max.y - box.min.y;
      float overlap_y  = (overlap_y1 < overlap_y2) ? overlap_y1 : -overlap_y2;

      float overlap_z1 = box.max.z - player_aabb.min.z;
      float overlap_z2 = player_aabb.max.z - box.min.z;
      float overlap_z  = (overlap_z1 < overlap_z2) ? overlap_z1 : -overlap_z2;

      // Находим ось наименьшего проникновения
      float abs_x = std::abs(overlap_x);
      float abs_y = std::abs(overlap_y);
      float abs_z = std::abs(overlap_z);

      // Если выталкиваем по вертикали (Y)
      if (abs_y <= abs_x && abs_y <= abs_z) {
        if (overlap_y > 0.0f && body.velocity.y <= 0.0f) {
          // Игрок приземлился на объект
          body.position.y  += overlap_y;
          body.velocity.y   = 0.0f;
          body.is_grounded  = true;
        } else if (overlap_y < 0.0f && body.velocity.y > 0.0f) {
          // Удар головой о потолок
          body.position.y += overlap_y;
          body.velocity.y  = 0.0f;
        }
      }
      // Выталкивание по горизонтали (X или Z - стены)
      else if (abs_x <= abs_z) {
        body.position.x += overlap_x;
        body.velocity.x  = 0.0f;  // Гасим скорость в стену
      } else {
        body.position.z += overlap_z;
        body.velocity.z  = 0.0f;  // Гасим скорость в стену
      }

      // Обновляем AABB игрока для следующей проверки коллизий в текущей
      // итерации
      player_aabb = body.GetAABB();
    }
  }
}

// Synchronizes camera position with the player's eye height
void FPSController::ApplyToCamera(Camera3D& camera) const noexcept {
  camera.Position = body.GetEyePosition();
}
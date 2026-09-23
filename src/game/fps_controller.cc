#include "game/fps_controller.hpp"

#include "core/window.hpp"
#include "core/clock.hpp"
#include "core/input.hpp"
#include "scene/camera.hpp"
#include <algorithm>
#include <cmath>

// Initializes the controller with given config, position, and orientation
FPSController::FPSController(Config cfg, glm::vec3 start_pos, float start_yaw)
    : config(cfg) {
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

  camera.Rotation.y += static_cast<float>(dx) * config.mouse_sens;
  camera.Rotation.x -= static_cast<float>(dy) * config.mouse_sens;
  camera.Rotation.x  = std::clamp(camera.Rotation.x, -89.0f, 89.0f);

  front.x = std::sin(camera.Rotation.y);
  front.z = std::cos(camera.Rotation.y);
  if (glm::length(front) > 0.001f) front = glm::normalize(front);
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

  want_jump = Input::GetKeyPressed(GLFW_KEY_SPACE);
}

// Sets the world position of the character body
void FPSController::SetPosition(const glm::vec3& pos) noexcept {
  body.position = pos;
}

// Sets the controller's horizontal facing direction by yaw angle in degrees
void FPSController::SetYaw(float yaw_degrees) noexcept {
  front.x = std::sin(glm::radians(yaw_degrees));
  front.z = std::cos(glm::radians(yaw_degrees));
  if (glm::length(front) > 0.001f) { front = glm::normalize(front); }
}

// Updates physics simulation including gravity, movement, jumping, and
// collision resolution
void FPSController::UpdatePhysics(
  float                           dt,
  std::span<const StaticCollider> colliders
) {
  // 1. Apply gravity
  body.velocity.y += config.gravity * dt;

  // 2. Calculate horizontal movement with sprint and drag
  float speed = config.move_speed;
  if (Input::GetKeyDown(GLFW_KEY_LEFT_SHIFT)) speed *= config.sprint_mult;

  glm::vec3 target_h_vel = input_dir * speed;
  float     drag = body.is_grounded ? config.ground_drag : config.air_drag;

  // Smooth horizontal velocity interpolation
  glm::vec3 h_vel(body.velocity.x, 0, body.velocity.z);
  h_vel            += (target_h_vel - h_vel) * std::min(drag * dt, 1.0f);
  body.velocity.x  = h_vel.x;
  body.velocity.z  = h_vel.z;

  // 3. Handle jumping
  if (want_jump && body.is_grounded) {
    body.velocity.y  = config.jump_force;
    body.is_grounded = false;
    want_jump        = false;  // Reset jump flag after processing
  }

  // 4. Position integration using Semi-implicit Euler
  body.position += body.velocity * dt;

  // 5. Resolve world collisions
  ResolveCollisions(colliders);
}

// Performs broadphase (AABB) and narrowphase collision checks against static
// colliders
void FPSController::ResolveCollisions(
  std::span<const StaticCollider> colliders
) {
  body.is_grounded  = false;
  Capsule player_cap = body.GetCapsule();

  for (const auto& col : colliders) {
    // Broadphase: fast AABB check
    if (!Collision::Intersect(body.GetAABB(), col.BoxBounds)) continue;

    // Narrowphase: exact Capsule-AABB check
    if (Collision::Intersect(col.BoxBounds, player_cap)) {
      // Simple push-out collision resolution
      glm::vec3 center = (col.BoxBounds.min + col.BoxBounds.max) * 0.5f;
      glm::vec3 diff =
        body.position - center;  // Vector from box center to player feet

      // Check if landing on top of the collider
      if (body.velocity.y <= 0.0f
          && body.position.y >= col.BoxBounds.max.y - 0.1f
          && std::abs(diff.x) < (col.BoxBounds.extents().x + body.radius)
          && std::abs(diff.z) < (col.BoxBounds.extents().z + body.radius)) {
        body.position.y  = col.BoxBounds.max.y;
        body.velocity.y  = 0.0f;
        body.is_grounded = true;
      }
    }
  }
}

// Synchronizes camera position with the player's eye height
void FPSController::ApplyToCamera(Camera3D& camera) const noexcept {
  camera.Position = body.GetEyePosition();
}
#include "fly_controller.hpp"
#include "core/clock.hpp"
#include "core/input.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/geometric.hpp>


void FlyController::Update(Camera3D& camera) {
  float dt = Clock::DeltaTime();
  if (dt <= 0.0f) return;

  // --- 1. Управление ориентацией камеры (Мышь) ---

  double xoffset, yoffset;
  Input::GetCursorDelta(xoffset, yoffset);

  xoffset *= mouse_sensitivity;
  yoffset *= mouse_sensitivity;

  camera.Rotation.y += xoffset;  // Yaw
  camera.Rotation.x -= yoffset;  // Pitch

  // Ограничение угла обзора по вертикали
  camera.Rotation.x = std::clamp(camera.Rotation.x, -89.0f, 89.0f);

  // --- 2. Определение целевого направления движения (Клавиатура) ---
  glm::vec3 target_dir(0.0f);

  glm::vec3           front = camera.GetFront();
  glm::vec3           right = camera.GetRight();
  constexpr glm::vec3 world_up(0.0f, 1.0f, 0.0f);

  if (Input::GetKeyDown(GLFW_KEY_W)) target_dir += front;
  if (Input::GetKeyDown(GLFW_KEY_S)) target_dir -= front;
  if (Input::GetKeyDown(GLFW_KEY_D)) target_dir += right;
  if (Input::GetKeyDown(GLFW_KEY_A)) target_dir -= right;

  // Space - вверх, Shift - вниз
  if (Input::GetKeyDown(GLFW_KEY_SPACE)) target_dir += world_up;
  if (Input::GetKeyDown(GLFW_KEY_LEFT_SHIFT)
      || Input::GetKeyDown(GLFW_KEY_RIGHT_SHIFT))
    target_dir -= world_up;

  if (glm::length(target_dir) > 0.0f) {
    target_dir = glm::normalize(target_dir);
  }

  // --- 3. Плавное, но быстрое ускорение и торможение (Инерция) ---
  glm::vec3 target_velocity = target_dir * max_speed;

  // Быстрое сближение текущей скорости с целевой (нарастание)
  if (glm::length(target_dir) > 0.0f) {
    velocity += (target_velocity - velocity) * (acceleration * dt);
  } else {
    // Плавное затухание скорости при отпускании клавиш (трение)
    velocity -= velocity * (friction * dt);
  }

  // --- 4. Применение перемещения к камере ---
  camera.Position += velocity * dt;
}
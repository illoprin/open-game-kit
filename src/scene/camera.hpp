#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

constexpr float CAM_NEAR = 0.01;
constexpr float CAM_FAR  = 1000.0;

class Camera3D {
  glm::mat4 proj, view;
  glm::vec3 front, right, up;

public:

  glm::vec3 Position = glm::vec3(0.0);              // x, y, z
  glm::vec3 Rotation = glm::vec3(0.f, -90.f, 0.f);  // pitch, yaw, roll

  float Fov = 90.f;

  void Update(glm::ivec2 screen_size);

  glm::mat4 GetProjection() const noexcept;
  glm::mat4 GetView() const noexcept;

  glm::vec3 GetFront() const noexcept;
  glm::vec3 GetRight() const noexcept;
};
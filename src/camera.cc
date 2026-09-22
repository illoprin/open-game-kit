#include "camera.hpp"

#include "camera.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

void Camera3D::Update(glm::ivec2 screen_size) {
  float pitch = glm::radians(this->Rotation.x);
  float yaw   = glm::radians(this->Rotation.y);
  float roll  = glm::radians(this->Rotation.z);


  // compute front
  glm::vec3 f = {cosf(pitch) * cosf(yaw), sinf(pitch), sinf(yaw) * cosf(pitch)};
  front       = glm::normalize(f);

  // compute right and up vectors
  glm::vec3 worldUp = glm::vec3{0, 1, 0};
  right             = glm::normalize(glm::cross(front, worldUp));
  up                = glm::normalize(glm::cross(right, front));

  if (roll != 0) {
    // rotate vectors by roll
    float c = cosf(roll);
    float s = sinf(roll);

    right = glm::normalize(right * c + glm::cross(front, right) * s);
    up    = glm::normalize(glm::cross(right, front));
  }

  // compute matrices
  view = glm::lookAt(Position, Position + front, up);
  float aspect =
    static_cast<float>(screen_size.x) / static_cast<float>(screen_size.y);
  proj = glm::perspective(glm::radians(Fov), aspect, CAM_NEAR, CAM_FAR);
}

glm::mat4 Camera3D::GetProjection() const noexcept {
  return proj;
}

glm::mat4 Camera3D::GetView() const noexcept {
  return view;
}

glm::vec3 Camera3D::GetFront() const noexcept {
  return front;
}

glm::vec3 Camera3D::GetRight() const noexcept {
  return right;
}
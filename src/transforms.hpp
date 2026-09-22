#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"

struct Transform {
  glm::vec3 Position;
  glm::vec3 Rotation;
  glm::vec3 Scale;
};

inline glm::mat4 CreateModel(const Transform& transform) {
  glm::mat4 model = glm::mat4(1.0f);

  // Apply translation
  model = glm::translate(model, transform.Position);

  // Apply rotations (typically in ZYX or XYZ order, here applying pitch, yaw,
  // roll)
  if (transform.Rotation.x != 0.0f)
    model = glm::rotate(model, glm::radians(transform.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  if (transform.Rotation.y != 0.0f)
    model = glm::rotate(model, glm::radians(transform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  if (transform.Rotation.z != 0.0f)
    model = glm::rotate(model, glm::radians(transform.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
  // Apply scale
  model = glm::scale(model, transform.Scale);

  return model;
}

inline glm::mat4 CreateModel(glm::vec3 pos, glm::vec3 scl) {
  glm::mat4 model = glm::mat4(1.0f);

  // Apply translation
  model = glm::translate(model, pos);

  // Apply scale
  model = glm::scale(model, scl);

  return model;
}
#pragma once

#include <glm/vec4.hpp>

struct PointLight {
  glm::vec4 Position; // xyz: pos, w: radius
  glm::vec4 Color; // rgb: color, a: intensity
};

struct SpotLight {
  glm::vec4 Position; // xyz: pos, w: radius
  glm::vec4 Color; // rgb: color, a: intensity
  glm::vec4 Dir; // xyz: direction vector, w: padding
  glm::vec4 Data; // xy: cos outer, inner; z: smoothness; w: padding 
};
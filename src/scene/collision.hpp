#pragma once

#include <array>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <span>

struct AABB {
  glm::vec3 min{0.f, 0.f, 0.f};
  glm::vec3 max{0.f, 0.f, 0.f};

  constexpr glm::vec3 center() const {
    return (min + max) * 0.5f;
  }

  constexpr glm::vec3 extents() const {
    return (max - min) * 0.5f;
  }
};

struct Sphere {
  glm::vec3 center{0.f, 0.f, 0.f};
  float     radius{0.f};
};

struct Line {
  glm::vec3 start{0.f, 0.f, 0.f};
  glm::vec3 end{0.f, 0.f, 0.f};
};

struct Capsule {
  glm::vec3 a{0.f, 0.f, 0.f};
  glm::vec3 b{0.f, 0.f, 0.f};
  float     radius{0.f};
};

struct Frustum {
  // 6 planes: Left, Right, Bottom, Top, Near, Far (Normal . p + d >= 0)
  std::array<glm::vec4, 6> planes;

  bool ContainsSphere(const Sphere& s) const;

  static Frustum FromPerspective(
    float            fovY,
    float            aspect,
    float            zNear,
    float            zFar,
    const glm::mat4& viewMatrix
  );
};

struct StaticCollider {
  AABB   BoxBounds;
  Sphere SphereBounds;
  // optional: material id for steps sound
};

namespace Collision {

// Collision detection functions
bool Intersect(const AABB& a, const AABB& b);
bool Intersect(const AABB& aabb, const Sphere& sphere);
bool Intersect(const AABB& aabb, const Line& line);
bool Intersect(const AABB& aabb, const Capsule& capsule);

}  // namespace Collision

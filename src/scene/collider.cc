#include "collider.hpp"
#include <algorithm>

namespace Collider {

AABB ComputeAABB(std::span<const glm::vec3> vertices) noexcept {
  if (vertices.empty()) return {glm::vec3{0.0f}, glm::vec3{0.0f}};

  glm::vec3 min_vert = vertices[0];
  glm::vec3 max_vert = vertices[0];

  for (const auto& v : vertices) {
    min_vert = glm::min(min_vert, v);
    max_vert = glm::max(max_vert, v);
  }

  return {min_vert, max_vert};
}

AABB ComputeAABB(const Geometry& geo) noexcept {
  if (geo.Vertices.empty()) return {glm::vec3{0.f}, glm::vec3{0.f}};

  glm::vec3 min_v = geo.Vertices[0].Position;
  glm::vec3 max_v = min_v;

  for (const auto& vert : geo.Vertices) {
    min_v = glm::min(min_v, vert.Position);
    max_v = glm::max(max_v, vert.Position);
  }
  return {min_v, max_v};
}

Sphere ComputeSphere(const Geometry& geo) noexcept {
  if (geo.Vertices.empty()) { return Sphere{glm::vec3(0.f), 0.f}; }

  // Compute center as the average of all vertex positions
  glm::vec3 sum(0.f);
  for (const auto& v : geo.Vertices) {
    sum += v.Position;
  }
  glm::vec3 center = sum / static_cast<float>(geo.Vertices.size());

  // Find the maximum distance from the center to any vertex
  float maxRadiusSq = 0.f;
  for (const auto& v : geo.Vertices) {
    glm::vec3 diff   = v.Position - center;
    float     distSq = glm::dot(diff, diff);
    if (distSq > maxRadiusSq) maxRadiusSq = distSq;
  }

  return Sphere{center, std::sqrt(maxRadiusSq)};
}

// transform an AABB by a matrix (approximate or precise via corners)
AABB TransformAABB(
  const AABB&      localAABB,
  const glm::mat4& modelMatrix
) noexcept {
  glm::vec3 min = localAABB.min;
  glm::vec3 max = localAABB.max;

  glm::vec3 corners[8] = {
    {min.x, min.y, min.z},
    {max.x, min.y, min.z},
    {min.x, max.y, min.z},
    {max.x, max.y, min.z},
    {min.x, min.y, max.z},
    {max.x, min.y, max.z},
    {min.x, max.y, max.z},
    {max.x, max.y, max.z}
  };

  glm::vec3 worldMin(std::numeric_limits<float>::max());
  glm::vec3 worldMax(std::numeric_limits<float>::lowest());

  for (const auto& corner : corners) {
    glm::vec3 worldCorner = glm::vec3(modelMatrix * glm::vec4(corner, 1.0f));
    worldMin              = glm::min(worldMin, worldCorner);
    worldMax              = glm::max(worldMax, worldCorner);
  }

  return AABB{worldMin, worldMax};
}

// transform a Sphere by a matrix (accounting for max scale component)
Sphere TransformSphere(
  const Sphere&    localSphere,
  const glm::mat4& modelMatrix
) noexcept {
  glm::vec3 worldCenter =
    glm::vec3(modelMatrix * glm::vec4(localSphere.center, 1.0f));

  // Extract scale from model matrix to scale the radius properly
  glm::vec3 scale;
  scale.x        = glm::length(glm::vec3(modelMatrix[0]));
  scale.y        = glm::length(glm::vec3(modelMatrix[1]));
  scale.z        = glm::length(glm::vec3(modelMatrix[2]));
  float maxScale = std::max({scale.x, scale.y, scale.z});

  return Sphere{worldCenter, localSphere.radius * maxScale};
}

}  // namespace Collider

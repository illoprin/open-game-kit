#pragma once

#include "scene/geometry.hpp"
#include <span>
#include "collision.hpp"


namespace Collider {

/**
 * @brief Строит AABB по облаку точек.
 * @complexity O(N)
 */
[[nodiscard]]
AABB ComputeAABB(std::span<const glm::vec3> vertices) noexcept;

[[nodiscard]]
AABB ComputeAABB(const Geometry& geo) noexcept;

[[nodiscard]]
Sphere ComputeSphere(const Geometry& geo) noexcept;

[[nodiscard]]
AABB TransformAABB(
  const AABB&      localAABB,
  const glm::mat4& modelMatrix
) noexcept;

[[nodiscard]]
Sphere TransformSphere(
  const Sphere&    localSphere,
  const glm::mat4& modelMatrix
) noexcept;

}  // namespace Collider
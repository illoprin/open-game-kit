#include "physics.hpp"
#include "scene/transforms.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "scene/collider.hpp"

void PhysicsWorld::FromMap(const MapData& mapData, const MapRepository& repository) {
  colliders.clear();
  colliders.reserve(mapData.Instances.size());

  for (const auto& inst : mapData.Instances) {
    // 1. Find the base collider for this geometry (fallback to "cube" if not found)
    auto it = repository.GeometryColliders.find(inst.GeometryID);
    const StaticCollider* baseCollider = nullptr;
    if (it != repository.GeometryColliders.end()) {
      baseCollider = &it->second;
    } else {
      auto cubeIt = repository.GeometryColliders.find("cube");
      if (cubeIt != repository.GeometryColliders.end()) {
        baseCollider = &cubeIt->second;
      }
    }

    if (!baseCollider) continue;

    // 2. Create model matrix for the instance
    glm::mat4 modelMatrix = CreateModel({
      inst.position,
      inst.rotation,
      inst.scale,
    });

    // 3. Transform local collider bounds into world space
    StaticCollider worldCollider;
    worldCollider.BoxBounds = Collider::TransformAABB(baseCollider->BoxBounds, modelMatrix);
    worldCollider.SphereBounds = Collider::TransformSphere(baseCollider->SphereBounds, modelMatrix);

    colliders.push_back(worldCollider);
  }
}

const std::vector<StaticCollider>& PhysicsWorld::StaticColliders() const {
  return colliders;
}
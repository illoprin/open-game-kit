#include "map_repository.hpp"

#include "core/files.hpp"
#include "scene/collider.hpp"
#include "core/log.hpp"

bool MapRepository::FromData(const MapData& mapData) noexcept {
  // 1. Load images (textures)
  for (const auto& [id, path] : mapData.Textures) {
    Image2D img;
    if (!img.FromFile(path)) {
      log(LogLevel::Error, "MapRepository: failed to load image: {}", path);
      return false;
    }
    Images[id] = std::move(img);
  }

  // 2. Load default primitives geometries & colliders
  {
    Geometry cubeGeo          = Geometry::CreateCube(glm::vec3(2.0f));
    GeometryColliders["cube"] = StaticCollider{
      Collider::ComputeAABB(cubeGeo),
      Collider::ComputeSphere(cubeGeo)
    };
    Geometries.insert_or_assign("cube", std::move(cubeGeo));

    Geometry planeGeo          = Geometry::CreatePlane(glm::vec2(2.0f));
    GeometryColliders["plane"] = StaticCollider{
      Collider::ComputeAABB(planeGeo),
      Collider::ComputeSphere(planeGeo)
    };
    Geometries.insert_or_assign("plane", std::move(planeGeo));
  }

  // 3. Load external geometries (.obj) and build colliders
  for (const auto& [id, path] : mapData.Geometries) {
    auto geoRes = Geometry::FromObj(path);
    if (!geoRes.has_value()) {
      log(
        LogLevel::Error,
        "MapRepository: failed to load model {}: {}",
        path,
        geoRes.error()
      );
      return false;
    }

    Geometry& geo =
      Geometries.emplace(id, std::move(geoRes.value())).first->second;

    // Build and store static collider for this mesh
    GeometryColliders[id] = StaticCollider{
      Collider::ComputeAABB(geo),
      Collider::ComputeSphere(geo)
    };
  }

  log(
    LogLevel::Success,
    "MapRepository: successfully loaded map '{}' resources (geometries: {}, "
    "images: {}, colliders: {}).",
    mapData.Name,
    Geometries.size(),
    Images.size(),
    GeometryColliders.size()
  );

  return true;
}
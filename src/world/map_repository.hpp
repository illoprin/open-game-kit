#pragma once

#include "gfx/image.hpp"
#include "scene/collision.hpp"
#include "scene/geometry.hpp"
#include "world/map_parser.hpp"
#include <unordered_map>

struct MapRepository {

  // Load map data to RAM
  // Returns false if error ocurred while loading
  bool FromData(const MapData&) noexcept;
  
  std::unordered_map<std::string, Geometry>       Geometries;
  std::unordered_map<std::string, Image2D>        Images;
  std::unordered_map<std::string, StaticCollider> GeometryColliders;
};
#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct MapMaterial {
  std::string ID;
  std::string DiffuseID;
  std::string EmissiveID;
  glm::vec3   Tint;
};

struct MapInstance {
  std::string GeometryID;  // "cube", "plane", "crate", etc.
  std::string MaterialID;
  float       UVScaling;
  glm::vec3   position;
  glm::vec3   scale{1.0};
  glm::vec3   rotation;
};

enum class LightType {
  Point = 0,
  Directional
};

struct LightEntry {
  LightType Type;
  glm::vec3 Position;
  float Intensity;
  float Radius;
  glm::vec3 Color;
  glm::vec3 Direction;
};

struct MapData {
  std::string                                  Name;
  std::unordered_map<std::string, std::string> Textures;
  std::unordered_map<std::string, std::string> Geometries;
  std::unordered_map<std::string, MapMaterial> Materials;
  std::vector<MapInstance>                     Instances;
  std::vector<LightEntry> Lights;
  bool LoadGameMap(const std::string& filepath) noexcept;
};

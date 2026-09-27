#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

enum class LightType {
  Point = 0,
  Spot
};

struct MapData {

  struct material {
    std::string ID;
    std::string DiffuseID;
    std::string EmissiveID;
    glm::vec3   Tint;
  };

  struct instance {
    std::string GeometryID;  // "cube", "plane", "crate", etc.
    std::string MaterialID;
    float       UVScaling;
    glm::vec3   position;
    glm::vec3   scale{1.0};
    glm::vec3   rotation;
  };

  struct light {
    LightType Type;
    glm::vec3 Position;
    float     Intensity;
    float     Radius;
    glm::vec3 Color;
    glm::vec3 Direction;
  };

  struct sun {
    glm::vec3 Direction{0.0};
    glm::vec3 Color{0.0};
    float     Intensity = 0.0;
  };

  struct ambient {
    float     Intensity = 0.0;
    glm::vec3 Color{0.0};
  };

  std::string                                  Name;
  std::unordered_map<std::string, std::string> Textures;
  std::unordered_map<std::string, std::string> Geometries;
  std::unordered_map<std::string, material>    Materials;
  std::vector<instance>                        Instances;
  std::vector<light>                           Lights;
  sun                                          Sun{};
  ambient                                      Amb;
  bool LoadGameMap(const std::string& filepath) noexcept;
};

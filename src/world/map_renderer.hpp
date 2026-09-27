#pragma once

#include "gfx/post/lights.hpp"
#include "map_parser.hpp"
#include "gfx/mesh.hpp"
#include "gfx/texture.hpp"
#include "gfx/program.hpp"
#include "scene/camera.hpp"
#include "world/map_repository.hpp"
#include <unordered_map>
#include <string>

class MapRenderer {
public:
  MapRenderer() = default;
  ~MapRenderer() = default;

  // init gpu objects and render items from Map Data and Loaded Assets
  void Init(const MapData&, const MapRepository&) noexcept;

  // Render all map items
  void Render(const Camera3D& cam) const;

  MapRenderer(const MapRenderer&) = delete;
  MapRenderer& operator=(const MapRenderer&) = delete;
  MapRenderer(MapRenderer&&) noexcept = default;
  MapRenderer& operator=(MapRenderer&&) noexcept = default;

  const std::vector<PointLight>& GetPointLights();
  const std::vector<SpotLight>& GetSpotLights();

private:
  // RenderItem represents each instance view
  struct RenderItem {
    const Mesh* mesh = nullptr;
    const Texture2D* texture = nullptr;
    float uvScale = 1.0;
    bool triplanar = false;
    glm::vec3 tint{1.f};
    glm::mat4 model{1.f};
  };

  // scene lighting
  MapData::sun sun{};
  MapData::ambient ambient{};
  std::vector<PointLight> pointLights;
  std::vector<SpotLight> spotLights;

  Program prog;

  std::vector<RenderItem> renderItems;
  
  // GPU resources
  std::unordered_map<std::string, Texture2D> textures;
  std::unordered_map<std::string, Mesh>      meshes;
};
#pragma once

#include "map_parser.hpp"
#include "gfx/mesh.hpp"
#include "gfx/texture.hpp"
#include "gfx/program.hpp"
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
  void Render(const Program& program) const;

  MapRenderer(const MapRenderer&) = delete;
  MapRenderer& operator=(const MapRenderer&) = delete;
  MapRenderer(MapRenderer&&) noexcept = default;
  MapRenderer& operator=(MapRenderer&&) noexcept = default;

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

  std::vector<RenderItem> renderItems;
  
  // GPU resources
  std::unordered_map<std::string, Texture2D> textures;
  std::unordered_map<std::string, Mesh>      meshes;
};
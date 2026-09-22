#pragma once

#include "map_parser.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "program.hpp"
#include <unordered_map>
#include <string>

class MapRenderer {
public:
  MapRenderer() = default;
  ~MapRenderer() = default;

  // Инициализация, загрузка текстур и геометрий по данным карты
  bool Initialize(const MapData& mapData);

  // Отрисовка всех инстансов карты
  void Render(const Program& program) const;

  MapRenderer(const MapRenderer&) = delete;
  MapRenderer& operator=(const MapRenderer&) = delete;
  MapRenderer(MapRenderer&&) noexcept = default;
  MapRenderer& operator=(MapRenderer&&) noexcept = default;

private:
  struct RenderItem {
    const Mesh* mesh = nullptr;
    const Texture2D* texture = nullptr;
    float uvScale = 1.0;
    bool triplanar = false;
    glm::mat4 model{1.0f};
  };

  std::vector<RenderItem> renderItems;
  
  // Кэш ресурсов, чтобы не дублировать загрузку
  std::unordered_map<std::string, Texture2D> textures;
  std::unordered_map<std::string, Mesh>      meshes;
};
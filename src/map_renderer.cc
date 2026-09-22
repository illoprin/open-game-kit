#include "map_renderer.hpp"
#include "files.hpp"
#include "gl_state.hpp"
#include "image.hpp"
#include "geometry.hpp"
#include "log.hpp"
#include "transforms.hpp"
#include <glm/gtc/matrix_transform.hpp>

bool MapRenderer::Initialize(const MapData& mapData) {
  // 1. Загрузка текстур
  Image2D img;
  for (const auto& [id, path] : mapData.Textures) {
    if (!img.FromFile(path)) {
      log(LogLevel::Error, "MapRenderer: failed to load texture: {}", path);
      return false;
    }
    Texture2D& tex = textures[id];
    tex.FromData(img.Pix(), img.Width(), img.Height(), GL_RGB8);
    tex.GenerateMipmaps();
    tex.SetSamplerState(GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR);
  }

  // 2. Загрузка геометрии (включая внешние .obj и дефолтный cube)
  // Создадим дефолтный куб единичного размера [-1..1] или под размер
  meshes["cube"].FromGeometry(Geometry::CreateCube(glm::vec3(2.0f)));
  meshes["plane"].FromGeometry(Geometry::CreatePlane(glm::vec3(2.0f)));

  for (const auto& [id, path] : mapData.Geometries) {
    auto geoRes = Geometry::FromObj(path);
    if (!geoRes.has_value()) {
      log(LogLevel::Error, "MapRenderer: failed to load model {}: {}", path, geoRes.error());
      return false;
    }
    meshes[id].FromGeometry(geoRes.value());
  }

  // 3. Подготовка RenderItem'ов для быстрого рендеринга инстансов
  for (const auto& inst : mapData.Instances) {
    RenderItem item;

    // Находим меш (по умолчанию если геометрия не найдена, пробуем "cube")
    auto meshIt = meshes.find(inst.GeometryID);
    if (meshIt != meshes.end()) {
      item.mesh = &meshIt->second;
    } else {
      item.mesh = &meshes["cube"];
    }

    // Находим текстуру через материалы
    auto matIt = mapData.Materials.find(inst.MaterialID);
    if (matIt != mapData.Materials.end()) {
      auto texIt = textures.find(matIt->second.DiffuseID);
      if (texIt != textures.end()) {
        item.texture = &texIt->second;
      }
    }

    // create model matrix
    item.model = CreateModel({
      inst.position, inst.rotation, inst.scale,
    });
    item.uvScale = inst.UVScaling;

    // set triplanar if flat primitive
    if (inst.GeometryID == "cube" || inst.GeometryID == "plane") item.triplanar = true;

    renderItems.push_back(item);
  }

  log(LogLevel::Info, "MapRenderer: initialized '{}' with {} instances.", mapData.Name, renderItems.size());
  return true;
}

void MapRenderer::Render(const Program& program) const {
  for (const auto& item : renderItems) {
    if (!item.mesh) continue;

    if (item.texture) {
      item.texture->Bind(0);
    }
    
    program.SetInt("u_use_triplanar",  static_cast<int>(item.triplanar));

    program.SetMat4("u_model", item.model);
    program.SetFloat("u_uv_scaling", item.uvScale);

    GL::DrawElements(item.mesh->GetVAO(), item.mesh->GetIndexCount(), GL_UNSIGNED_INT);
  }
}
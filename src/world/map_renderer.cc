#include "map_renderer.hpp"

#include "core/log.hpp"
#include "gfx/gl.hpp"
#include "scene/transforms.hpp"
#include <glm/gtc/matrix_transform.hpp>


void MapRenderer::Init(
  const MapData&       mapData,
  const MapRepository& repository
) noexcept {
  // 1. Upload textures to GPU using pre-loaded images from MapRepository
  for (const auto& [id, img] : repository.Images) {
    Texture2D& tex = textures[id];
    tex.FromData(img.Pix(), img.Width(), img.Height(), GL_RGB8);
    tex.GenerateMipmaps();
    tex.SetSamplerState(GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR);
  }

  // 2. Upload geometries to GPU meshes from MapRepository
  for (const auto& [id, geo] : repository.Geometries) {
    meshes[id].FromGeometry(geo);
  }

  // 3. Prepare RenderItems for fast instance rendering
  for (const auto& inst : mapData.Instances) {
    RenderItem item;

    // Find mesh (fallback to "cube" if geometry ID is not found)
    auto meshIt = meshes.find(inst.GeometryID);
    if (meshIt != meshes.end()) {
      item.mesh = &meshIt->second;
    } else {
      auto cubeIt = meshes.find("cube");
      if (cubeIt != meshes.end()) { item.mesh = &cubeIt->second; }
    }

    // Find texture through materials
    auto matIt = mapData.Materials.find(inst.MaterialID);
    if (matIt != mapData.Materials.end()) {
      auto texIt = textures.find(matIt->second.DiffuseID);
      if (texIt != textures.end()) { item.texture = &texIt->second; }
      item.tint = matIt->second.Tint;
    }

    // Create model matrix
    item.model   = CreateModel({
      inst.position,
      inst.rotation,
      inst.scale,
    });
    item.uvScale = inst.UVScaling;

    // Set triplanar if flat primitive
    if (inst.GeometryID == "cube" || inst.GeometryID == "plane") {
      item.triplanar = true;
    }

    renderItems.push_back(item);
  }

  log(
    LogLevel::Info,
    "MapRenderer: initialized '{}' with {} instances.",
    mapData.Name,
    renderItems.size()
  );
}

void MapRenderer::Render(const Program& program) const {
  for (const auto& item : renderItems) {
    if (!item.mesh) continue;

    program.SetInt("u_use_diffuse", static_cast<int>(item.texture == nullptr));
    if (item.texture) { item.texture->Bind(0); }

    program.SetInt("u_use_triplanar", static_cast<int>(item.triplanar));

    program.SetMat4("u_model", item.model);
    program.SetFloat("u_uv_scaling", item.uvScale);

    program.SetVec3("u_tint", item.tint);

    GL::DrawElements(
      item.mesh->GetVAO(),
      item.mesh->GetIndexCount(),
      GL_UNSIGNED_INT
    );
  }
}
#include "map_renderer.hpp"

#include "core./log.hpp"
#include "core/files.hpp"
#include "gfx/gl.hpp"
#include "gfx/program.hpp"
#include "scene/transforms.hpp"
#include <glm/gtc/matrix_transform.hpp>

void MapRenderer::Init(
  const MapData&       mapData,
  const MapRepository& repository
) noexcept {

  og_assert(
    Program::FastLoad(
      prog,
      ShaderPath("g_map.vert"),
      ShaderPath("g_map.frag")
    ),
    "failed load map redering shaders"
  );

  sun     = mapData.Sun;
  ambient = mapData.Amb;

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

void MapRenderer::Render(const Camera3D& cam) const {

  prog.Use();
  prog.SetMat4("u_projection", cam.GetProjection());
  prog.SetMat4("u_view", cam.GetView());
  prog.SetInt("u_diffuse", 0);

  // scene lighting
  prog.SetVec3("u_sun_direction", sun.Direction);
  prog.SetVec3("u_sun_color", sun.Color);
  prog.SetFloat("u_sun_intensity", sun.Intensity);

  prog.SetVec3("u_ambient_color", ambient.Color);
  prog.SetFloat("u_ambient_intensity", ambient.Intensity);

  for (const auto& item : renderItems) {
    if (!item.mesh) continue;

    prog.SetInt("u_use_diffuse", static_cast<int>(item.texture != nullptr));
    if (item.texture) { item.texture->Bind(0); }

    prog.SetInt("u_use_triplanar", static_cast<int>(item.triplanar));

    prog.SetMat4("u_model", item.model);
    prog.SetFloat("u_uv_scaling", item.uvScale);

    prog.SetVec3("u_tint", item.tint);

    GL::DrawElements(
      item.mesh->GetVAO(),
      item.mesh->GetIndexCount(),
      GL_UNSIGNED_INT
    );
  }
}
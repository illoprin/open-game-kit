#pragma once


// -------------------------------------------------------------------
//                       Interface
// -------------------------------------------------------------------

#include "gfx/mesh.hpp"
#include "gfx/post/lights.hpp"
#include "gfx/program.hpp"
#include "gfx/render_target.hpp"
#include "gfx/texture.hpp"

struct PostProcessingContext {
  const RenderTarget2D* Destination = nullptr;
  const Texture2D*  Color       = nullptr;
  const Texture2D*  Normal      = nullptr;
  const Texture2D*  Depth       = nullptr;
  const Mesh*           BasicQuad   = nullptr;

  glm::vec3            CamPos;
  glm::mat4            View;
  glm::mat4            Proj;
  const Texture2D* Noise = nullptr;

  const std::vector<PointLight>* PointLights = nullptr;
  const std::vector<SpotLight>*  SpotLights  = nullptr;
};

class IPostEffect {
protected:

  Program prog;
  char        name[32] = "default_pass_name";

public:

  IPostEffect() = default;

  virtual ~IPostEffect() {
  }

  IPostEffect(const IPostEffect&)             = delete;
  IPostEffect& operator =(const IPostEffect&) = delete;

  virtual bool Use() const {
    return false;
  }

  const char* GetName() const {
    return name;
  }

  virtual void DrawConfigUI() noexcept {
  }

  virtual void Resize(glm::ivec2 new_size) noexcept {
  }

  virtual void RenderPass(const PostProcessingContext& ctx) noexcept {
  }
};

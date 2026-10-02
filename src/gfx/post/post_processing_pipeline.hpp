#pragma once

#include "gfx/framebuffer.hpp"
#include "gfx/mesh.hpp"
#include "gfx/post/light_pass.hpp"
#include "gfx/post/ping_pong_buffer.hpp"
#include "gfx/post/post_effect.hpp"
#include "gfx/post/vignette_pass.hpp"
#include "scene/camera.hpp"
#include <vector>

class PostProcessingPipeline {
public:

  PostProcessingPipeline(glm::ivec2 screenSize);

  const Framebuffer& Perform(
    const Mesh& basicQuad,
    const GBuffer&,
    const Camera3D&,
    const std::vector<PointLight>&,
    const std::vector<SpotLight>&
  ) noexcept;

  void DrawUI() noexcept;

  void Resize(glm::ivec2 newSize) noexcept;

  PostProcessingPipeline& operator =(const PostProcessingPipeline&) = delete;
  PostProcessingPipeline(const PostProcessingPipeline&)             = delete;

  ~PostProcessingPipeline() = default;

private:

  void pass(uint& index, IPostEffect& postEffect) noexcept;

  PingPongBuffer        buffer;
  PostProcessingContext ctx;

  // passes

  LightPass    light;
  VignettePass vignette{VignetteConfig{}};
};
#include "post_processing_pipeline.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/post/ping_pong_buffer.hpp"
#include "gfx/resource.hpp"

PostProcessingPipeline::PostProcessingPipeline(glm::ivec2 screenSize)
    : buffer(screenSize) {
}

void PostProcessingPipeline::pass(uint& idx, IPostEffect& fx) noexcept {
  if (!fx.Use()) return;

  const auto& read_target = buffer.GetReadBuffer();

  ctx.Destination = &buffer.GetWriteBuffer();
  ctx.Color       = idx == 0 ? ctx.Color : &read_target.GetColor();

  fx.RenderPass(ctx);

  ++idx;

  buffer.Swap();
}

const Framebuffer& PostProcessingPipeline::Perform(
  const Mesh&     basicQuad,
  const GBuffer&  gBuffer,
  const Camera3D& cam
  // TODO std::span for lights
) noexcept {

  ctx.BasicQuad = &basicQuad;
  ctx.CamPos    = cam.Position;
  ctx.View      = cam.GetView();
  ctx.Proj      = cam.GetProjection();
  ctx.Normal    = &gBuffer.GetNormal();
  ctx.Depth     = &gBuffer.GetDepth();
  ctx.Color     = &gBuffer.GetDiffuse();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  uint idx = 0;

  pass(idx, vignette);

  return buffer.GetReadBuffer().GetFramebuffer();
}

void PostProcessingPipeline::DrawUI() noexcept {
  vignette.DrawConfigUI();
}

void PostProcessingPipeline::Resize(glm::ivec2 newSize) noexcept {
  buffer.Resize(newSize);
}

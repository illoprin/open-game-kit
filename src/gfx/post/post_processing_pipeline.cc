#include "post_processing_pipeline.hpp"
#include "core/engine.hpp"
#include "core/window.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/post/ping_pong_buffer.hpp"
#include "gfx/resource.hpp"
#include "imgui/imgui.h"

PostProcessingPipeline::PostProcessingPipeline(glm::ivec2 screenSize)
    : buffer(screenSize) {
}

void PostProcessingPipeline::pass(uint& idx, IPostEffect& fx) noexcept {
  if (!fx.Use()) return;

  const auto& read_target = buffer.GetReadBuffer();

  ctx.Destination = &buffer.GetWriteBuffer();
  ctx.Color       = idx == 0 ? ctx.Color : &read_target.GetColor();

  fx.RenderPass(ctx);

  idx++;

  buffer.Swap();
}

const Framebuffer& PostProcessingPipeline::Perform(
  const Mesh&                    basicQuad,
  const GBuffer&                 gBuffer,
  const Camera3D&                cam,
  const std::vector<PointLight>& pointLights,
  const std::vector<SpotLight>&  spotLights
) noexcept {

  ctx.BasicQuad   = &basicQuad;
  ctx.CamPos      = cam.Position;
  ctx.View        = cam.GetView();
  ctx.Proj        = cam.GetProjection();
  ctx.Normal      = &gBuffer.GetNormal();
  ctx.Depth       = &gBuffer.GetDepth();
  ctx.Color       = &gBuffer.GetDiffuse();
  ctx.PointLights = &pointLights;
  ctx.SpotLights  = &spotLights;

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  uint idx = 0;

  pass(idx, light);
  pass(idx, vignette);

  if (idx == 0) return gBuffer.GetFramebuffer();

  return buffer.GetReadBuffer().GetFramebuffer();
}

void PostProcessingPipeline::DrawUI() noexcept {
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus
                           | ImGuiWindowFlags_NoResize;

  glm::ivec2 win_size   = Window::Size();
  float      ui_padding = Engine::GetConfig().UIPadding;
  ImGui::SetNextWindowPos({win_size.x - ui_padding, ui_padding}, 0, {1.0, 0});
  ImGui::SetNextWindowSize(
    {static_cast<float>(static_cast<float>(win_size.x) / 4.0),
     win_size.y - ui_padding * 2}
  );

  ImGui::Begin("Post Effects", nullptr, flags);
  // ImGui::SeparatorText("Core");

  // ImGui::SeparatorText("Light");

  ImGui::SeparatorText("Cosmetic");
  if (ImGui::CollapsingHeader(vignette.GetName())) vignette.DrawConfigUI();
  ImGui::End();
}

void PostProcessingPipeline::Resize(glm::ivec2 newSize) noexcept {
  buffer.Resize(newSize);
}

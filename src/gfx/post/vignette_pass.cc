
#include "vignette_pass.hpp"
#include "core./log.hpp"
#include "core./window.hpp"
#include "core/files.hpp"
#include "gfx/program.hpp"
#include "imgui/imgui.h"
#include <cstring>

VignettePass::VignettePass(const VignetteConfig& _cfg) : cfg(_cfg) {
  std::strcpy(name, "vignette");
  og_assert(
    Program::FastLoad(
      prog,
      ShaderPath("post/screen.vert"),
      ShaderPath("post/vignette.frag")
    ),
    "failed to load vignette program"
  );
}

void VignettePass::DrawConfigUI() noexcept {
  ImGui::Checkbox("Use##vig", &cfg.Use);
  ImGui::SliderFloat("Radius##vig", &cfg.Radius, 0.2, 2);
  ImGui::SliderFloat("Softness##vig", &cfg.Softness, 0.1, 1);
}

void VignettePass::RenderPass(const PostProcessingContext& ctx) noexcept {
  ctx.Destination->BindForDrawing(GL_COLOR_BUFFER_BIT);

  prog.Use();

  ctx.Color->Bind(0);
  prog.SetInt("u_color", 0);

  prog.SetFloat("u_radius", cfg.Radius);
  prog.SetFloat("u_softness", cfg.Softness);

  ctx.BasicQuad->Draw();
}
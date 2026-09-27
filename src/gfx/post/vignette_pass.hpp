
#pragma once

#include "gfx/post/post_effect.hpp"

struct VignetteConfig {
  float Softness = 0.53;
  float Radius   = 0.9;
  bool Use = true;
};

class VignettePass : public IPostEffect {
  VignetteConfig cfg;

public:

  VignettePass(const VignetteConfig&);
  ~VignettePass() override = default;

  bool Use() const override {
    return cfg.Use;
  }

  void DrawConfigUI()  noexcept override;
  void RenderPass(const PostProcessingContext& ctx) noexcept override;
};
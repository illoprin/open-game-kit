#pragma once

#include "gfx/buffer.hpp"
#include "gfx/post/post_effect.hpp"

class LightPass : public IPostEffect {
public:

  LightPass();
  ~LightPass() override = default;

  bool Use() const override {
    return true;
  }

  void RenderPass(const PostProcessingContext& ctx) noexcept override;

private:

  Buffer lightBuffer;
};

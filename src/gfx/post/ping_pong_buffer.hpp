#pragma once

#include "gfx/render_target.hpp"

class PingPongBuffer {
  std::array<RenderTarget2D, 2> buffers;
  uch                           readIndex = 0u;

public:

  PingPongBuffer(
    glm::ivec2 size,
    GLenum     internalFormat = GL_RGBA16F
  ) noexcept
      : buffers{
          RenderTarget2D(size, internalFormat),
          RenderTarget2D(size, internalFormat)
        } {
  }

  const RenderTarget2D& GetWriteBuffer() const {
    return buffers[1 - readIndex];
  }

  const RenderTarget2D& GetReadBuffer() const {
    return buffers[readIndex];
  }

  void Swap() {
    readIndex = 1 - readIndex;
  }

  void Resize(glm::ivec2 new_size) noexcept {
    for (auto& b : buffers) {
      b.Resize(new_size);
    }
  }
};
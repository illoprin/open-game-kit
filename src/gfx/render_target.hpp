#pragma once

#include "framebuffer.hpp"
#include "resource.hpp"
#include "texture.hpp"
#include <glm/vec2.hpp>

class RenderTarget {
public:

  RenderTarget(glm::ivec2 _size) : size(_size) {
  }

  RenderTarget(const RenderTarget&)             = delete;
  RenderTarget& operator =(const RenderTarget&) = delete;

  RenderTarget(RenderTarget&& other) noexcept             = default;
  RenderTarget& operator =(RenderTarget&& other) noexcept = default;

  void BindForDrawing(GLenum clear = 0) const noexcept {
    fbo.Bind();
    glViewport(0, 0, size.x, size.y);
    if (clear) { glClear(clear); }
  }

  bool CanUpdateSize(glm::ivec2 new_size) noexcept {
    if (new_size.x < 1 || new_size.y < 1 || new_size == size) return false;
    size = new_size;
    return true;
  }

  glm::ivec2 Size() const noexcept {
    return size;
  }

  const Framebuffer& GetFramebuffer() const {
    return fbo;
  }

protected:

  Framebuffer fbo;
  glm::ivec2  size{0};
};

// ----------------------------------------------------------------------
//                       RenderTarget2D
// ----------------------------------------------------------------------

class RenderTarget2D : public RenderTarget {
public:
  RenderTarget2D(glm::ivec2 _size, GLenum _internalFormat = GL_RGBA8);
  void Resize(glm::ivec2 newSize);
  const Texture2D& GetColor() const {
    return color;
  }
private:
  Texture2D color;
  GLenum    internalFormat;
};

// ----------------------------------------------------------------------
//                       GBuffer
// ----------------------------------------------------------------------

class GBuffer : public RenderTarget {
public:
  GBuffer(glm::ivec2 _size);

  void Resize(glm::ivec2 newSize);

  const Texture2D& GetDiffuse() const { return diffuse; }
  const Texture2D& GetNormal() const { return normal; }
  const Texture2D& GetDepth() const { return depth; }

private:
  Texture2D diffuse;
  Texture2D normal;
  Texture2D depth;
};
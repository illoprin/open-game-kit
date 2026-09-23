#include "render_target.hpp"
#include "core/log.hpp"

// ----------------------------------------------------------------------
//                       RenderTarget2D
// ----------------------------------------------------------------------

RenderTarget2D::RenderTarget2D(glm::ivec2 _size, GLenum _internalFormat)
    : RenderTarget(_size), internalFormat(_internalFormat) {
  color.Allocate(size.x, size.y, internalFormat);
  color.SetSamplerState(GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
  fbo.AttachTexture(GL_COLOR_ATTACHMENT0, color.ID());
  fbo.SetDrawBuffers({GL_COLOR_ATTACHMENT0});

  if (!fbo.IsComplete()) {
    log(LogLevel::Error, "fbo id={} incomplete", fbo.ID());
  }
}

void RenderTarget2D::Resize(glm::ivec2 newSize) {
  if (!CanUpdateSize(newSize)) return;
  color.Allocate(size.x, size.y, internalFormat);
  if (!fbo.IsComplete()) {
    log(LogLevel::Error, "fbo id={} incomplete on resize", fbo.ID());
  }
}

// ----------------------------------------------------------------------
//                       GBuffer
// ----------------------------------------------------------------------

GBuffer::GBuffer(glm::ivec2 _size) : RenderTarget(_size) {
  // 1. Diffuse (RGBA16F)
  diffuse.Allocate(size.x, size.y, GL_RGBA16F);
  diffuse.SetSamplerState(GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
  fbo.AttachTexture(GL_COLOR_ATTACHMENT0, diffuse.ID());

  // 2. Normal (RGBA16F)
  normal.Allocate(size.x, size.y, GL_RGBA16F);
  normal.SetSamplerState(GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
  fbo.AttachTexture(GL_COLOR_ATTACHMENT1, normal.ID());

  // 3. Depth (GL_DEPTH_COMPONENT24 as a texture or renderbuffer)
  depth.Allocate(
    size.x,
    size.y,
    GL_DEPTH_COMPONENT24
  );
  depth.SetSamplerState(GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
  fbo.AttachTexture(GL_DEPTH_ATTACHMENT, depth.ID());

  // Set active draw buffers
  fbo.SetDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});

  if (!fbo.IsComplete()) {
    log(LogLevel::Error, "GBuffer fbo id={} incomplete", fbo.ID());
  }
}

void GBuffer::Resize(glm::ivec2 newSize) {
  if (!CanUpdateSize(newSize)) return;

  diffuse.Allocate(size.x, size.y, GL_RGBA16F);
  normal.Allocate(size.x, size.y, GL_RGBA16F);
  depth.Allocate(
    size.x,
    size.y,
    GL_DEPTH_COMPONENT24
  );

  if (!fbo.IsComplete()) {
    log(LogLevel::Error, "GBuffer fbo id={} incomplete on resize", fbo.ID());
  }
}

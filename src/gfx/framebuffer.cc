#include "framebuffer.hpp"
#include "resource.hpp"

Framebuffer::Framebuffer() : Resource(0) {
  glGenFramebuffers(1, &id);
}

Framebuffer::~Framebuffer() {
  if (id) { glDeleteFramebuffers(1, &id); }
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : Resource(std::move(other)) {
}

Framebuffer& Framebuffer::operator =(Framebuffer&& other) noexcept {
  if (this != &other) {
    if (id) { glDeleteFramebuffers(1, &id); }
    Resource::operator =(std::move(other));
  }
  return *this;
}

void Framebuffer::Bind(GLenum target) const {
  glBindFramebuffer(target, id);
}

void Framebuffer::Unbind(GLenum target) {
  glBindFramebuffer(target, 0);
}

void Framebuffer::AttachTexture(
  GLenum attachment,
  GLuint textureId,
  GLint  level
) {
  Bind();
  glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    attachment,
    GL_TEXTURE_2D,
    textureId,
    level
  );
}

void Framebuffer::AttachRenderbuffer(GLenum attachment, GLuint rboId) {
  Bind();
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rboId);
}

void Framebuffer::SetDrawBuffers(const std::vector<GLenum>& buffers) {
  Bind();
  glDrawBuffers(static_cast<GLsizei>(buffers.size()), buffers.data());
}

bool Framebuffer::IsComplete() const {
  Bind();
  bool complete =
    (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

  return complete;
}
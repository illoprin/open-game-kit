#include "buffer.hpp"
#include <utility>

Buffer::Buffer(GLuint target) : target(target) {
  glGenBuffers(1, &id);
}

Buffer::Buffer(GLuint target, size_t bufferSize) : target(target) {
  glGenBuffers(1, &id);
  glBindBuffer(target, id);
  glBufferData(target, bufferSize, nullptr, GL_STATIC_DRAW);
  glBindBuffer(target, 0);
}

void Buffer::Allocate(size_t size, GLuint usage, const void* data) {
  glBindBuffer(target, id);
  glBufferData(target, size, data, usage);
  glBindBuffer(target, 0);
}

void Buffer::Set(size_t offset, size_t size, const void* data) {
  glBindBuffer(target, id);
  glBufferSubData(target, offset, size, data);
  glBindBuffer(target, 0);
}

Buffer::Buffer(Buffer&& other) noexcept
    : Resource(std::move(other)), target(other.target) {
  other.target = GL_ARRAY_BUFFER;
}

Buffer& Buffer::operator =(Buffer&& other) noexcept {
  if (this != &other) {
    Resource::operator =(std::move(other));
    target       = other.target;
    other.target = GL_ARRAY_BUFFER;
  }
  return *this;
}

Buffer::~Buffer() {
  if (id != 0) {
    glDeleteBuffers(1, &id);
    id = 0;
  }
}

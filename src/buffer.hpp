#pragma once

#include "resource.hpp"

class Buffer : public Resource {
public:

  Buffer(GLuint target);

  Buffer(GLuint target, size_t bufferSize);

  void Allocate(size_t size, GLuint usage, const void* data);
  void Set(size_t offset, size_t size,  const void* data);

  GLuint Target() const { return target; };

  Buffer(const Buffer&)             = delete;
  Buffer& operator =(const Buffer&) = delete;

  Buffer(Buffer&&) noexcept;
  Buffer& operator =(Buffer&&) noexcept;

  ~Buffer();

private:

  GLuint target = GL_ARRAY_BUFFER;
};
#pragma once

#include "buffer.hpp"
#include "resource.hpp"

#include <utility>
#include <vector>

struct Attribute {
  GLuint  Location;
  GLint   Comps;
  GLenum  Type;
  bool    Normalized;
  GLsizei Stride;
  size_t  Offset;
  GLuint  Divisor = 0u;  
};

class VertexArray : public Resource {
public:
  VertexArray() {
    glGenVertexArrays(1, &id);
  }

  ~VertexArray() {
    freeResource();
  }

  // Delete copy constructor and copy assignment
  VertexArray(const VertexArray&) = delete;
  VertexArray& operator=(const VertexArray&) = delete;

  // Move constructor and move assignment
  VertexArray(VertexArray&& other) noexcept 
    : Resource(std::move(other)) {
      eboBinding = other.eboBinding;
      other.eboBinding = 0;
    }

  VertexArray& operator=(VertexArray&& other) noexcept {
    if (this != &other) {
      freeResource();
      Resource::operator=(std::move(other));
      eboBinding = other.eboBinding;
      other.eboBinding = 0;
    }
    return *this;
  }

  void AttachIndexBuffer(const Buffer&) noexcept;

  void SetAttribute(const Buffer&, const std::vector<Attribute>&) noexcept;

  void Bind() const {
    glBindVertexArray(id);
  }

  static void Unbind() {
    glBindVertexArray(0);
  }

private:
  GLuint eboBinding = 0;
  void freeResource() {
    if (id != 0) {
      glDeleteVertexArrays(1, &id);
      eboBinding = 0;
      id = 0;
    }
  }
};
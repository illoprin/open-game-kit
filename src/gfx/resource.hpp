#pragma once

#include <glad/glad.h>

class Resource {
public:

  // Constructor & Destructor
  Resource() : id(0) {
  }

  explicit Resource(GLuint id) : id(id) {
  }

  // Delete copy constructor and copy assignment
  Resource(const Resource&)             = delete;
  Resource& operator =(const Resource&) = delete;

  // Implement move constructor and move assignment
  Resource(Resource&& other) noexcept : id(other.id) {
    other.id = 0;
  }

  Resource& operator =(Resource&& other) noexcept {
    if (this != &other) {
      id       = other.id;
      other.id = 0;
    }
    return *this;
  }

  GLuint ID() const {
    return id;
  }

protected:

  GLuint id;
};

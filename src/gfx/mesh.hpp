#pragma once

#include "buffer.hpp"
#include "scene/geometry.hpp"
#include "vertex_array.hpp"
#include <span>

class Mesh {
public:
  Mesh() = default;
  
  // create mesh from geometry (initializes VBO EBO and VAO)
  void FromGeometry(const Geometry& geometry);

  void FromFlat(std::span<const glm::vec2> v, std::span<const uint> i);

  // Удаляем конструкторы и операторы копирования
  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  // Конструкторы и операторы перемещения
  Mesh(Mesh&&) noexcept = default;
  Mesh& operator=(Mesh&&) noexcept = default;

  ~Mesh() = default;

  const VertexArray& GetVAO() const { return vao; }
  const Buffer& GetVBO() const { return vbo; }
  const Buffer& GetEBO() const { return ebo; }
  size_t GetIndexCount() const { return indexCount; }

  void Draw(uint instances = 1) const noexcept;

private:
  VertexArray vao;
  Buffer vbo{GL_ARRAY_BUFFER};
  Buffer ebo{GL_ELEMENT_ARRAY_BUFFER};
  size_t indexCount = 0;
};
#include "mesh.hpp"
#include "gfx/gl.hpp"
#include "gfx/resource.hpp"
#include "gfx/vertex_array.hpp"
#include "glm/ext/vector_float2.hpp"

void Mesh::FromGeometry(const Geometry& geometry) {
  indexCount = geometry.Indices.size();

  VertexArray::Unbind();

  // Выделяем и заполняем VBO
  vbo.Allocate(
    geometry.Vertices.size() * sizeof(ModelVertex),
    GL_STATIC_DRAW,
    geometry.Vertices.data()
  );

  // Выделяем и заполняем EBO
  ebo.Allocate(
    geometry.Indices.size() * sizeof(uint),
    GL_STATIC_DRAW,
    geometry.Indices.data()
  );

  // Настраиваем атрибуты вершин для VAO
  std::vector<Attribute> attributes = {
    {0,
     3, GL_FLOAT,
     false, sizeof(ModelVertex),
     offsetof(ModelVertex, Position),
     0},
    {1,
     3, GL_FLOAT,
     false, sizeof(ModelVertex),
     offsetof(ModelVertex, Normal),
     0},
    {2,
     2, GL_FLOAT,
     false, sizeof(ModelVertex),
     offsetof(ModelVertex, Texcoord),
     0}
  };

  vao.SetAttribute(vbo, attributes);
  vao.AttachIndexBuffer(ebo);
}

void Mesh::FromFlat(std::span<const glm::vec2> v, std::span<const uint> i) {
  indexCount = i.size();

  VertexArray::Unbind();

  vbo.Allocate(v.size() * sizeof(glm::vec2), GL_STATIC_DRAW, v.data());
  ebo.Allocate(i.size() * sizeof(uint), GL_STATIC_DRAW, i.data());

  Attribute attr{
    0, 
    2,
    GL_FLOAT,
    false,
    sizeof(glm::vec2),
    0, 
    0
  };
  vao.SetAttribute(vbo, {attr});
  vao.AttachIndexBuffer(ebo);
}

void Mesh::Draw(uint instances) const noexcept {
  if (indexCount < 3 || !instances) return;

  GL::DrawElements(vao, indexCount, GL_TRIANGLES, instances);
}
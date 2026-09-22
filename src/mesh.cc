#include "mesh.hpp"

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
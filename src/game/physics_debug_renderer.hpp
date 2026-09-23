#pragma once

#include "core./utils.hpp"
#include "gfx/buffer.hpp"
#include "gfx/program.hpp"
#include "gfx/vertex_array.hpp"
#include "scene/camera.hpp"
#include "scene/collision.hpp"
#include <span>
#include <vector>

class PhysicsDebugRenderer {
public:

  PhysicsDebugRenderer();

  void
    DrawStaticColliders(std::span<const StaticCollider> colliders, const Camera3D& cam) noexcept;

private:

  Program                debugProgram;
  VertexArray            vao;
  Buffer                 vbo;
  Buffer                 ebo;
  std::vector<glm::vec3> vertices;
  std::vector<uint>      indices;
};
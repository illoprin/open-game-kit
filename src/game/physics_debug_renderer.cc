#include "physics_debug_renderer.hpp"
#include "gfx/gl.hpp"
#include "gfx/resource.hpp"
#include "core/files.hpp"
#include "gfx/vertex_array.hpp"

constexpr uint MAX_COLLIDERS = 256;

PhysicsDebugRenderer::PhysicsDebugRenderer()
    : vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER) {
  
  // Load debug shader program
  Program::FastLoad(debugProgram, ShaderPath("debug_line.vert"), ShaderPath("debug_line.frag"));

  VertexArray::Unbind();

  // Initial reserve memory for MAX_COLLIDERS
  vbo.Allocate(96 * MAX_COLLIDERS, GL_STREAM_DRAW, nullptr);
  ebo.Allocate(96 * MAX_COLLIDERS, GL_STREAM_DRAW, nullptr);
  
  vertices.reserve(24 * MAX_COLLIDERS);
  indices.reserve(24 * MAX_COLLIDERS);

  std::vector<Attribute> attrs = {
    {0,
     3, GL_FLOAT,
     GL_FALSE,
     sizeof(glm::vec3),
     0,
     0}
  };
  vao.SetAttribute(vbo, attrs);
  vao.AttachIndexBuffer(ebo);
}


void PhysicsDebugRenderer::DrawStaticColliders(
  std::span<const StaticCollider> colliders,
  const Camera3D& cam
) noexcept {
  
  vertices.clear();
  indices.clear();

  uint baseIndex = 0, count = 0;
  for (const auto& collider : colliders) {
    if (count >= MAX_COLLIDERS) break;

    const auto& box = collider.BoxBounds;
    glm::vec3   min = box.min;
    glm::vec3   max = box.max;

    // 8 corners of AABB
    vertices.push_back({min.x, min.y, min.z});
    vertices.push_back( {max.x, min.y, min.z});
    vertices.push_back( {max.x, max.y, min.z});
    vertices.push_back( {min.x, max.y, min.z});
    vertices.push_back( {min.x, min.y, max.z});
    vertices.push_back( {max.x, min.y, max.z});
    vertices.push_back( {max.x, max.y, max.z});
    vertices.push_back( {min.x, max.y, max.z});

    // Wireframe indices for a box (12 lines = 24 indices)
    uint idx[] = {
      0, 1, 1, 2, 2, 3, 3, 0,  // Bottom
      4, 5, 5, 6, 6, 7, 7, 4,  // Top
      0, 4, 1, 5, 2, 6, 3, 7  // Connectors
    };

    for (uint i : idx) {
      indices.push_back(baseIndex + i);
    }
    baseIndex += 8;
    ++count;
  }

  if (vertices.empty()) return;

  vbo.Set(
    0,
    vertices.size() * sizeof(glm::vec3),
    vertices.data()
  );
  ebo.Set(0, indices.size() * sizeof(uint), indices.data());

  debugProgram.Use();
  debugProgram.SetMat4("u_view", cam.GetView());
  debugProgram.SetMat4("u_projection", cam.GetProjection());
  debugProgram.SetMat4("u_model", glm::mat4(1.0f));
  debugProgram.SetVec4("u_color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

  GL::DrawElements(
    vao,
    static_cast<uint>(indices.size()),
    GL_UNSIGNED_INT,
    1,
    GL_LINES
  );
}

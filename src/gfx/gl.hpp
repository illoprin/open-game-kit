#pragma once

#include "resource.hpp"
#include <core/utils.hpp>
#include "vertex_array.hpp"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

struct RenderStats {
  uint Triangles = 0;
  uint DrawCalls = 0;
};

class GL {
public:

  static bool PopError() noexcept;

  static void BlitFramebuffer(
    GLuint readFbo, 
    GLuint drawFbo, 
    GLenum readBuffer, 
    GLenum drawBuffer, 
    glm::ivec2 readSize, 
    glm::ivec2 drawSize, 
    GLbitfield mask = GL_COLOR_BUFFER_BIT, 
    GLenum filter = GL_NEAREST
  ) noexcept;

  static bool CreateContext() noexcept {
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  };

  static void DrawArrays(const VertexArray&, uint count, uint instances = 1) noexcept;
  static void DrawElements(
    const VertexArray&,
    uint   count,
    GLuint data_type,
    uint   instances = 1
  ) noexcept;

  static const RenderStats& GetStats();
  static void ResetStats();

private:
  static RenderStats stats;
};
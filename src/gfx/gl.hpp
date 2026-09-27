#pragma once

#include "resource.hpp"
#include "vertex_array.hpp"
#include <GLFW/glfw3.h>
#include <core/utils.hpp>
#include <glm/vec2.hpp>

class GL {

public:

  static bool PopError() noexcept;

  static void BlitFramebuffer(
    GLuint     readFbo,
    GLuint     drawFbo,
    GLenum     readBuffer,
    GLenum     drawBuffer,
    glm::ivec2 readSize,
    glm::ivec2 drawSize,
    GLbitfield mask   = GL_COLOR_BUFFER_BIT,
    GLenum     filter = GL_NEAREST
  ) noexcept;

  static bool CreateContext() noexcept {
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  }

  static void
    DrawArrays(const VertexArray&, uint count, uint instances = 1) noexcept;
  static void DrawElements(
    const VertexArray&,
    uint   count,
    GLuint data_type,
    uint   instances = 1,
    GLenum mode      = GL_TRIANGLES
  ) noexcept;

  struct RenderStats {
    uint Triangles = 0;
    uint DrawCalls = 0;
  };

  static const RenderStats& GetStats() { return stats; };
  static void               ResetStats();

private:

  static RenderStats stats;
};
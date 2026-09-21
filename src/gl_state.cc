#include "gl_state.hpp"
#include "log.hpp"
#include "resource.hpp"
#include "vertex_array.hpp"
#include <cstdio>
#include <cstring>

RenderStats GL::stats;

void GL::DrawArrays(
  const VertexArray& vao,
  uint               count,
  uint               instances
) noexcept {
  vao.Bind();
  if (instances > 1)
    glDrawArraysInstanced(GL_TRIANGLES, 0, count, instances);
  else
    glDrawArrays(GL_TRIANGLES, 0, count);
  VertexArray::Unbind();
}

void GL::DrawElements(
  const VertexArray& vao,
  uint               count,
  GLuint             data_type,
  uint               instances
) noexcept {
  vao.Bind();
  if (instances > 1)
    glDrawElementsInstanced(GL_TRIANGLES, count, data_type, nullptr, instances);
  else
    glDrawElements(GL_TRIANGLES, count, data_type, nullptr);
  VertexArray::Unbind();
}

const RenderStats& GL::GetStats() {
  return stats;
}

void GL::ResetStats() {
  stats.Triangles = 0;
  stats.DrawCalls = 0;
}

bool GL::PopError() noexcept {
  GLenum error = glGetError();
  if (error == GL_NO_ERROR) return false;

  char buf[128];

  switch (error) {
    case GL_INVALID_ENUM:
      strcpy(
        buf,
        "[GL_INVALID_ENUM] "
        "\n\tAn unacceptable value is specified for an enumerated argument"
      );
      break;
    case GL_INVALID_VALUE:
      strcpy(
        buf,
        "[GL_INVALID_VALUE] "
        "\n\tA numeric argument is out of range or invalid"
      );
      break;
    case GL_INVALID_OPERATION:
      strcpy(
        buf,
        "[GL_INVALID_OPERATION] "
        "\n\tThe specified operation is not allowed in the current OpenGL "
        "state"
      );
      break;
    case GL_OUT_OF_MEMORY:
      strcpy(
        buf,
        "[GL_OUT_OF_MEMORY] "
        "\n\tMemory allocation operation cannot allocate memory"
      );
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      strcpy(
        buf,
        "[GL_INVALID_FRAMEBUFFER_OPERATION] "
        "\n\tReading or writing to a framebuffer that is not complete"
      );
      break;
  }

  std::printf("[GL ERROR] [0x%04x] %s\n", error, buf);
  return true;
}

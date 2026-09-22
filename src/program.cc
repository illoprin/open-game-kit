#include "program.hpp"

#include "files.hpp"
#include "log.hpp"

Program::~Program() {
  if (id != 0) { glDeleteProgram(id); }
}


void Program::Use() const {
  glUseProgram(id);
}

void Program::SetBool(std::string_view name, bool value) const {
  glUniform1i(GetUniformLocation(name), static_cast<int>(value));
}

void Program::SetInt(std::string_view name, int value) const {
  glUniform1i(GetUniformLocation(name), value);
}

void Program::SetFloat(std::string_view name, float value) const {
  glUniform1f(GetUniformLocation(name), value);
}

void
  Program::SetVec2(std::string_view name, const glm::vec2& value) const {
  glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void
  Program::SetVec3(std::string_view name, const glm::vec3& value) const {
  glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void
  Program::SetVec4(std::string_view name, const glm::vec4& value) const {
  glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void
  Program::SetMat4(std::string_view name, const glm::mat4& mat) const {
  glUniformMatrix4fv(
    GetUniformLocation(name),
    1,
    GL_FALSE,
    glm::value_ptr(mat)
  );
}

void Program::SetUniformBlockBinding(
  std::string_view blockName,
  GLuint           bindingPoint
) const {
  Use();
  GLuint blockIndex = glGetUniformBlockIndex(id, blockName.data());
  if (blockIndex != GL_INVALID_INDEX) {
    glUniformBlockBinding(id, blockIndex, bindingPoint);
  }
}

GLint Program::GetUniformLocation(std::string_view name) const {
  std::string nameStr(name);
  auto        it = uniformLocationCache.find(nameStr);
  if (it != uniformLocationCache.end()) { return it->second; }

  GLint location                = glGetUniformLocation(id, nameStr.c_str());
  uniformLocationCache[nameStr] = location;
  return location;
}


bool Program::CompileAndLink(
  std::string_view vertexSource,
  std::string_view fragmentSource
) {
  GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
  if (vs == 0) return false;

  GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
  if (fs == 0) {
    glDeleteShader(vs);
    return false;
  }

  GLuint prog = glCreateProgram();
  glAttachShader(prog, vs);
  glAttachShader(prog, fs);
  glLinkProgram(prog);

  GLint success = 0;
  glGetProgramiv(prog, GL_LINK_STATUS, &success);
  if (!success) {
    GLint logLength = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    std::string msg(logLength, '\0');
    glGetProgramInfoLog(prog, logLength, nullptr, msg.data());
    log(LogLevel::Error, "Program Linking Error:\n{}", msg);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteProgram(prog);
    return false;
  }

  glDetachShader(prog, vs);
  glDetachShader(prog, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);

  if (id != 0) { glDeleteProgram(id); }

  id = prog;
  uniformLocationCache.clear();
  return true;
}

GLuint Program::CompileShader(
  GLenum           type,
  std::string_view source
) {
  GLuint      shader = glCreateShader(type);
  const char* srcPtr = source.data();
  GLint       srcLen = static_cast<GLint>(source.size());
  glShaderSource(shader, 1, &srcPtr, &srcLen);
  glCompileShader(shader);

  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
      GLint logLength = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
      std::string msg(logLength, '\0');
      glGetShaderInfoLog(shader, logLength, nullptr, msg.data());
      std::string typeStr = (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
      log(LogLevel::Error, "{} Shader Compilation Error:\n{}", typeStr, msg);
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}


bool Program::FastLoad(
  Program&         prog,
  std::string_view vertexPath,
  std::string_view fragmentPath
) noexcept {
  std::string vert = ReadFile(vertexPath).value_or("");
  std::string frag = ReadFile(fragmentPath).value_or("");
  if (vert.size() < 3 || frag.size() < 3) return false;
  return prog.CompileAndLink(vert, frag);
}

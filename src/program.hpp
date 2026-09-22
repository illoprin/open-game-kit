#pragma once

#include "resource.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

class Program : public Resource {
public:

  Program() = default;

  explicit Program(GLuint id) : Resource(id) {
  }

  ~Program();

  // Disable copying, allow moving
  Program(const Program&)                 = delete;
  Program& operator =(const Program&)     = delete;
  Program(Program&&) noexcept             = default;
  Program& operator =(Program&&) noexcept = default;

  bool CompileAndLink(
    std::string_view vertexSource,
    std::string_view fragmentSource
  );

  static bool FastLoad(Program& prog, std::string_view vertexPath,std::string_view fragmentPath) noexcept;

  void Use() const;

  // Uniform Setters (PascalCase)
  void SetBool(std::string_view name, bool value) const;
  void SetInt(std::string_view name, int value) const;
  void SetFloat(std::string_view name, float value) const;
  void SetVec2(std::string_view name, const glm::vec2& value) const;
  void SetVec3(std::string_view name, const glm::vec3& value) const;
  void SetVec4(std::string_view name, const glm::vec4& value) const;
  void SetMat4(std::string_view name, const glm::mat4& mat) const;

  // Uniform Block Binding
  void SetUniformBlockBinding(
    std::string_view blockName,
    GLuint           bindingPoint
  ) const;

private:

  mutable std::unordered_map<std::string, GLint> uniformLocationCache;

  GLint GetUniformLocation(std::string_view name) const;
  GLuint
    CompileShader(GLenum type, std::string_view source);
};

#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "core/utils.hpp"
#include <expected>
#include <string>
#include <utility>
#include <vector>

struct ModelVertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 Texcoord;
};

struct Geometry {
  std::vector<ModelVertex> Vertices;
  std::vector<uint>        Indices;

  Geometry(std::vector<ModelVertex>&& v, std::vector<uint>&& i)
      : Vertices(std::move(v)), Indices(std::move(i)) {
  }

  static std::expected<Geometry, std::string> FromObj(const std::string& filepath);
  static Geometry CreateCube(glm::vec3 size);
  static Geometry CreatePlane(glm::vec2 size);

  Geometry(const Geometry&)             = delete;
  Geometry& operator =(const Geometry&) = delete;

  Geometry(Geometry&&) noexcept             = default;
  Geometry& operator =(Geometry&&) noexcept = default;
};

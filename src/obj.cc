#include "model.hpp"
#include <fstream>

std::expected<Geometry, std::string>
  Geometry::FromObj(const std::string& filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    return std::unexpected("Failed to open file: " + filepath);
  }

  std::vector<glm::vec3> tempPositions;
  std::vector<glm::vec2> tempTexcoords;
  std::vector<glm::vec3> tempNormals;

  struct VertexKey {
    int posIdx  = 0;
    int uvIdx   = 0;
    int normIdx = 0;

    auto operator<=>(const VertexKey&) const = default;
  };

  struct VertexKeyHash {
    std::size_t operator ()(const VertexKey& k) const {
      return ((std::hash<int>()(k.posIdx) ^ (std::hash<int>()(k.uvIdx) << 1))
              >> 1)
             ^ (std::hash<int>()(k.normIdx) << 1);
    }
  };

  std::vector<ModelVertex>                           vertices;
  std::vector<uint>                                  indices;
  std::unordered_map<VertexKey, uint, VertexKeyHash> uniqueVertices;

  // Быстрый парсер чисел с помощью std::from_chars (без аллокаций)
  auto fastAtoi = [](std::string_view sv, int& out) -> bool {
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), out);
    return ec == std::errc{};
  };

  auto fastAtof = [](std::string_view sv, float& out) -> bool {
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), out);
    return ec == std::errc{};
  };

  // Парсинг токена вершины вида v/vt/vn без stringstream
  auto parseIndexToken =
    [&](std::string_view token, int& p, int& vt, int& vn) -> bool {
    p = vt = vn       = 0;
    size_t firstSlash = token.find('/');
    if (firstSlash == std::string_view::npos) { return fastAtoi(token, p); }

    if (!fastAtoi(token.substr(0, firstSlash), p)) return false;

    size_t secondSlash = token.find('/', firstSlash + 1);
    if (secondSlash == std::string_view::npos) {
      std::string_view vtStr = token.substr(firstSlash + 1);
      if (!vtStr.empty()) { return fastAtoi(vtStr, vt); }
      return true;
    }

    std::string_view vtStr =
      token.substr(firstSlash + 1, secondSlash - (firstSlash + 1));
    if (!vtStr.empty()) {
      if (!fastAtoi(vtStr, vt)) return false;
    }

    std::string_view vnStr = token.substr(secondSlash + 1);
    if (!vnStr.empty()) {
      if (!fastAtoi(vnStr, vn)) return false;
    }

    return true;
  };

  std::string line;
  int         lineNum = 0;
  while (std::getline(file, line)) {
    lineNum++;
    std::string_view sv(line);

    // Пропуск пробелов в начале
    size_t start = sv.find_first_not_of(" \t\r\n");
    if (start == std::string_view::npos || sv[start] == '#') continue;
    sv.remove_prefix(start);

    // Разделение на префикс и аргументы
    size_t           spacePos = sv.find_first_of(" \t");
    std::string_view prefix   = sv.substr(0, spacePos);

    if (prefix == "v" || prefix == "vt" || prefix == "vn") {
      if (spacePos == std::string_view::npos) continue;
      sv.remove_prefix(spacePos);

      float  vals[3] = {0.0f, 0.0f, 0.0f};
      int    count   = 0;
      size_t pos     = 0;
      while (pos < sv.size() && count < 3) {
        while (pos < sv.size() && (sv[pos] == ' ' || sv[pos] == '\t'))
          pos++;
        if (pos >= sv.size()) break;
        size_t endPos = pos;
        while (endPos < sv.size() && sv[endPos] != ' ' && sv[endPos] != '\t'
               && sv[endPos] != '\r')
          endPos++;

        if (!fastAtof(sv.substr(pos, endPos - pos), vals[count++])) {
          return std::unexpected(
            "Line " + std::to_string(lineNum) + ": Invalid float format."
          );
        }
        pos = endPos;
      }

      if (prefix == "v") {
        tempPositions.emplace_back(vals[0], vals[1], vals[2]);
      } else if (prefix == "vt") {
        tempTexcoords.emplace_back(vals[0], vals[1]);
      } else if (prefix == "vn") {
        tempNormals.emplace_back(vals[0], vals[1], vals[2]);
      }
    } else if (prefix == "f") {
      if (spacePos == std::string_view::npos) continue;
      sv.remove_prefix(spacePos);

      std::vector<std::string_view> faceTokens;
      size_t                        pos = 0;
      while (pos < sv.size()) {
        while (pos < sv.size() && (sv[pos] == ' ' || sv[pos] == '\t'))
          pos++;
        if (pos >= sv.size()) break;
        size_t endPos = pos;
        while (endPos < sv.size() && sv[endPos] != ' ' && sv[endPos] != '\t'
               && sv[endPos] != '\r')
          endPos++;
        faceTokens.push_back(sv.substr(pos, endPos - pos));
        pos = endPos;
      }

      if (faceTokens.size() != 3 && faceTokens.size() != 4) {
        return std::unexpected(
          "Line " + std::to_string(lineNum)
          + ": Only triangles and quads are supported (found "
          + std::to_string(faceTokens.size()) + " vertices)."
        );
      }

      auto processVertex =
        [&](std::string_view token) -> std::expected<uint, std::string> {
        int p = 0, vt = 0, vn = 0;
        if (!parseIndexToken(token, p, vt, vn)) {
          return std::unexpected(
            "Line " + std::to_string(lineNum) + ": Invalid face index format."
          );
        }

        if (p < 0) p = static_cast<int>(tempPositions.size()) + p + 1;
        if (vt < 0) vt = static_cast<int>(tempTexcoords.size()) + vt + 1;
        if (vn < 0) vn = static_cast<int>(tempNormals.size()) + vn + 1;

        VertexKey key{p, vt, vn};
        if (auto it = uniqueVertices.find(key); it != uniqueVertices.end()) {
          return it->second;
        }

        ModelVertex mv{};
        if (p > 0 && p <= static_cast<int>(tempPositions.size())) {
          mv.Position = tempPositions[p - 1];
        }
        if (vt > 0 && vt <= static_cast<int>(tempTexcoords.size())) {
          mv.Texcoord = tempTexcoords[vt - 1];
        }
        if (vn > 0 && vn <= static_cast<int>(tempNormals.size())) {
          mv.Normal = tempNormals[vn - 1];
        }

        uint newIndex = static_cast<uint>(vertices.size());
        vertices.push_back(mv);
        uniqueVertices[key] = newIndex;
        return newIndex;
      };

      uint faceIndices[4];
      for (size_t i = 0; i < faceTokens.size(); ++i) {
        auto idxRes = processVertex(faceTokens[i]);
        if (!idxRes) return std::unexpected(idxRes.error());
        faceIndices[i] = *idxRes;
      }

      indices.push_back(faceIndices[0]);
      indices.push_back(faceIndices[1]);
      indices.push_back(faceIndices[2]);

      if (faceTokens.size() == 4) {
        indices.push_back(faceIndices[0]);
        indices.push_back(faceIndices[2]);
        indices.push_back(faceIndices[3]);
      }
    }
  }

  return Geometry{std::move(vertices), std::move(indices)};
}
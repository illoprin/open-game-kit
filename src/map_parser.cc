#include "map_parser.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Предполагается, что заголовки glm и структуры уже подключены

bool MapData::LoadGameMap(const std::string& filepath) noexcept {
  uint line_num = 0;
  try {
    std::ifstream file(filepath);
    if (!file.is_open()) {
      std::cerr << "[MapLoader] Failed to open file: " << filepath << "\n";
      return false;
    }

    // Очистка старых данных
    Name.clear();
    Textures.clear();
    Geometries.clear();
    Materials.clear();
    Instances.clear();
    Lights.clear();

    std::string line;

    // Вспомогательная функция для парсинга vec3 из строки "(x, y, z)"
    auto parseVec3 = [](const std::string& s) -> glm::vec3 {
      glm::vec3   result(0.0f);
      std::string clean = s;

      // Заменяем скобки и запятые на пробелы, чтобы stringstream всё легко
      // прочитал
      for (char& c : clean) {
        if (c == '(' || c == ')' || c == ',') { c = ' '; }
      }

      std::stringstream ss(clean);
      ss >> result.x >> result.y >> result.z;

      if (ss.fail()) { throw std::runtime_error("Invalid vec3 format: " + s); }
      return result;
    };

    while (std::getline(file, line)) {
      // Убираем ведущие пробелы
      size_t start = line.find_first_not_of(" \t\r\n");
      if (start == std::string::npos) continue;
      line = line.substr(start);

      // Пропускаем комментарии и пустые строки
      if (line.empty() || line[0] == '#') continue;

      std::stringstream ss(line);
      std::string       token;
      ss >> token;

      line_num++;
      if (token == "NAME") {
        // NAME может содержать пробелы, берем остаток строки
        std::string nameRaw;
        std::getline(ss, nameRaw);
        // Убираем кавычки и лишние пробелы
        nameRaw.erase(
          std::remove(nameRaw.begin(), nameRaw.end(), '"'),
          nameRaw.end()
        );

        size_t first = nameRaw.find_first_not_of(" \t");
        size_t last  = nameRaw.find_last_not_of(" \t");
        if (first != std::string::npos)
          Name = nameRaw.substr(first, last - first + 1);
      } else if (token == "TEXTURE") {
        std::string id, path;
        if (ss >> id >> path) Textures[id] = path;
      } else if (token == "GEOMETRY") {
        std::string id, path;
        if (ss >> id >> path) Geometries[id] = path;
      } else if (token == "MATERIAL") {
        std::string id, diffuse, emissive, tintStr;
        if (ss >> id >> diffuse >> emissive >> tintStr) {
          MapMaterial mat;
          mat.ID         = id;
          mat.DiffuseID  = (diffuse == "NULL") ? "" : diffuse;
          mat.EmissiveID = (emissive == "NULL") ? "" : emissive;
          mat.Tint       = parseVec3(tintStr);
          Materials[id]  = std::move(mat);
        }
      } else if (token == "INSTANCE") {
        std::string proto, matId, uvStr, posStr, sclStr, rotStr;
        if (ss >> proto >> matId >> uvStr >> posStr >> sclStr >> rotStr) {
          MapInstance inst;
          inst.GeometryID = proto;
          inst.MaterialID = matId;
          inst.UVScaling  = std::stof(uvStr);
          inst.position   = parseVec3(posStr);
          inst.scale      = parseVec3(sclStr);
          inst.rotation   = parseVec3(rotStr);
          Instances.push_back(std::move(inst));
        }
      } else if (token == "LIGHT") {
        std::string typeStr, posStr, intensityStr, radiusStr, colorStr, dirStr;

        // Читаем ВСЕ поля включая direction (он может быть NULL или vec3)
        if (ss >> typeStr >> posStr >> intensityStr >> radiusStr >> colorStr
            >> dirStr) {
          LightEntry light{};
          light.Type      = (typeStr == "directional") ? LightType::Directional
                                                       : LightType::Point;
          light.Position  = parseVec3(posStr);
          light.Intensity = std::stof(intensityStr);
          light.Radius    = std::stof(radiusStr);
          light.Color     = parseVec3(colorStr);

          if (dirStr != "NULL") {
            light.Direction = parseVec3(dirStr);
          } else {
            light.Direction = glm::vec3(0.0f);
          }

          Lights.push_back(light);
        } else {
          log(LogLevel::Error, "MapData: Malformed LIGHT entry: {}", line);
        }
      }
    }

  } catch (const std::exception& e) {
    log(
      LogLevel::Error,
      "MapData: exception during parsing (line = {}) : {}",
      line_num,
      e.what()
    );
    ;
    return false;
  } catch (...) {
    log(LogLevel::Error, "MapData: unknown exception");
    return false;
  }

  log(
    LogLevel::Success,
    "MapData: `{}` data loaded\n{{\n  Textures {}\n  Geometry {}\n  Instances "
    "{}\n  Lights {}\n}}",
    Name,
    Textures.size(),
    Geometries.size(),
    Instances.size(),
    Lights.size()
  );

  return true;
}

#include <expected>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

inline std::expected<std::string, std::error_code>
  ReadFile(const fs::path& filePath) {
  // Open in binary mode and start at the end to get the file size
  std::ifstream file(filePath, std::ios::binary | std::ios::ate);
  if (!file) {
    return std::unexpected(
      std::make_error_code(std::errc::no_such_file_or_directory)
    );
  }

  const auto size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::string content;
  content.resize(size);

  if (file.read(content.data(), size)) { return content; }

  return std::unexpected(std::make_error_code(std::errc::io_error));
}

constexpr std::string_view ASSETS_FOLDER   = "assets";
constexpr std::string_view TEXTURES_FOLDER = "textures";
constexpr std::string_view MODELS_FOLDER   = "models";
constexpr std::string_view SHADERS_FOLDER  = "shaders";
constexpr std::string_view FONTS_FOLDER    = "fonts";
constexpr std::string_view LEVELS_FOLDER    = "levels";

inline std::string TexturePath(const std::string& file) {
  return fs::path{fs::path{ASSETS_FOLDER} / TEXTURES_FOLDER / file}.string();
}

inline std::string FontPath(const std::string& file) {
  return fs::path{fs::path{ASSETS_FOLDER} / FONTS_FOLDER / file}.string();
}

inline std::string LevelPath(const std::string& file) {
  return fs::path{fs::path{ASSETS_FOLDER} / LEVELS_FOLDER / file}.string();
}

inline std::string ShaderPath(const std::string& file) {
  return fs::path{fs::path{ASSETS_FOLDER} / SHADERS_FOLDER / file}.string();
}

inline std::string ModelPath(const std::string& file) {
  return fs::path{fs::path{ASSETS_FOLDER} / MODELS_FOLDER / file}.string();
}
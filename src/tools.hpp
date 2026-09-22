#pragma once

#include <string_view>
class ScreenshotTool {
public:
  static bool Needs;
  static void Update(const std::string_view& path) noexcept;
};
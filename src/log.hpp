#pragma once

#include <chrono>
#include <format>
#include <print>
#include <string_view>

enum class LogLevel {
  Info,
  Success,
  Warning,
  Error
};

template<typename... Args>
inline void log(
  LogLevel                    level,
  std::format_string<Args...> fmt,
  Args&&... args
) noexcept {
  // 1. Get current time string
  auto now =
    std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
  std::string time_str = std::format("{:%H:%M:%S}", now);

  // 2. Determine color and tag based on level
  std::string_view color;
  std::string_view tag;

  switch (level) {
    case LogLevel::Info:
      color = "\033[36m";  // Cyan
      tag   = "INFO";
      break;
    case LogLevel::Success:
      color = "\033[32m";  // Green
      tag   = "SUCCESS";
      break;
    case LogLevel::Warning:
      color = "\033[33m";  // Yellow
      tag   = "WARN";
      break;
    case LogLevel::Error:
      color = "\033[31m";  // Red
      tag   = "ERROR";
      break;
  }

  // 3. Format the user message
  std::string message = std::format(fmt, std::forward<Args>(args)...);

  // 4. Print with ANSI colors and reset code (\033[0m)
  std::println(
    "\033[90m[{}]\033[0m {}[{}]\033[0m {}",
    time_str,
    color,
    tag,
    message
  );
}

template<typename... Args>
inline void og_assert(
  bool                        condition,
  std::format_string<Args...> fmt,
  Args&&... args
) noexcept {
  if (!condition) {
    log(LogLevel::Error, fmt, std::forward<Args>(args)...);
    std::exit(EXIT_FAILURE);
  }
}
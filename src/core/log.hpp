#pragma once

#include <chrono>
#include <format>
#include <print>
#include <string_view>

// Reset
#define RESET   "\033[0m"

// Regular Text Colors
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

// Bold / Bright Text Colors
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"

// High Intensity Text Colors
#define HI_BLACK   "\033[90m"
#define HI_RED     "\033[91m"
#define HI_GREEN   "\033[92m"
#define HI_YELLOW  "\033[93m"
#define HI_BLUE    "\033[94m"
#define HI_MAGENTA "\033[95m"
#define HI_CYAN    "\033[96m"
#define HI_WHITE   "\033[97m"

// Background Colors
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"


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
  std::string time_str = std::format("{:%T}", now);

  // 2. Determine color and tag based on level
  std::string_view color;
  std::string_view tag;

  switch (level) {
    case LogLevel::Info:
      color = BG_CYAN;  // Cyan
      tag   = "INFO";
      break;
    case LogLevel::Success:
      color = BG_GREEN;  // Green
      tag   = "SUCCESS";
      break;
    case LogLevel::Warning:
      color = BG_YELLOW;  // Yellow
      tag   = "WARN";
      break;
    case LogLevel::Error:
      color = BG_RED;  // Red
      tag   = "ERROR";
      break;
  }

  // 3. Format the user message
  std::string message = std::format(fmt, std::forward<Args>(args)...);

  // 4. Print with ANSI colors and reset code (\033[0m)
  std::println(
    "\033[90m[{}] " RESET BLACK "{}[{}]" RESET CYAN " {}" RESET,
    time_str,
    color,
    tag,
    message
  );
}

#define LOG_INFO(STR, ...) \
  log(LogLevel::Info, STR, __VA_ARGS__)

#define LOG_ERR(STR, ...) \
  log(LogLevel::Error, STR, __VA_ARGS__)

#define LOG_WAR(STR, ...) \
  log(LogLevel::Warning, STR, __VA_ARGS__)

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
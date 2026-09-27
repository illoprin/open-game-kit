
#pragma once

#include "utils.hpp"
#include "window.hpp"
#include <glm/vec2.hpp>
#include <memory>

struct Config {
  glm::ivec2       WinSize;
  float            Ratio               = 0.75;
  uint             ScreenshotKey       = GLFW_KEY_F2;
  std::string_view ScreenshotsPath     = {"screenshots"};
  bool             Debug               = true;
  float            UIPadding           = 10.0;
  uint             DebugStatsSwitchKey = GLFW_KEY_F1;
};

class IEngineState {

public:

  IEngineState();

  virtual void Update() noexcept {
  }

  virtual void FixedUpdate60() noexcept {
  }

  virtual void FixedUpdate30() noexcept {
  }

  virtual void Render() noexcept {
  }

  virtual void OnExit() noexcept {
  }

  virtual void OnResize() noexcept {
  }

  virtual void OnEnter() noexcept {
  }

  virtual ~IEngineState() = default;
};

class Engine {
public:

  [[nodiscard]] static bool Create(const Config&);

  [[nodiscard]] static bool Created() {
    return created;
  }

  [[nodiscard]] static const Config& GetConfig() {
    return cfg;
  }

  static void SetState(std::unique_ptr<IEngineState>& state) noexcept;

  static void Run();

  static void Destroy();

  [[nodiscard]] static glm::ivec2 ScreenSize() {
    return screenSize;
  }

private:

  static bool                          created;
  static std::unique_ptr<IEngineState> currentState;
  static Config                        cfg;
  static glm::ivec2                    screenSize;
};
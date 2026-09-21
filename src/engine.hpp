
#pragma once

#include <glm/vec2.hpp>
#include <memory>

struct Config {
  glm::ivec2 WinSize;
  float      Ratio = 0.75;
  char       ScreenshotsPath[32] = "screenshots";
  bool       Debug               = true;
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

  static bool Create(const Config*);

  static bool Created() {
    return created;
  }

  static void SetState(std::unique_ptr<IEngineState>& state) noexcept;

  static void Run();

  static void Destroy();

  static glm::ivec2 GetScreenSize();

private:

  static bool                          created;
  static std::unique_ptr<IEngineState> currentState;
};
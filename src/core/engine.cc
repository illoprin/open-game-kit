#include "engine.hpp"
#include "clock.hpp"
#include "core./input.hpp"
#include "game/initial_ui.hpp"
#include "gfx/gl.hpp"
#include "input.hpp"
#include "log.hpp"
#include "tools.hpp"
#include "window.hpp"

std::unique_ptr<IEngineState> Engine::currentState(nullptr);
bool                          Engine::created = false;
glm::ivec2                    Engine::screenSize{0, 0};
Config                        Engine::cfg = {};

glm::ivec2 computscreenSize(glm::ivec2 size, float r) {
  return {
    static_cast<int>(static_cast<float>(size[0]) * r),
    static_cast<int>(static_cast<float>(size[1]) * r)
  };
}

IEngineState::IEngineState() {
  og_assert(
    Engine::Created(),
    "an attempt to create a state where the engine has not yet been created"
  );
}

bool Engine::Create(const Config& conf) {
  cfg = conf;

  if (created) return false;
  // win size - glm::ivec2
  og_assert(Window::Create(cfg.WinSize), "Failed to create window");
  og_assert(
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
    "Failed to create OpenGL 3.3 context"
  );

  Window::SetResizeCallback([](int w, int h) {
    screenSize = computscreenSize({w, h}, cfg.Ratio);
    if (currentState) currentState->OnResize();
    LOG_INFO(
      "resized (Window: {} {}) (Screen: {} {})",
      w,
      h,
      screenSize.x,
      screenSize.y
    );
  });

  // Register Input callbacks
  glfwSetKeyCallback(
    Window::Handle(),
    [](GLFWwindow*, int key, int scancode, int action, int mods) {
      Input::KeyCallback(key, scancode, action, mods);
    }
  );

  glfwSetMouseButtonCallback(
    Window::Handle(),
    [](GLFWwindow*, int button, int action, int mods) {
      Input::MouseButtonCallback(button, action, mods);
    }
  );

  glfwSetScrollCallback(
    Window::Handle(),
    [](GLFWwindow*, double xoffset, double yoffset) {
      Input::ScrollCallback(xoffset, yoffset);
    }
  );

  glfwSetCursorPosCallback(
    Window::Handle(),
    [](GLFWwindow*, double xpos, double ypos) {
      Input::CursorPosCallback(xpos, ypos);
    }
  );

  Clock::Init();

  Window::Center();
  Window::ShowAndFocus();
  created    = true;
  screenSize = computscreenSize(Window::Size(), cfg.Ratio);

  InitialUI::Init();

  log(LogLevel::Info, "Engine initialized");

  return true;
}

void Engine::SetState(std::unique_ptr<IEngineState>& state) noexcept {
  if (!created) return;
  if (state.get() == currentState.get() || state.get() == nullptr) return;

  if (currentState) currentState->OnExit();

  currentState = std::move(state);

  currentState->OnEnter();
}

uch currentStatsMode = 0;

void Engine::Run() {
  if (!created) return;
  while (GL::PopError()) {}

  Timer t1(1.0, true);
  Timer t30(1.0 / 30.0, true);
  Timer t60(1.0 / 60.0, true);

  while (!Window::ShouldClose()) {
    Input::Update();
    Clock::Update();
    Window::PollEvents();

    if (Input::GetKeyPressed(cfg.ScreenshotKey)) ScreenshotTool::Needs = true;
    if (Input::GetKeyPressed(cfg.DebugStatsSwitchKey))
      DebugUI::SetStatsMode(
        DebugUI::StatsMode(
          currentStatsMode + 1 % uch(DebugUI::StatsMode::Count)
        )
      );

    if (t1.IsExpired()) {
      while (GL::PopError()) {}
    }

    InitialUI::Begin();
    DebugUI::ShowStats();
    if (currentState) currentState->Update();
    InitialUI::End();

    if (t30.IsExpired()) {
      if (currentState) currentState->FixedUpdate30();
    }

    if (t60.IsExpired()) {
      if (currentState) currentState->FixedUpdate60();
    }

    GL::ResetStats();
    VertexArray::Unbind();

    // except rendering to main buffer
    if (currentState) currentState->Render();
    InitialUI::Render();

    ScreenshotTool::Update(cfg.ScreenshotsPath);
    Window::SwapBuffers();
  }
}

void Engine::Destroy() {
  if (!created) return;
  if (currentState) currentState.reset();
  InitialUI::Shutdown();
  Window::Destroy();
  log(LogLevel::Info, "Engine destoyed");
}

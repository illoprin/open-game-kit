#include "engine.hpp"
#include "gl_state.hpp"
#include "input.hpp"
#include "log.hpp"
#include "window.hpp"
#include "clock.hpp"

std::unique_ptr<IEngineState> Engine::currentState(nullptr);
bool                          Engine::created = false;
glm::ivec2                    e_screenSize{0, 0};
const Config* e_cfg = nullptr;

glm::ivec2 computeScreenSize(glm::ivec2 size, float r) {
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

bool Engine::Create(const Config* cfg) {
  cfg ? e_cfg = cfg : e_cfg = new Config;

  if (created) return false;
  // win size - glm::ivec2
  og_assert(Window::Create(e_cfg->WinSize), "Failed to create window");
  og_assert(GL::CreateContext(), "Failed to create OpenGL 3.3 context");

  Window::SetResizeCallback([](int w, int h) {
    e_screenSize = computeScreenSize({w,h}, e_cfg->Ratio);
    if (currentState) currentState->OnResize();
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
  VertexArray::Unbind();
  created = true;
  e_screenSize = computeScreenSize(Window::Size(), e_cfg->Ratio);

  return true;
}

void Engine::SetState(std::unique_ptr<IEngineState>& state) noexcept {
  if (!created) return;
  if (state.get() == currentState.get() || state.get() == nullptr) return;

  if (currentState) currentState->OnExit();

  currentState = std::move(state);

  currentState->OnEnter();
}

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

    if (currentState) currentState->Update();

    if (t1.IsExpired()) {
      while (GL::PopError()) {}
      std::println("FPS: {}", Clock::FPS());
    }

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

    Window::SwapBuffers();
  }
}

void Engine::Destroy() {
  if (!created) return;
  if (currentState) currentState.reset();
  Window::Destroy();
}

glm::ivec2 Engine::GetScreenSize() {
  return e_screenSize;
}

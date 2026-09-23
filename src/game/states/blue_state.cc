#include "blue_state.hpp"
#include "core./log.hpp"
#include "core./utils.hpp"

#include "core/files.hpp"
#include "core/input.hpp"
#include "gfx/gl.hpp"
#include "world/map_repository.hpp"

glm::vec2 vertices[] = {
  {-0.5f, -0.5f}, // 0: Bottom-left
  {0.5f,  -0.5f}, // 1: Bottom-right
  {0.5f,  0.5f }, // 2: Top-right
  {-0.5f, 0.5f }  // 3: Top-left
};

uint indices[] = {
  0,
  1,
  2,  // First triangle
  0,
  2,
  3  // Second triangle
};

BlueState::BlueState() : gBuffer(Engine::ScreenSize()) {

  // load map file
  MapData md;
  if (!md.LoadGameMap(LevelPath("crimson_nexus.ogk.map"))) {
    log(LogLevel::Error, "failed load map");
    std::exit(1);
  };

  // load map assets
  MapRepository repo;
  if (!repo.FromData(md)) {
    log(LogLevel::Error, "failed load map");
    std::exit(1);
  };

  // build gpu objects for map renderering
  mRenderer.Init(md, repo);
  // build colliders
  phys.FromMap(md, repo);

  // program
  og_assert(
    Program::FastLoad(
      pMain,
      ShaderPath("basic.vert"),
      ShaderPath("basic.frag")
    ),
    "failed load program"
  );

  log(LogLevel::Info, "scene loaded");
}

void BlueState::OnEnter() noexcept {
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  log(LogLevel::Info, "blue state enter");
}

void BlueState::OnResize() noexcept {
  gBuffer.Resize(Engine::ScreenSize());
}

void BlueState::Update() noexcept {

  if (Input::GetKeyPressed(GLFW_KEY_ESCAPE)) Window::ToggleMouseGrab();

  if (Window::Grabbed() || Input::IsButtonDown(GLFW_MOUSE_BUTTON_1)) {
    fps.ProcessInput();
    fps.UpdateLook(cam);
  }

  fps.ApplyToCamera(cam);
  cam.Update(Window::Size());
}

void BlueState::FixedUpdate60() noexcept {
  fps.UpdatePhysics(1.0 / 60.0, phys.StaticColliders());
}

void BlueState::Render() noexcept {

  // bind GBuffer for drawing

  gBuffer.BindForDrawing(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  pMain.Use();
  pMain.SetMat4("u_projection", cam.GetProjection());
  pMain.SetMat4("u_view", cam.GetView());
  pMain.SetInt("u_diffuse", 0);

  // render scene

  mRenderer.Render(pMain);

  // blit framebuffer to screen

  GLuint     bufferId   = gBuffer.GetFramebuffer().ID();
  glm::ivec2 bufferSize = gBuffer.Size();
  glm::ivec2 windowSize = Window::Size();

  GL::BlitFramebuffer(
    bufferId,
    0,
    GL_COLOR_ATTACHMENT0,
    GL_BACK,
    bufferSize,
    windowSize
  );
}

BlueState::~BlueState() {
  log(LogLevel::Info, "blue state destroy");
}

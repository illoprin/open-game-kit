#include "blue_state.hpp"
#include "GLFW/glfw3.h"
#include "core./log.hpp"
#include "core./utils.hpp"

#include "core/files.hpp"
#include "core/input.hpp"
#include "gfx/gl.hpp"
#include "world/map_repository.hpp"
#include <array>

bool showColliders = false;

std::array<glm::vec2, 4> vertices = {
  glm::vec2{-1.f, -1.f}, // 0: Bottom-left
  glm::vec2{1.f,  -1.f}, // 1: Bottom-right
  glm::vec2{1.f,  1.f }, // 2: Top-right
  glm::vec2{-1.f, 1.f }  // 3: Top-left
};

std::array<uint, 6> indices = {
  0,
  1,
  2,  // First triangle
  0,
  2,
  3  // Second triangle
};

BlueState::BlueState() : gBuffer(Engine::ScreenSize()) {
  fps.SetPosition(glm::vec3{0, 15, 0});
  fps.Cfg.mouse_sens = 0.08;

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

  // build basic quad
  basicQuad.FromFlat(vertices, indices);

  log(LogLevel::Info, "scene loaded");
}

void BlueState::OnEnter() noexcept {

  log(LogLevel::Info, "blue state enter");
}

void BlueState::OnResize() noexcept {
  gBuffer.Resize(Engine::ScreenSize());
  pipeline.Resize(Engine::ScreenSize());
}

void BlueState::Update() noexcept {

  if (Input::GetKeyPressed(GLFW_KEY_ESCAPE)) Window::ToggleMouseGrab();

  if (Input::GetKeyPressed(GLFW_KEY_F3)) showColliders = !showColliders;

  if (Window::Grabbed() || Input::IsButtonDown(GLFW_MOUSE_BUTTON_1)) {
    fps.UpdateLook(cam);
    fps.ProcessInput();
  }

  fps.ApplyToCamera(cam);
  cam.Update(Window::Size());

  pipeline.DrawUI();
}

void BlueState::FixedUpdate60() noexcept {
  fps.UpdatePhysics(1.0 / 60.0, phys.StaticColliders());
}

void BlueState::Render() noexcept {

  // bind GBuffer for drawing
  gBuffer.BindForDrawing(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  // render scene
  mRenderer.Render(cam);

  // render colliders
  if (showColliders)
    physRenderer.DrawStaticColliders(phys.StaticColliders(), cam);

  const auto& resultBuf = pipeline.Perform(basicQuad, gBuffer, cam);

  // blit framebuffer to screen
  GLuint     bufferId   = resultBuf.ID();
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

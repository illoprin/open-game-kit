#include "camera.hpp"
#include "engine.hpp"
#include "files.hpp"
#include "fly_controller.hpp"
#include "input.hpp"
#include "log.hpp"
#include "map_parser.hpp"
#include "map_renderer.hpp"
#include "program.hpp"
#include "utils.hpp"
#include "window.hpp"
#include <glm/vec3.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

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

class BlueState : public IEngineState {
  Program     pMain;
  MapRenderer mr;

  FlyController controller;
  Camera3D      cam;

public:

  BlueState() {
    // setup controller

    controller.SetMaxSpeed(20.0);
    cam.Position.z += 3.0;
    cam.Position.y += 1.5;

    // load map

    MapData md;
    if (!md.LoadGameMap(LevelPath("crimson_nexus.ogk.map"))) {
      log(LogLevel::Error, "failed load map");
      std::exit(1);
    };

    if (!mr.Initialize(md)) {
      log(LogLevel::Error, "failed to init map renderer");
      std::exit(1);
    };

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

  void OnEnter() noexcept override {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    log(LogLevel::Info, "blue state enter");
  }

  void Update() noexcept override {

    if (Input::GetKeyPressed(GLFW_KEY_ESCAPE)) Window::ToggleMouseGrab();

    if (Window::Grabbed() || Input::IsButtonDown(GLFW_MOUSE_BUTTON_1))
      controller.Update(cam);
    cam.Update(Window::Size());
  }

  void Render() noexcept override {

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glDrawBuffer(GL_BACK);

    // clear framebuffer
    auto size = Window::Size();
    glViewport(0, 0, size.x, size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    pMain.Use();
    pMain.SetMat4("u_projection", cam.GetProjection());
    pMain.SetMat4("u_view", cam.GetView());
    pMain.SetInt("u_diffuse", 0);

    mr.Render(pMain);
  }

  ~BlueState() override {
    log(LogLevel::Info, "blue state destroy");
  }
};

int main() {

  Config conf{
    .WinSize = {1280, 720},
  };

  og_assert(Engine::Create(&conf), "failed to create engine");
  glfwSwapInterval(1);
  std::unique_ptr<IEngineState> state(new BlueState);
  Engine::SetState(state);
  Engine::Run();
  Engine::Destroy();

  return 0;
}
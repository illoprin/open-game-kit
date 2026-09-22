#include "camera.hpp"
#include "engine.hpp"
#include "files.hpp"
#include "fly_controller.hpp"
#include "gl_state.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "input.hpp"
#include "log.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "program.hpp"
#include "resource.hpp"
#include "texture.hpp"
#include "transforms.hpp"
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
  Mesh      mShotgun;
  Mesh      mCrate;
  Texture2D tCrate;
  Texture2D tColors;
  Program   pMain;

  FlyController controller;
  Camera3D      cam;

public:

  BlueState() {
    controller.SetMaxSpeed(8.0);
    cam.Position.z += 3.0;
    cam.Position.y += 1.5;

    // crate model
    auto crateRes = Geometry::FromObj(ModelPath("crate.obj"));
    if (!crateRes.has_value()) {
      log(LogLevel::Error, "failed load crate model\n{}", crateRes.error());
      std::exit(1);
    }
    mCrate.FromGeometry(crateRes.value());

    // shotgun
    auto shotGunRes = Geometry::FromObj(ModelPath("shotgun.obj"));
    if (!shotGunRes.has_value()) {
      log(LogLevel::Error, "failed load shotgun model\n{}", shotGunRes.error());
      std::exit(1);
    }
    mShotgun.FromGeometry(shotGunRes.value());

    // texture

    Image2D img;
    auto    loadTexture = [&](Texture2D& tex, std::string path) {
      og_assert(img.FromFile(TexturePath(path)), "failed load texture");
      tex.FromData(img.Pix(), img.Width(), img.Height(), GL_RGB8);
      tex.GenerateMipmaps();
      tex.SetSamplerState(GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR);
    };

    loadTexture(tCrate, "crate_128.png");
    loadTexture(tColors, "colors.png");

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
    log(LogLevel::Info, "blue state enter");

  }

  void Update() noexcept override {

    if (Input::GetKeyPressed(GLFW_KEY_ESCAPE)) Window::ToggleMouseGrab();

    if (Window::Grabbed() || Input::IsButtonDown(GLFW_MOUSE_BUTTON_1))
      controller.Update(cam);
    cam.Update(Window::Size());
  }

  void renderPrefab(
    const Mesh&      mesh,
    const Texture2D& tex,
    const glm::mat4& model
  ) const {
    // configure
    tex.Bind(0);
    pMain.SetMat4("u_model", model);

    // draw
    GL::DrawElements(mesh.GetVAO(), mesh.GetIndexCount(), GL_UNSIGNED_INT);
  }

  void Render() noexcept override {

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // clear framebuffer
    auto size = Window::Size();
    glViewport(0, 0, size.x, size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    pMain.Use();
    pMain.SetMat4("u_projection", cam.GetProjection());
    pMain.SetMat4("u_view", cam.GetView());
    pMain.SetInt("u_diffuse", 0);

    renderPrefab(mCrate, tCrate, glm::mat4(1.0));
    renderPrefab(
      mShotgun,
      tColors,
      CreateModel(glm::vec3(0, 2.5, 0), glm::vec3(0.35))
    );

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
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
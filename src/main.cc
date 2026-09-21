#include "buffer.hpp"
#include "engine.hpp"
#include "gl_state.hpp"
#include "log.hpp"
#include "resource.hpp"
#include "utils.hpp"
#include "vertex_array.hpp"
#include "window.hpp"
#include <glm/vec3.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

glm::vec2 vertices[] = {
  {-0.5f, -0.5f}, // 0: Bottom-left
  {0.5f,  -0.5f}, // 1: Bottom-right
  {0.5f,  0.5f}, // 2: Top-right
  {-0.5f, 0.5f}  // 3: Top-left
};

uint indices[] = {
  0,1,2,  // First triangle
  0,2,3  // Second triangle
};

class BlueState : public IEngineState {
  VertexArray vao;
  Buffer      vbo{GL_ARRAY_BUFFER};
  Buffer      ebo{GL_ELEMENT_ARRAY_BUFFER};

public:

  BlueState() {

    vbo.Allocate(sizeof(vertices), GL_STATIC_DRAW, vertices);
    ebo.Allocate(sizeof(indices), GL_STATIC_DRAW, indices);
    Attribute in_position = {
      .Location   = 0,
      .Comps      = 2,
      .Type       = GL_FLOAT,
      .Normalized = false,
      .Stride     = sizeof(glm::vec2),
      .Offset     = 0,
    };
    vao.SetAttribute(vbo, {in_position});
    vao.AttachIndexBuffer(ebo);
  }

  void OnEnter() noexcept override {
    std::println("blue state enter");

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
  }

  void Update() noexcept override {
    
  }

  void Render() noexcept override {
    // draw on back buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    // clear framebuffer
    auto size = Window::Size();
    glViewport(0, 0, size.x, size.y);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw
    GL::DrawElements(vao, 6, GL_UNSIGNED_INT);

  }

  ~BlueState() override {
    std::println("blue state destroy");
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
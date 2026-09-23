#include "core/log.hpp"
#include "core/engine.hpp"
#include "game/states/blue_state.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

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
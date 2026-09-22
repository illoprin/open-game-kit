#include "tools.hpp"
#include "image.hpp"
#include "log.hpp"
#include "window.hpp"
#include <chrono>
#include <exception>
#include <filesystem>
#include <format>


namespace fs = std::filesystem;

bool ScreenshotTool::Needs = false;

void ScreenshotTool::Update(const std::string_view& path) noexcept {
  if (Needs) {

    // allocate storage
    glm::ivec2 winSize = Window::Size();
    Image2D    img(winSize.x, winSize.y, 3);

    // read pixels to storage

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glReadBuffer(GL_BACK);
    glReadPixels(
      0,
      0,
      winSize.x,
      winSize.y,
      GL_RGB,
      GL_UNSIGNED_BYTE,
      (void*)img.Pix()
    );

    // Get the current system time
    auto now = std::chrono::system_clock::now();


    // cut off precision to seconds
    std::chrono::zoned_time local_time{
      std::chrono::current_zone(),
      std::chrono::floor<std::chrono::milliseconds>(now)
    };

    // Format directly to a std::string (YYYY-MM-DD HH:MM:SS)
    std::string filepath =
      std::format("{}/{:%Y-%m-%d-%H-%M-%S}.png", path, local_time);

    // check folder
    try {
      if (!fs::is_directory(path)) fs::create_directories(path);
    } catch(const std::exception& e) {
      log(LogLevel::Error, "screenshot - could not create directories\n{}", e.what());
    }

    // write to file
    if (!img.WriteFile(filepath))
      log(LogLevel::Error, "screenshot - failed write image '{}'", filepath);

    Needs = false;
  }
}

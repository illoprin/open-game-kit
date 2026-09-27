#pragma once

#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>


class Window {
public:

  using ResizeCallback = std::function<void(int width, int height)>;

  [[nodiscard]] static bool Create(const glm::ivec2& size);
  static void               Destroy();

  static void Center();
  static void ShowAndFocus();
  static void ToggleMouseGrab();

  [[nodiscard]] static bool SetIcons();

  [[nodiscard]] static bool ShouldClose();

  [[nodiscard]] static bool Created() {
    return created;
  }

  static void PollEvents() {
    if (!created) return;
    glfwPollEvents();
  }

  static void SwapBuffers() {
    if (!created || !handle) return;
    glfwSwapBuffers(handle);
  }

  static void SetResizeCallback(ResizeCallback callback);

  [[nodiscard]] static glm::ivec2  Size();
  [[nodiscard]] static GLFWwindow* Handle();  // GLFWwindow* pointer

  [[nodiscard]] static bool Grabbed() {
    return grabbed;
  }

private:

  static bool           created;
  static bool           grabbed;
  static GLFWwindow*    handle;
  static ResizeCallback resizeCallback;
};
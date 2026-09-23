#pragma once

#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>


class Window {
public:

  using ResizeCallback = std::function<void(int width, int height)>;

  static bool Create(const glm::ivec2& size);
  static void Destroy();

  static void Center();
  static void ShowAndFocus();
  static void ToggleMouseGrab();

  static bool ShouldClose();
  static void PollEvents();
  static void SwapBuffers();

  static void SetResizeCallback(ResizeCallback callback);

  static glm::ivec2  Size();
  static GLFWwindow* Handle();  // GLFWwindow* pointer
  static bool Grabbed();

private:

  static bool           created;
  static bool           grabbed;
  static GLFWwindow*    handle;
  static ResizeCallback resizeCallback;
};
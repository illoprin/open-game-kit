#include "window.hpp"
#include "GLFW/glfw3.h"
#include "log.hpp"
#include "window.hpp"

GLFWwindow*            Window::handle         = nullptr;
Window::ResizeCallback Window::resizeCallback = nullptr;
bool                   Window::created        = false;
bool                   Window::grabbed        = false;
bool wLastGrabbed = false;
bool wRawInputSupport = false;

bool Window::Create(const glm::ivec2& size) {
  if (created) return false;

  if (!glfwInit()) { return false; }

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  handle = glfwCreateWindow(size.x, size.y, "OpenGameKit", nullptr, nullptr);
  if (!handle) {
    log(LogLevel::Error, "Failed to create GLFW window");
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(handle);

  glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* window, int w, int h) {
    Window::resizeCallback(w, h);
  });

  glfwSetWindowFocusCallback(handle, [](GLFWwindow*, int focused) {
    if (!focused) {
      wLastGrabbed = grabbed;
      if (grabbed) ToggleMouseGrab();
    } else {
      if (wLastGrabbed) ToggleMouseGrab();
    }
  });

  wRawInputSupport = glfwRawMouseMotionSupported() ? true : false;

  created = true;

  return true;
}

void Window::Destroy() {
  if (!created) return;
  glfwDestroyWindow(handle);
  glfwTerminate();
}

void Window::Center() {
  if (!created || !handle) return;

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  if (!monitor) return;

  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  if (!mode) return;

  int width, height;
  glfwGetWindowSize(handle, &width, &height);
  glfwSetWindowPos(
    handle,
    (mode->width - width) / 2,
    (mode->height - height) / 2
  );
}

void Window::ShowAndFocus() {
  if (!created || !handle) return;
  glfwShowWindow(handle);
  glfwFocusWindow(handle);
}

void Window::ToggleMouseGrab() {
  if (!created) return;
  grabbed = !grabbed;
  if (grabbed) {
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (wRawInputSupport) glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  } else {
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (wRawInputSupport) glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
  }
}

bool Window::ShouldClose() {
  if (!created || !handle) return true;
  return glfwWindowShouldClose(handle);
}

void Window::PollEvents() {
  if (!created) return;
  glfwPollEvents();
}

void Window::SwapBuffers() {
  if (!created || !handle) return;
  glfwSwapBuffers(handle);
}

void Window::SetResizeCallback(ResizeCallback callback) {
  if (!created) return;
  resizeCallback = std::move(callback);
}

glm::ivec2 Window::Size() {
  if (!created || !handle) return {0, 0};
  int w, h;
  glfwGetFramebufferSize(handle, &w, &h);
  return {w, h};
}

GLFWwindow* Window::Handle() {
  if (!created) return nullptr;
  return handle;
}

bool Window::Grabbed() {
  return grabbed;
}
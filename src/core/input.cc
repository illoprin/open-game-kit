#include "input.hpp"

#include "window.hpp"

struct KeyState {
  bool current  = false;
  bool previous = false;
};

static KeyState s_keys[GLFW_KEY_LAST + 1];
static KeyState s_mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];

static double s_scrollDeltaX = 0.0;
static double s_scrollDeltaY = 0.0;

static double s_cursorX           = 0.0;
static double s_cursorY           = 0.0;
static double s_lastCursorX       = 0.0;
static double s_lastCursorY       = 0.0;
static bool   s_cursorInitialized = false;

void Input::KeyCallback(int key, int /*scancode*/, int action, int /*mods*/) {
  if (key >= 0 && key <= GLFW_KEY_LAST) {
    if (action == GLFW_PRESS) {
      s_keys[key].current = true;
    } else if (action == GLFW_RELEASE) {
      s_keys[key].current = false;
    }
  }
}

void Input::MouseButtonCallback(int button, int action, int /*mods*/) {
  if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
    if (action == GLFW_PRESS) {
      s_mouseButtons[button].current = true;
    } else if (action == GLFW_RELEASE) {
      s_mouseButtons[button].current = false;
    }
  }
}

void Input::ScrollCallback(double xoffset, double yoffset) {
  s_scrollDeltaX += xoffset;
  s_scrollDeltaY += yoffset;
}

void Input::CursorPosCallback(double xpos, double ypos) {
  if (!s_cursorInitialized) {
    s_cursorX           = xpos;
    s_cursorY           = ypos;
    s_lastCursorX       = xpos;
    s_lastCursorY       = ypos;
    s_cursorInitialized = true;
  } else {
    s_cursorX = xpos;
    s_cursorY = ypos;
  }
}

void Input::Update() {
  for (int i = 0; i <= GLFW_KEY_LAST; ++i) {
    s_keys[i].previous = s_keys[i].current;
  }

  for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i) {
    s_mouseButtons[i].previous = s_mouseButtons[i].current;
  }

  s_scrollDeltaX = 0.0;
  s_scrollDeltaY = 0.0;

  s_lastCursorX = s_cursorX;
  s_lastCursorY = s_cursorY;
}

bool Input::GetKeyDown(int key) {
  if (key >= 0 && key <= GLFW_KEY_LAST) { return s_keys[key].current; }
  return false;
}

bool Input::GetKeyUp(int key) {
  if (key >= 0 && key <= GLFW_KEY_LAST) {
    return !s_keys[key].current && s_keys[key].previous;
  }
  return false;
}

bool Input::GetKeyPressed(int key) {
  if (key >= 0 && key <= GLFW_KEY_LAST) {
    return s_keys[key].current && !s_keys[key].previous;
  }
  return false;
}

bool Input::IsButtonDown(int button) {
  if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
    return s_mouseButtons[button].current;
  }
  return false;
}

bool Input::IsButtonReleased(int button) {
  if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
    return !s_mouseButtons[button].current && s_mouseButtons[button].previous;
  }
  return false;
}

bool Input::IsButtonPressed(int button) {
  if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
    return s_mouseButtons[button].current && !s_mouseButtons[button].previous;
  }
  return false;
}

void Input::GetMouseScroll(double& x, double& y) {
  x = s_scrollDeltaX;
  y = s_scrollDeltaY;
}

void Input::GetCursorPos(double& x, double& y) {
  x = s_cursorX;
  y = s_cursorY;
}

void Input::GetCursorDelta(double& x, double& y) {
  x = s_cursorX - s_lastCursorX;
  y = s_cursorY - s_lastCursorY;
}
#include "clock.hpp"

#include "GLFW/glfw3.h"
#include <thread>

double Clock::last_time         = 0.0;
float  Clock::dt                = 0.0f;
float  Clock::smoothed_dt       = 0.0;
float  Clock::target_frame_time = 0; 
uint   Clock::fps               = 0;

static float g_update_timer = 0.0;

void Clock::Init() {
  last_time   = glfwGetTime();
  dt          = 0.0f;
  smoothed_dt = 0.0;
  fps         = 0;
}

void Clock::SetFPSCap(unsigned int cap) {
  if (cap) {
    target_frame_time = 1.0 / static_cast<float>(cap);
    return;
  }
  target_frame_time = 0.0;
}

float Clock::DeltaTime() {
  return dt;
}

uint Clock::FPS() {
  return fps;
}

float Clock::SmoothedDeltaTime() {
  return static_cast<float>(smoothed_dt);
}

double Clock::Time() {
  return glfwGetTime();
}

void Clock::Update() {
  double current_time = glfwGetTime();
  double frame_time   = current_time - last_time;

  if (target_frame_time > 0) {
    if (frame_time < target_frame_time) {
      // passive waiting
      double curr_time;
      do {
        // tell the OS that this thread can be used for more useful tasks
        std::this_thread::yield();
        curr_time = glfwGetTime();
      } while (curr_time - last_time < target_frame_time);
    }
  }

  dt        = static_cast<float>(frame_time);
  last_time = current_time;

  g_update_timer += dt;
  if (g_update_timer > FPS_UPDATE_INTERVAL) {
    fps = dt != 0.0 ? static_cast<uint>(1.0 / dt) : 9999u;
    g_update_timer -= FPS_UPDATE_INTERVAL;
  }

  if (smoothed_dt == 0.0) {
    smoothed_dt = frame_time;
  } else {
    smoothed_dt = smoothed_dt + LERP_FACTOR * (frame_time - smoothed_dt);
  }
}

bool Timer::IsExpired() noexcept {
  current_time += Clock::DeltaTime();
  if (current_time > target_time) {
    if (is_looping) { current_time -= target_time; }
    return true;
  }
  return false;
}
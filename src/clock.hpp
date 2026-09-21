
#include "utils.hpp"

constexpr double LERP_FACTOR         = 0.1;  // for smoothed dt
constexpr float  FPS_UPDATE_INTERVAL = .5;   // seconds

class Clock {

public:

  static void Init();
  static void SetFPSCap(uint fps_cap);

  static float  DeltaTime();
  static uint   FPS();
  static float  SmoothedDeltaTime();
  static double Time();

  static void Update();

private:

  static double last_time;
  static float  dt;
  static float  smoothed_dt;
  static float  target_frame_time;
  static uint   fps;
};

class Timer {
  float current_time = 0.0;
  float target_time;
  bool  is_looping;

public:

  Timer(float _target, bool loop = true)
      : target_time(_target), is_looping(loop) {
  }

  bool IsExpired() noexcept;
};
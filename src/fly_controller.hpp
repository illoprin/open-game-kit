#pragma once

#include "camera.hpp"
#include <glm/vec3.hpp>

class FlyController {
  glm::vec3 velocity{0.0f};

  float max_speed    = 15.0f;  // Максимальная скорость движения
  float acceleration = 35.0f;  // Скорость разгона (быстрое нарастание)
  float friction     = 10.0f;  // Коэффициент затухания (плавность остановки)
  float mouse_sensitivity = 0.1f;  // Чувствительность мыши

public:

  FlyController() = default;

  // Основной метод обновления, вызываемый каждый кадр
  void Update(Camera3D& camera);

  // Настройки
  void SetMaxSpeed(float speed) {
    max_speed = speed;
  }

  void SetAcceleration(float acc) {
    acceleration = acc;
  }

  void SetSensitivity(float sens) {
    mouse_sensitivity = sens;
  }
};
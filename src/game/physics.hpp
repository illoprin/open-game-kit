#pragma once
#include "scene/collision.hpp"
#include "world/map_parser.hpp"
#include "world/map_repository.hpp"
#include <glm/glm.hpp>
#include <vector>


class PhysicsWorld {
public:

  PhysicsWorld() = default;

  void FromMap(const MapData&, const MapRepository&);

  // Ban copy
  PhysicsWorld& operator=(const PhysicsWorld&) = delete;
  PhysicsWorld(const PhysicsWorld&) = delete;

  // Move semantics
  PhysicsWorld(PhysicsWorld&&) = default;
  PhysicsWorld& operator=(PhysicsWorld&&) = default;

  const std::vector<StaticCollider>& StaticColliders() const;

private:

  std::vector<StaticCollider> colliders;
};

// Физическое тело игрока (Капсула)
struct PlayerBody {

  glm::vec3 position{0.0f};  // Позиция основания капсулы (ноги)
  glm::vec3 velocity{0.0f};  // Текущая скорость

  float height = 1.8f;  // Общая высота капсулы
  float radius = 0.4f;  // Радиус капсулы

  bool is_grounded = false;  // Стоит ли на земле

  // Получить AABB для широкой фазы (broadphase) или отладки
  AABB GetAABB() const noexcept {
    return {
      .min = {position.x - radius, position.y,          position.z - radius},
      .max = {position.x + radius, position.y + height, position.z + radius}
    };
  }

  // Получить математическую капсулу для узкой фазы (narrowphase)
  Capsule GetCapsule() const noexcept {
    return {
      .a = position,                                            // Низ
      .b = position + glm::vec3(0, height - radius * 2.0f, 0),  // Верх цилиндра
      .radius = radius
    };
  }

  // Центр масс (для камеры)
  glm::vec3 GetEyePosition(float eye_offset = 1.6f) const noexcept {
    return position + glm::vec3(0.0f, eye_offset, 0.0f);
  }
};
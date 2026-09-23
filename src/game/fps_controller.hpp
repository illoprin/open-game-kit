#pragma once
#include "game/physics.hpp"
#include "scene/camera.hpp"
#include <span>

class FPSController {
public:

  struct Config {
    float move_speed;
    float sprint_mult;
    float jump_force;
    float gravity;
    float ground_drag;  // Ground friction
    float air_drag;     // Air resistance
    float mouse_sens;

    // Provide a constructor to set default values
    Config()
        : move_speed(5.0f),
          sprint_mult(1.6f),
          jump_force(7.0f),
          gravity(-20.0f),
          ground_drag(10.0f),
          air_drag(1.0f),
          mouse_sens(0.1f) {
    }
  };

  /**
   * @brief Construct a new FPSController.
   * @param cfg Controller configuration settings.
   * @param start_pos Initial world position for the player body.
   * @param start_yaw Initial yaw angle in degrees.
   */
  explicit FPSController(
    Config    cfg       = {},
    glm::vec3 start_pos = glm::vec3(0.0f),
    float     start_yaw = 0.0f
  );

  /**
   * @brief Poll input states (WASD, Jump) before physics integration.
   */
  void ProcessInput();

  /**
   * @brief Update player physics and resolve collisions. Call in FixedUpdate.
   */
  void UpdatePhysics(float dt, std::span<const StaticCollider> world_colliders);

  /**
   * @brief Update camera orientation based on mouse delta.
   */
  void UpdateLook(Camera3D& camera);

  /**
   * @brief Synchronize camera position and direction with the player body.
   */
  void ApplyToCamera(Camera3D& camera) const noexcept;

  /**
   * @brief Set the player body position and synchronize camera view vector.
   */
  void SetPosition(const glm::vec3& pos) noexcept;

  /**
   * @brief Set the player looking direction (degrees).
   */
  void SetYaw(float v) noexcept;

  [[nodiscard]] const PlayerBody& GetBody() const noexcept {
    return body;
  }

  [[nodiscard]] bool IsGrounded() const noexcept {
    return body.is_grounded;
  }

private:

  void ResolveCollisions(std::span<const StaticCollider> colliders);
  void ApplyMovementInput(float dt);

  PlayerBody body;
  Config     config;
  glm::vec3  input_dir{0.0f};  // Movement input direction (WASD)
  glm::vec3  front{0, 0, 1};
  bool       want_jump = false;
};
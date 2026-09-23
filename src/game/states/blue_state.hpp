#pragma once

#include "core/engine.hpp"
#include "game/fps_controller.hpp"
#include "game/physics.hpp"
#include "gfx/program.hpp"
#include "gfx/render_target.hpp"
#include "world/map_renderer.hpp"

class BlueState : public IEngineState {

  GBuffer gBuffer;
  Program pMain;

  MapRenderer  mRenderer;
  PhysicsWorld phys;

  FPSController fps;
  Camera3D      cam;

public:

  BlueState();
  void FixedUpdate60() noexcept override;
  void OnEnter() noexcept override;
  void OnResize() noexcept override;
  void Update() noexcept override;
  void Render() noexcept override;

  ~BlueState() override;
};
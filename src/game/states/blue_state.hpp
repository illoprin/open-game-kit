#pragma once

#include "core/engine.hpp"
#include "game/fps_controller.hpp"
#include "game/physics.hpp"
#include "game/physics_debug_renderer.hpp"
#include "gfx/post/post_processing_pipeline.hpp"
#include "gfx/render_target.hpp"
#include "world/map_renderer.hpp"

class BlueState : public IEngineState {

  GBuffer gBuffer;

  MapRenderer  mRenderer;
  PhysicsWorld phys;
  PhysicsDebugRenderer physRenderer;

  PostProcessingPipeline pipeline{Engine::ScreenSize()};
  Mesh basicQuad;

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
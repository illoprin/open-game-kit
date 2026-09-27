#pragma once

#include "core./utils.hpp"
#include "scene/camera.hpp"
#include "scene/transforms.hpp"

namespace DebugUI {

enum class StatsMode : uch {
  Hidden = 0,
  Compact,
  Detailed,
  Count,
};

void ShowStats();
void DrawCameraSliders(Camera3D& cam);
void SetStatsMode(StatsMode);
void DrawTransformsSliders(Transform& trs, const char* id = "transforms");

};  // namespace DebugUI

namespace InitialUI {

// init / destroy

void Init() noexcept;
void Shutdown() noexcept;

// runtime

void Begin() noexcept;
void End() noexcept;
void Render() noexcept;

// capture

bool WantCaptureInput() noexcept;
void SetWantCaptureInput(bool v) noexcept;

};  // namespace InitialUI
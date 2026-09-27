#include "initial_ui.hpp"
#include "core./log.hpp"
#include "core/clock.hpp"
#include "core/engine.hpp"
#include "core/files.hpp"
#include "core/window.hpp"
#include "gfx/gl.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <cstdio>

// -------------------------------------------------------------------
//                       Debug UI
// -------------------------------------------------------------------

namespace DebugUI {

static StatsMode uiStatsMode = StatsMode::Hidden;

void ShowStats() {
  if (uiStatsMode == StatsMode::Hidden) return;

  float padding = static_cast<float>(Engine::GetConfig().UIPadding);

  ImGuiWindowFlags window_flags =
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize
    | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
    | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

  ImGui::SetNextWindowPos({padding, padding});

  if (ImGui::Begin("##StatsOverlay", nullptr, window_flags)) {

    ImGui::Text("FPS: %d", Clock::FPS());
    ImGui::Text("Frametime: %.2f ms", Clock::DeltaTime() * 1000.0);

    if (uiStatsMode == StatsMode::Detailed) {
      GL::RenderStats stats = GL::GetStats();

      ImGui::Separator();
      ImGui::Text("Draw Calls: %d", stats.DrawCalls);
      ImGui::Text("Triangles: %d", stats.Triangles);

      ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
      ImGui::Text("Version: %s", glGetString(GL_VERSION));

      glm::ivec2 screen_size = Engine::ScreenSize();
      glm::ivec2 win_size    = Window::Size();
      ImGui::Text("Window %dx%d", win_size.x, win_size.y);
      ImGui::Text("Screen %dx%d", screen_size.x, screen_size.y);
    }
    ImGui::End();
  }
}

void SetStatsMode(StatsMode mode) {
  uiStatsMode = mode;
}

void DrawCameraSliders(Camera3D& cam) {
  ImGui::Text("Camera");

  ImGui::DragFloat3(
    "Position",
    &cam.Position[0],
    .1f,
    0,
    0,
    "%.2f",
    ImGuiDragDropFlags_None
  );
  ImGui::DragFloat3(
    "Rotatiton",
    &cam.Rotation[0],
    .1f,
    0,
    0,
    "%.2f",
    ImGuiDragDropFlags_None
  );
}

void DrawTransformsSliders(Transform& trs, const char* id) {
  ImGui::Text("%s", id);
  char buf[32];
  std::sprintf(buf, "Position##%s", id);
  ImGui::DragFloat3(
    buf,
    &trs.Position[0],
    .01f,
    0,
    0,
    "%.2f",
    ImGuiDragDropFlags_None
  );
  std::sprintf(buf, "Rotation##%s", id);
  ImGui::DragFloat3(
    buf,
    &trs.Rotation[0],
    .01f,
    0,
    0,
    "%.2f",
    ImGuiDragDropFlags_None
  );
  std::sprintf(buf, "Scale##%s", id);
  ImGui::DragFloat3(
    buf,
    &trs.Scale[0],
    .01f,
    0,
    0,
    "%.2f",
    ImGuiDragDropFlags_None
  );
}

}  // namespace DebugUI

// -------------------------------------------------------------------
//                       Initial UI
// -------------------------------------------------------------------

static ImFont* g_inter_font = nullptr;

static void ui_setup_fonts() {
  ImGuiIO& io = ImGui::GetIO();

  std::string inter_font_path{FontPath("inter_regular_28.ttf")};

  g_inter_font = io.Fonts->AddFontFromFileTTF(
    inter_font_path.c_str(),
    18.0,
    nullptr,
    io.Fonts->GetGlyphRangesCyrillic()
  );

  og_assert(
    g_inter_font != nullptr,
    "failed to load \"Inter Regular 28pt\" font"
  );

  io.FontDefault = g_inter_font;
}

static void ui_setup_style() {
  ImGuiStyle& style = ImGui::GetStyle();

  ImVec4* colors                       = style.Colors;
  colors[ImGuiCol_WindowBg]            = ImVec4(0.059f, 0.059f, 0.059f, 0.800f);
  colors[ImGuiCol_FrameBg]             = ImVec4(0.48f, 0.33f, 0.16f, 0.54f);
  colors[ImGuiCol_FrameBgHovered]      = ImVec4(0.98f, 0.65f, 0.26f, 0.40f);
  colors[ImGuiCol_FrameBgActive]       = ImVec4(0.98f, 0.65f, 0.26f, 0.67f);
  colors[ImGuiCol_TitleBgActive]       = ImVec4(0.478f, 0.280f, 0.159f, 1.f);
  colors[ImGuiCol_CheckMark]           = ImVec4(0.98f, 0.65f, 0.26f, 1.00f);
  colors[ImGuiCol_SliderGrab]          = ImVec4(0.88f, 0.59f, 0.24f, 1.00f);
  colors[ImGuiCol_SliderGrabActive]    = ImVec4(0.98f, 0.65f, 0.26f, 1.00f);
  colors[ImGuiCol_Button]              = ImVec4(0.98f, 0.65f, 0.26f, 0.40f);
  colors[ImGuiCol_ButtonHovered]       = ImVec4(0.98f, 0.65f, 0.26f, 1.00f);
  colors[ImGuiCol_ButtonActive]        = ImVec4(0.98f, 0.56f, 0.06f, 1.00f);
  colors[ImGuiCol_Header]              = ImVec4(0.98f, 0.583f, 0.261f, 0.31f);
  colors[ImGuiCol_HeaderHovered]       = ImVec4(0.98f, 0.65f, 0.26f, 0.80f);
  colors[ImGuiCol_HeaderActive]        = ImVec4(0.98f, 0.65f, 0.26f, 1.00f);
  colors[ImGuiCol_SeparatorHovered]    = ImVec4(0.75f, 0.45f, 0.10f, 0.78f);
  colors[ImGuiCol_SeparatorActive]     = ImVec4(0.75f, 0.45f, 0.10f, 1.00f);
  colors[ImGuiCol_ResizeGrip]          = ImVec4(0.98f, 0.65f, 0.26f, 0.20f);
  colors[ImGuiCol_ResizeGripHovered]   = ImVec4(0.98f, 0.65f, 0.26f, 0.67f);
  colors[ImGuiCol_ResizeGripActive]    = ImVec4(0.98f, 0.65f, 0.26f, 0.95f);
  colors[ImGuiCol_TabHovered]          = ImVec4(0.98f, 0.65f, 0.26f, 0.80f);
  colors[ImGuiCol_Tab]                 = ImVec4(0.58f, 0.40f, 0.18f, 0.86f);
  colors[ImGuiCol_TabSelected]         = ImVec4(0.68f, 0.46f, 0.20f, 1.00f);
  colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.98f, 0.65f, 0.26f, 1.00f);
  colors[ImGuiCol_TabDimmed]           = ImVec4(0.15f, 0.11f, 0.07f, 0.97f);
  colors[ImGuiCol_TabDimmedSelected]   = ImVec4(0.42f, 0.29f, 0.14f, 1.00f);
  colors[ImGuiCol_DockingPreview]      = ImVec4(0.98f, 0.65f, 0.26f, 0.70f);
  colors[ImGuiCol_TextLink]            = ImVec4(0.98f, 0.65f, 0.26f, 1.00f);
  colors[ImGuiCol_TextSelectedBg]      = ImVec4(0.98f, 0.65f, 0.26f, 0.35f);
  colors[ImGuiCol_NavCursor]           = ImVec4(0.98f, 0.65f, 0.26f, 1.00f);

  style.WindowRounding = 6.0;
  style.ChildRounding  = 6.0;
  style.FrameRounding  = 6.0;
  style.PopupRounding  = 6.0;
  style.GrabRounding   = 6.0;

  style.WindowBorderSize = 0;
  style.FrameBorderSize  = 0;
  style.TabBorderSize    = 0;

  style.WindowPadding = ImVec2{10, 10};
  style.FramePadding  = ImVec2{10, 5};

  style.ItemSpacing      = ImVec2{8, 6};
  style.ItemInnerSpacing = ImVec2{6, 4};

  style.ScrollbarSize = 15;

  style.TabBarBorderSize   = 0;
  style.TabBarOverlineSize = 0;
  style.TabRounding        = 12;
}

static void ui_init_impl() {
  ImGui_ImplGlfw_InitForOpenGL(Window::Handle(), true);
  ImGui_ImplOpenGL3_Init("#version 330 core");
}

namespace InitialUI {

void Init() noexcept {
  og_assert(Window::Created(), "unable to init ui without glfw window");

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io     = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

  ui_init_impl();
  ui_setup_fonts();
  ui_setup_style();
}

void Begin() noexcept {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void End() noexcept {
  ImGui::Render();
}

void Render() noexcept {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Shutdown() noexcept {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

bool WantCaptureInput() noexcept {
  ImGuiIO& io = ImGui::GetIO();
  return io.WantCaptureMouse || io.WantCaptureKeyboard;
}

void SetWantCaptureInput(bool v) noexcept {
  ImGui::SetNextFrameWantCaptureMouse(v);
  ImGui::SetNextFrameWantCaptureKeyboard(v);
}

}  // namespace InitialUI

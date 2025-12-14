#pragma once

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui {
	enum ImGuiDPadDirection { None = 0, Up, Down, Left, Right };
	ImGuiDPadDirection ImGuiDPad(const char* id, float r0, float r1, float offset = 0.f, bool slice = true, ImVec2 center = ImVec2(FLT_MAX, FLT_MAX));
}
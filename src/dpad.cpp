#include "dpad.hpp"

namespace ImGui {
    constexpr double M_PI180 = IM_PI / 180.f;

    static void ringSlice(ImVector<ImVec2>& pts, ImVec2 center, float r0, float r1, float a0, float a1, float offset = 0.f, bool slice = true, int steps = 32) {
        pts.clear();
        if (a1 < a0) a1 += IM_PI * 2.f;

        const float mid = 0.5f * (a0 + a1);
        ImVec2 shift = ImVec2(cosf(mid), sinf(mid)) * offset;
        ImVec2 shift2 = ImVec2(cosf(mid), sinf(mid)) * (offset + r1 - r0);

        pts.resize((steps + 1) * 2);

        for (int i = 0; i <= steps; ++i) {
            float t = (float)i / steps;
            float a = a0 + (a1 - a0) * t;
            ImVec2 dir(cosf(a), sinf(a));
            pts[i] = center + dir * (slice ? r1 : r0) + (slice ? shift : shift2);
            pts[(steps + 1) * 2 - 1 - i] = center + dir * r0 + shift;
        }
    }

    static bool ringButton(const char* id, const ImVec2* pts, int count) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiID wid = window->GetID(id);

        ImRect bb(pts[0], pts[0]);
        for (int i = 1; i < count; ++i)
            bb.Add(pts[i]);
        ImGui::ItemAdd(bb, wid);

        ImVec2 mouse = ImGui::GetIO().MousePos;

        bool inside = false;
        for (int i = 0, j = count - 1; i < count; j = i++) {
            const ImVec2& a = pts[i];
            const ImVec2& b = pts[j];
            if (((a.y > mouse.y) != (b.y > mouse.y)) && (mouse.x < (b.x - a.x) * (mouse.y - a.y) / (b.y - a.y + 1e-5f) + a.x))
                inside = !inside;
        }

        bool hovered = bb.Contains(mouse) && inside;

        bool pressed = false, held = false;
        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            ImGui::SetActiveID(wid, window);
        if (ImGui::GetActiveID() == wid) {
            held = ImGui::IsMouseDown(ImGuiMouseButton_Left);
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                pressed = hovered;
                ImGui::ClearActiveID();
            }
        }
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 color = hovered ? (held ? style.Colors[ImGuiCol_ButtonActive] : style.Colors[ImGuiCol_ButtonHovered]) : style.Colors[ImGuiCol_Button];
        window->DrawList->AddConcavePolyFilled(pts, count, ImGui::ColorConvertFloat4ToU32(color));
        return pressed;
    }

    ImGuiDPadDirection ImGuiDPad(const char* id, float r0, float r1, float offset, bool slice, ImVec2 center) {
        ImVector<ImVec2> pts;

        float sector = 45.f * M_PI180;
        struct Slice { ImGuiDPadDirection dir; float a; const char* name; };
        const Slice slices[] = {
            { Right, 0.f,          "_right" },
            { Down,  IM_PI * 0.5f, "_down" },
            { Left,  IM_PI,        "_left" },
            { Up,    IM_PI * 1.5f, "_up" }
        };

        bool conc = center.x == FLT_MAX && center.y == FLT_MAX;
        if (conc) center = ImGui::GetCursorScreenPos() + ImVec2(r1, r1);

        ImGuiDPadDirection result = None;
        for (auto& s : slices) {
            ringSlice(pts, center, r0 + offset, r1, s.a - sector, s.a + sector, offset / sqrtf(2.f), slice);
            if (ringButton((std::string(id) + s.name).c_str(), pts.Data, pts.Size))
                result = s.dir;
        }

        if (conc) ImGui::Dummy(ImVec2(r1, r1) * 2.f);
        return result;
    }
}


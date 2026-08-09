#include "imgui.h"
#include "imgui_internal.h"

void ImGui::SwitchButton(const char* str_id, bool* v)
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = 46;
    float width = height * 2.2f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked()) *v = !*v;

    // Fixed animation
    float t = *v ? 1.0f : 0.0f;
    ImGuiContext& gg = *GImGui;
    float ANIM_SPEED = 0.085f;
    if (gg.LastActiveId == gg.CurrentWindow->GetID(str_id)) {
        float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
        t = *v ? t_anim : (1.0f - t_anim);
    }

    bool hovered = ImGui::IsItemHovered();

    // Background color with animation
    ImVec4 bgOn = colors[ImGuiCol_ButtonActive];
    ImVec4 bgOff = ImVec4(17.0f/255.0f, 183.0f/255.0f, 103.0f/255.0f, 1.0f);
    ImVec4 bg = ImVec4(
        bgOff.x + (bgOn.x - bgOff.x) * t,
        bgOff.y + (bgOn.y - bgOff.y) * t,
        bgOff.z + (bgOn.z - bgOff.z) * t,
        1.0f
    );

    if (hovered) {
        bg.x = ImMin(bg.x + 0.1f, 1.0f);
        bg.y = ImMin(bg.y + 0.1f, 1.0f);
        bg.z = ImMin(bg.z + 0.1f, 1.0f);
    }

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height),
        ImGui::GetColorU32(bg), height * 0.5f);

    // Knob with smooth position
    float knob_x = p.x + radius + t * (width - radius * 2.0f);
    draw_list->AddCircleFilled(ImVec2(knob_x, p.y + radius),
        radius - 1.5f, IM_COL32(255, 255, 255, 255));

    // Label
    RenderText(ImVec2(p.x + width * 1.25f, p.y + radius - 15), str_id);
}

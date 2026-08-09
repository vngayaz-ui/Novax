# ImGui Professional UI Design — Complete Research

## 1. CUSTOM DRAWING (ImDrawList) — The Foundation

ImGui mein professional UI banane ka **sabse important tool** `ImDrawList` hai. Ye low-level drawing primitives deta hai jo CSS ki jagah le sakte hain.

### Available Drawing Functions:
```cpp
ImDrawList* draw = ImGui::GetWindowDrawList();

// Rectangles — Cards, Panels, Backgrounds
draw->AddRectFilled(p_min, p_max, color, rounding, flags);       // Filled rounded rect
draw->AddRect(p_min, p_max, color, rounding, flags, thickness);  // Border/outline

// Circles — Toggles, Indicators, Dots
draw->AddCircleFilled(center, radius, color);                    // Filled circle
draw->AddCircle(center, radius, color, segments, thickness);     // Circle outline

// Lines — Separators, Dividers
draw->AddLine(p1, p2, color, thickness);                         // Simple line
draw->AddLineH(y, min_x, max_x, color, thickness);              // Horizontal (optimized)
draw->AddLineV(x, max_y, min_y, color, thickness);              // Vertical (optimized)

// Text — Labels, Headers
draw->AddText(pos, color, text, text_end);                       // Raw text

// Gradients — 4-corner color fill
draw->AddRectFilledMultiColor(p_min, p_max, col_tl, col_tr, col_br, col_bl);

// Shadows
draw->AddShadowRect(p_min, p_max, color, thickness, offset, flags, rounding);
```

### Rounded Corners — Individual Control:
```cpp
// Round all corners
draw->AddRectFilled(p_min, p_max, color, 10.0f);

// Round only specific corners (top-left and top-right)
draw->AddRectFilled(p_min, p_max, color, 10.0f, ImDrawFlags_RoundCornersTop);

// Round everything EXCEPT right side
draw->AddRectFilled(p_min, p_max, color, 10.0f, ImDrawFlags_RoundCornersAll & ~ImDrawFlags_RoundCornersRight);
```

### Gradient Fill with Rounded Corners:
```cpp
// AddRectFilledMultiColor doesn't support rounding directly
// Workaround: draw white rounded rect, then shade vertices
int vtx_start = draw->VtxBuffer.Size;
draw->AddRectFilled(p_min, p_max, IM_COL32_WHITE, rounding);
int vtx_end = draw->VtxBuffer.Size;
ImGui::ShadeVertsLinearColorGradientKeepAlpha(draw, vtx_start, vtx_end, p_min, p_max, col_top, col_bottom);
```

---

## 2. LAYOUT TECHNIQUES

### Responsive Sizing:
```cpp
// Use available width
ImVec2 avail = ImGui::GetContentRegionAvail();
ImGui::Button("Full Width", ImVec2(avail.x, 0));

// Fixed width, dynamic height
ImGui::Button("Fixed Width", ImVec2(200, 50));
```

### Horizontal Layout:
```cpp
ImGui::Button("Left");
ImGui::SameLine(0.0f, 10.0f);  // 10px spacing
ImGui::Button("Right");
```

### Vertical Spacing:
```cpp
ImGui::Spacing();           // Small space
ImGui::Dummy(ImVec2(0, 20)); // 20px vertical space
ImGui::Separator();         // Horizontal line
```

### Grouped Elements (for background behind multiple widgets):
```cpp
ImVec2 group_start = ImGui::GetCursorScreenPos();
ImGui::BeginGroup();
// ... widgets here ...
ImGui::EndGroup();
float group_height = ImGui::GetCursorPosY() - group_start.y;

// Draw background BEHIND the group (use background draw list)
ImGui::GetBackgroundDrawList()->AddRectFilled(
    group_start,
    ImVec2(group_start.x + width, group_start.y + group_height),
    IM_COL32(30, 30, 40, 255), 10.0f);
```

### Child Windows (Scrollable Sections):
```cpp
ImGui::BeginChild("ScrollArea", ImVec2(0, 300), true);
// ... scrollable content ...
ImGui::EndChild();
```

### Tables for Column Layout:
```cpp
if (ImGui::BeginTable("##table", 2, ImGuiTableFlags_BordersOuter)) {
    ImGui::TableSetupColumn("Label");
    ImGui::TableSetupColumn("Value");
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("Setting");
    ImGui::TableSetColumnIndex(1);
    ImGui::Checkbox("##setting", &value);
    ImGui::EndTable();
}
```

---

## 3. STYLE MANAGEMENT

### Temporary Style Changes (Push/Pop):
```cpp
// Color changes
ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.9f, 1.0f));
ImGui::Button("Styled");
ImGui::PopStyleColor();

// Style variable changes
ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 6));
ImGui::Checkbox("Rounded", &value);
ImGui::PopStyleVar(2);
```

### Key Style Variables:
```cpp
ImGuiStyle& style = ImGui::GetStyle();

// Visual Polish
style.WindowRounding = 8.0f;        // Window corners
style.FrameRounding = 6.0f;         // Widget corners
style.GrabRounding = 4.0f;          // Slider/grab corners
style.ScrollbarRounding = 8.0f;     // Scrollbar corners
style.TabRounding = 6.0f;           // Tab corners

// Spacing & Padding
style.WindowPadding = ImVec2(12, 12);    // Inside window
style.FramePadding = ImVec2(10, 6);      // Inside widgets
style.ItemSpacing = ImVec2(8, 6);        // Between widgets
style.ItemInnerSpacing = ImVec2(6, 4);   // Inside composed widgets
style.IndentSpacing = 20.0f;             // Nesting indent

// Borders
style.WindowBorderSize = 1.0f;
style.FrameBorderSize = 0.0f;
style.PopupBorderSize = 1.0f;

// Scrollbar
style.ScrollbarSize = 14.0f;
```

---

## 4. CUSTOM WIDGETS

### Professional Toggle Button:
```cpp
bool ToggleButton(const char* label, bool* v, const char* id) {
    ImGui::Text("%s", label);
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);

    float height = 30.0f;
    float width = 56.0f;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw = ImGui::GetWindowDrawList();

    ImGui::InvisibleButton(id, ImVec2(width, height));
    bool changed = false;
    if (ImGui::IsItemClicked()) { *v = !*v; changed = true; }

    // Animation
    float t = *v ? 1.0f : 0.0f;
    ImGuiContext& g = *GImGui;
    if (g.LastActiveId == g.CurrentWindow->GetID(id)) {
        float t_anim = ImSaturate(g.LastActiveIdTimer / 0.085f);
        t = *v ? t_anim : (1.0f - t_anim);
    }

    // Background
    ImVec4 bgOff(0.20f, 0.20f, 0.25f, 1.0f);
    ImVec4 bgOn(0.0f, 0.75f, 0.85f, 1.0f);
    ImVec4 bg = {
        bgOff.x + (bgOn.x - bgOff.x) * t,
        bgOff.y + (bgOn.y - bgOff.y) * t,
        bgOff.z + (bgOn.z - bgOff.z) * t, 1.0f
    };
    draw->AddRectFilled(p, ImVec2(p.x + width, p.y + height),
                        ImGui::GetColorU32(bg), height * 0.5f);

    // Border
    draw->AddRect(p, ImVec2(p.x + width, p.y + height),
                  IM_COL32(255, 255, 255, 30), height * 0.5f, 0, 1.0f);

    // Knob
    float knob_x = p.x + (height * 0.5f) + t * (width - height);
    draw->AddCircleFilled(ImVec2(knob_x, p.y + height * 0.5f),
                          height * 0.38f, IM_COL32(255, 255, 255, 255));
    return changed;
}
```

### Card Panel with Header:
```cpp
void CardHeader(const char* title, bool* open) {
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImVec2 p = ImGui::GetCursorScreenPos();
    float header_height = 40.0f;

    // Header background
    ImGui::GetWindowDrawList()->AddRectFilled(
        p, ImVec2(p.x + avail.x, p.y + header_height),
        IM_COL32(20, 25, 35, 255), 8.0f, ImDrawFlags_RoundCornersTop);

    // Header border
    ImGui::GetWindowDrawList()->AddRect(
        p, ImVec2(p.x + avail.x, p.y + header_height),
        IM_COL32(0, 180, 210, 60), 8.0f, ImDrawFlags_RoundCornersTop, 1.0f);

    // Title text
    ImGui::SetCursorScreenPos(ImVec2(p.x + 12, p.y + 8));
    ImGui::TextColored(ImVec4(0.0f, 0.9f, 1.0f, 1.0f), "%s", title);

    // Collapse indicator
    ImGui::SetCursorScreenPos(ImVec2(p.x + avail.x - 30, p.y + 8));
    ImGui::Text("%s", *open ? "-" : "+");

    ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + header_height));
    ImGui::Dummy(ImVec2(0, 0));
}
```

### Gradient Button:
```cpp
bool GradientButton(const char* label, const ImVec2& size, ImU32 col1, ImU32 col2) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size_calc = ImGui::CalcItemSize(size, ImGui::CalcTextSize(label).x + 20, 40);
    ImRect bb(pos, pos + size_calc);

    ImGui::ItemSize(size_calc, 5.0f);
    if (!ImGui::ItemAdd(bb, 0)) return false;

    bool hovered, held;
    ImGui::ButtonBehavior(bb, 0, &hovered, &held);

    // Draw gradient background
    int vtx_start = window->DrawList->VtxBuffer.Size;
    window->DrawList->AddRectFilled(bb.Min, bb.Max, col1, 6.0f);
    int vtx_end = window->DrawList->VtxBuffer.Size;
    ImGui::ShadeVertsLinearColorGradientKeepAlpha(window->DrawList, vtx_start, vtx_end,
                                                   bb.Min, ImVec2(bb.Max.x, bb.Min.y), col1, col2);

    // Draw text centered
    ImVec2 text_size = ImGui::CalcTextSize(label);
    ImVec2 text_pos(bb.Min.x + (size_calc.x - text_size.x) * 0.5f,
                    bb.Min.y + (size_calc.y - text_size.y) * 0.5f);
    window->DrawList->AddText(text_pos, IM_COL32_WHITE, label);

    return held;
}
```

---

## 5. PROFESSIONAL DESIGN PATTERNS

### Card-Based Layout:
```
┌──────────────────────────────┐
│  SECTION HEADER (colored)    │  ← Rounded top corners
├──────────────────────────────┤
│  Toggle 1            [ON]    │  ← Custom toggle
│  Toggle 2           [OFF]    │
│  Toggle 3            [ON]    │
├──────────────────────────────┤
│  Slider 1  ═══════○══════   │  ← Custom slider
│  Slider 2  ═══○═════════    │
└──────────────────────────────┘  ← Rounded bottom corners
```

### Visual Hierarchy:
1. **Window background**: Darkest (0.06-0.08)
2. **Card/Panel background**: Slightly lighter (0.10-0.12)
3. **Input/Frame background**: Medium (0.12-0.15)
4. **Text**: Bright (0.90-1.00)
5. **Accent colors**: Cyan/Pink for ON states
6. **Borders**: Subtle (alpha 30-60)

### Spacing Rules:
- **Window padding**: 12-16px
- **Frame padding**: 8-12px horizontal, 4-6px vertical
- **Item spacing**: 6-8px horizontal, 4-6px vertical
- **Section separator**: 8-12px vertical + thin line

### Color Contrast:
- **ON toggle**: Bright cyan (0, 192, 217) on dark background
- **OFF toggle**: Dark gray (51, 51, 64) — clearly different from ON
- **Hover state**: Slightly brighter than normal
- **Active state**: Full brightness

---

## 6. SHADOWS & DEPTH

### Drop Shadow:
```cpp
// Shadow behind a card
draw->AddRectFilled(
    ImVec2(p.x + 2, p.y + 4),  // Offset
    ImVec2(p.x + width + 2, p.y + height + 4),
    IM_COL32(0, 0, 0, 40), rounding);

// Actual card on top
draw->AddRectFilled(p, ImVec2(p.x + width, p.y + height),
                    card_color, rounding);
```

### Neon Glow (for toggles/buttons when ON):
```cpp
if (is_on) {
    // Outer glow
    draw->AddRect(
        ImVec2(p.x - 2, p.y - 2),
        ImVec2(p.x + width + 2, p.y + height + 2),
        IM_COL32(0, 200, 220, 30), rounding, 0, 2.0f);
    // Inner glow
    draw->AddRect(
        ImVec2(p.x - 1, p.y - 1),
        ImVec2(p.x + width + 1, p.y + height + 1),
        IM_COL32(0, 200, 220, 60), rounding, 0, 1.0f);
}
```

---

## 7. FONT & TEXT

### Text Alignment:
```cpp
// Left-aligned (default)
ImGui::Text("Left aligned");

// Right-aligned (use SameLine with calculated offset)
float text_width = ImGui::CalcTextSize("Right aligned").x;
ImGui::SameLine(ImGui::GetContentRegionAvail().x - text_width);
ImGui::Text("Right aligned");

// Centered in available width
float text_width = ImGui::CalcTextSize("Centered").x;
ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
ImGui::Text("Centered");
```

### Colored Text Sections:
```cpp
ImGui::TextColored(ImVec4(0.0f, 0.9f, 1.0f, 1.0f), "Accent Header");
ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.75f, 1.0f), "Secondary text");
ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.4f, 1.0f), "Warning/Important");
```

---

## 8. PERFORMANCE TIPS

1. **Batch draw calls** — Group similar primitives
2. **Use high-level functions** — `AddRect()` over manual vertex allocation
3. **Cache text sizes** — `CalcTextSize()` once, reuse
4. **Minimize Push/Pop** — Use direct modification for permanent changes
5. **Avoid unnecessary clipping** — Only use when needed

---

## 9. KEY TAKEAWAY

**ImGui is NOT HTML/CSS.** You cannot use CSS classes, flexbox, or grid. Instead:
- Use `ImDrawList` for custom shapes (cards, toggles, gradients)
- Use `InvisibleButton()` + draw calls for custom interactive widgets
- Use `BeginGroup()`/`EndGroup()` with background drawing for sections
- Use proper spacing/padding via `ImGuiStyle` variables
- Use `PushStyleColor()`/`PushStyleVar()` for temporary theming

The "professional look" comes from:
1. **Consistent spacing** (not cramped, not too spread)
2. **Visual hierarchy** (background → cards → text → accent)
3. **Rounded corners** (soft, modern feel)
4. **Custom toggles** (not default ImGui checkboxes)
5. **Proper color contrast** (dark bg + bright accents)
6. **Subtle borders** (low alpha, not harsh)
7. **Shadows/depth** (layered backgrounds)

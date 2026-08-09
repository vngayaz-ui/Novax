# NovaX UI Improvement Research — Complete Findings

## 1. Icon Fonts — Why & How

### Problem
Current NovaX UI has **no icons** — only text labels ("HOME", "ESP", "AIM", "Memory", "Settings"). This looks cheap and unprofessional on mobile.

### Solution: Font Awesome / Material Icons
**Best approach:** Merge icon font into main font using `ImFontConfig::MergeMode = true`.

**Step-by-step:**
1. Download Font Awesome 5 Solid (`fa-solid-900.ttf`) or Font Awesome 7
2. Convert to byte array using `binary_to_compressed_c.exe`:
   ```
   binary_to_compressed_c.exe fa-solid-900.ttf FontAwesome > FontAwesome.h
   ```
3. Load in ImGui init:
   ```cpp
   // Main font (32px for touch)
   io.Fonts->AddFontFromMemoryCompressedTTF(font_data, font_size, 32.0f);
   
   // Merge icons into main font
   ImFontConfig config;
   config.MergeMode = true;
   config.PixelSnapH = true;
   float iconFontSize = 32.0f * 2.0f / 3.0f; // icons slightly smaller
   static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
   io.Fonts->AddFontFromMemoryCompressedTTF(
       icon_compressed_data, icon_compressed_size,
       iconFontSize, &config, icon_ranges
   );
   ```

4. Use icons in code:
   ```cpp
   ImGui::Text(ICON_FA_HOME "  HOME");
   ImGui::Text(ICON_FA_EYE "  ESP");
   ImGui::Text(ICON_FA_CROSSHAIRS "  AIM");
   ImGui::Text(ICON_FA_MEMORY "  Memory");
   ImGui::Text(ICON_FA_COG "  Settings");
   ImGui::Text(ICON_FA_SAVE "  Save");
   ImGui::Text(ICON_FA_POWER_OFF "  Exit");
   ```

### Available Icon Sets
| Font | Icons | Best For |
|------|-------|----------|
| Font Awesome 5 Solid | 1000+ | General purpose, most popular |
| Font Awesome 7 | 2000+ | Latest, most comprehensive |
| Material Design Icons | 2500+ | Material design style |
| Kenney Game Icons | 100+ | Game-specific (controller, sword, shield) |
| IconsKenney.h | Game-focused | Perfect for game overlay |

### References
- [IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders) — C/C++ headers for icon fonts
- [ImGui FONTS.md](https://github.com/ocornut/imgui/blob/master/docs/FONTS.md) — Official ImGui font guide
- [Android-Imgui-Template](https://github.com/Shad0w-KuNgen/Android-Imgui-Template) — Uses Font Awesome 5 + 7

---

## 2. Toggle/Switch Widgets — Modern Design

### Problem
Current ToggleButton is functional but basic (50x25px, flat colors). Looks small and cheap.

### Solution: Modern Animated Toggle

**Option A: Simple animated toggle (recommended for NovaX)**
```cpp
void ToggleButton(const char* str_id, bool* v) {
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    float height = ImGui::GetFrameHeight();
    float width = height * 1.8f;      // wider ratio
    float radius = height * 0.50f;
    
    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked()) *v = !*v;
    
    float t = *v ? 1.0f : 0.0f;
    ImGuiContext& g = *GImGui;
    float ANIM_SPEED = 0.085f;
    if (g.LastActiveId == g.CurrentWindow->GetID(str_id)) {
        float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
        t = *v ? (t_anim) : (1.0f - t_anim);
    }
    
    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = ImGui::GetColorU32(ImLerp(
            ImVec4(0.78f, 0.78f, 0.78f, 1.0f),
            ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
    else
        col_bg = ImGui::GetColorU32(ImLerp(
            ImVec4(0.85f, 0.85f, 0.85f, 1.0f),
            ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));
    
    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height),
        col_bg, height * 0.5f);
    draw_list->AddCircleFilled(
        ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius),
        radius - 1.5f, IM_COL32(255, 255, 255, 255));
}
```

**Option B: Full-featured toggle library**
- [cmdwtf/imgui_toggle](https://github.com/cmdwtf/imgui_toggle) — Production-ready:
  - iOS style, Material style, Minecraft style presets
  - Animated, Bordered, Shadowed, Accessibility labels
  - Custom colors via ImGuiTogglePalette
  - DPI-aware scaling

**Option C: Multiple toggle styles**
```cpp
// iOS style (green/white)
ImGui::Toggle("##ios", &v, ImGuiToggleFlags_Animated, 0.5f);

// Material style (purple)
ImGui::Toggle("##material", &v, ImGuiToggleFlags_Animated, 0.3f);

// Glowing style (neon green/red)
ImGui::Toggle("##glow", &v, ImGuiToggleFlags_Animated | ImGuiToggleFlags_Shadowed);
```

### Recommended Sizes for Touch
| Element | Current | Recommended |
|---------|---------|-------------|
| Toggle width | 50px | 80-100px |
| Toggle height | 25px | 36-44px (48dp minimum) |
| Knob radius | 10px | 14-16px |
| Hit area | 50x25 | 80x44+ (invisible padding) |

---

## 3. Slider Improvements

### Problem
Current HorizontalSlider is functional but plain. Values are hard to read.

### Solution: Enhanced Sliders

**Option A: Trail/filled slider**
```cpp
void EnhancedSlider(const char* label, float* v, float min, float max) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::SliderFloat(label, v, min, max, "%.1f");
    
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1);
}
```

**Option B: Knob/dial widget**
```cpp
// Circular knob for precise values
float value = 50.0f;
ImGui::Knob("FOV", &value, 0.0f, 100.0f, ImVec2(60, 60));
```
- [imgui-knobs](https://github.com/altschuler/imgui-knobs) — Multiple knob variants (Tick, Dot, Wiper, WiperDot, Stepped)

**Option C: Color-coded slider**
```cpp
// Slider with color gradient background
ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(0.0f, 0.8f, 0.9f).Value);
ImGui::SliderFloat("##speed", &speed, 0.0f, 100.0f);
ImGui::PopStyleColor(2);
```

---

## 4. Color Picker — Built-in & Custom

### Current State
NovaX has `ImGui::ColorEdit4` for color settings — this is good but could be enhanced.

### Improvements

**A. Add alpha bar**
```cpp
ImGui::ColorEdit4("##color", color,
    ImGuiColorEditFlags_AlphaBar |        // show alpha bar
    ImGuiColorEditFlags_NoInputs |        // hide RGB sliders
    ImGuiColorEditFlags_NoLabel |         // hide label
    ImGuiColorEditFlags_PickerHueBar);    // hue bar mode
```

**B. Inline color picker with preview**
```cpp
static float color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
ImGui::ColorEdit4("##col", color,
    ImGuiColorEditFlags_AlphaBar |
    ImGuiColorEditFlags_AlphaPreview |
    ImGuiColorEditFlags_NoSidePreview);
```

**C. Compact color editor**
```cpp
ImGui::PushItemWidth(120);
ImGui::ColorEdit4("##c", color,
    ImGuiColorEditFlags_NoInputs |
    ImGuiColorEditFlags_NoLabel);
ImGui::SameLine();
ImGui::Text("Color Name");
```

---

## 5. Layout & Navigation Improvements

### Problem
Current layout uses CollapsingHeader which is not touch-friendly. TabBar categories are too small.

### Solution: Better Navigation Patterns

**A. Bottom tab bar (recommended)**
```cpp
// Bottom navigation bar - most touch-friendly
if (ImGui::BeginTabBar("##BottomNav",
    ImGuiTabBarFlags_Reorderable |
    ImGuiTabBarFlags_FittingPolicyScroll)) {
    
    if (ImGui::BeginTabItem(ICON_FA_HOME)) { /* HOME */ ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem(ICON_FA_EYE)) { /* ESP */ ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem(ICON_FA_CROSSHAIRS)) { /* AIM */ ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem(ICON_FA_MEMORY)) { /* Memory */ ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem(ICON_FA_COG)) { /* Settings */ ImGui::EndTabItem(); }
    
    ImGui::EndTabBar();
}
```

**B. Side panel navigation**
```cpp
// Left side panel (30% width)
ImGui::BeginChild("##NavPanel", ImVec2(screenWidth * 0.3f, 0), true);
if (ImGui::Selectable(ICON_FA_HOME " HOME", currentTab == 0)) currentTab = 0;
if (ImGui::Selectable(ICON_FA_EYE " ESP", currentTab == 1)) currentTab = 1;
if (ImGui::Selectable(ICON_FA_CROSSHAIRS " AIM", currentTab == 2)) currentTab = 2;
ImGui::EndChild();

ImGui::SameLine();

// Right content area (70% width)
ImGui::BeginChild("##Content", ImVec2(0, 0), true);
switch (currentTab) {
    case 0: DrawHomeTab(); break;
    case 1: DrawESPTab(); break;
    case 2: DrawAIMTab(); break;
}
ImGui::EndChild();
```

**C. Collapsible sections with icons**
```cpp
ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
if (ImGui::CollapsingHeader(ICON_FA_EYE " ESP Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
    // ESP options
}
if (ImGui::CollapsingHeader(ICON_FA_CROSSHAIRS " Aim Settings")) {
    // Aim options
}
ImGui::PopStyleVar();
```

### Touch Target Guidelines
- **Minimum touch target:** 48x48dp (Apple HIG: 44x44pt, Material: 48x48dp)
- **Spacing between targets:** Minimum 8dp
- **Hit area > visual size:** Use invisible padding to extend tap zones
- **Thumb zone:** Place most-used buttons at bottom of screen (natural thumb arc)

---

## 6. Theme & Styling — Cyberpunk/Neon Theme

### Problem
Current theme is basic dark. Needs personality and visual appeal.

### Solution: Neon/Cyberpunk Theme

**Color Palette:**
```cpp
// Cyberpunk Neon Theme
namespace CyberTheme {
    // Backgrounds
    const ImVec4 VoidBlack    = ImVec4(0.03f, 0.03f, 0.05f, 1.0f);  // #080808
    const ImVec4 DarkSurface  = ImVec4(0.06f, 0.06f, 0.08f, 1.0f);  // #101014
    const ImVec4 PanelBg      = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);  // #141418
    
    // Accents
    const ImVec4 NeonCyan     = ImVec4(0.0f, 0.96f, 1.0f, 1.0f);    // #00F5FF
    const ImVec4 NeonMagenta  = ImVec4(1.0f, 0.0f, 0.43f, 1.0f);    // #FF006E
    const ImVec4 NeonGreen    = ImVec4(0.0f, 0.93f, 0.58f, 1.0f);   // #00ED94
    const ImVec4 NeonYellow   = ImVec4(0.95f, 0.9f, 0.0f, 1.0f);    // #F3E600
    const ImVec4 NeonRed      = ImVec4(1.0f, 0.37f, 0.37f, 1.0f);   // #FF5E5E
    
    // Text
    const ImVec4 TextPrimary  = ImVec4(0.97f, 0.97f, 0.95f, 1.0f);  // #F8F8F2
    const ImVec4 TextSecondary= ImVec4(0.6f, 0.6f, 0.65f, 1.0f);    // #9999A6
}
```

**Apply Theme:**
```cpp
void ApplyCyberTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Window
    style.WindowRounding = 8.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowPadding = ImVec2(12, 12);
    
    // Frame (inputs, sliders, toggles)
    style.FrameRounding = 6.0f;
    style.FramePadding = ImVec2(10, 6);
    style.FrameBorderSize = 0.0f;
    
    // Tabs
    style.TabRounding = 6.0f;
    style.TabBorderSize = 0.0f;
    
    // Scrollbar
    style.ScrollbarSize = 16.0f;
    style.ScrollbarRounding = 8.0f;
    
    // Grab (slider knobs)
    style.GrabMinSize = 12.0f;
    style.GrabRounding = 4.0f;
    
    // Colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg]        = CyberTheme::DarkSurface;
    colors[ImGuiCol_ChildBg]         = CyberTheme::PanelBg;
    colors[ImGuiCol_PopupBg]         = CyberTheme::DarkSurface;
    colors[ImGuiCol_Border]          = ImVec4(0.2f, 0.2f, 0.25f, 0.5f);
    colors[ImGuiCol_FrameBg]         = ImVec4(0.1f, 0.1f, 0.12f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]  = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
    colors[ImGuiCol_FrameBgActive]   = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    colors[ImGuiCol_TitleBg]         = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
    colors[ImGuiCol_TitleBgActive]   = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
    colors[ImGuiCol_Tab]             = ImVec4(0.1f, 0.1f, 0.12f, 1.0f);
    colors[ImGuiCol_TabHovered]      = CyberTheme::NeonCyan;
    colors[ImGuiCol_TabActive]       = CyberTheme::NeonMagenta;
    colors[ImGuiCol_SliderGrab]      = CyberTheme::NeonCyan;
    colors[ImGuiCol_SliderGrabActive]= CyberTheme::NeonMagenta;
    colors[ImGuiCol_Button]          = ImVec4(0.12f, 0.12f, 0.15f, 1.0f);
    colors[ImGuiCol_ButtonHovered]   = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    colors[ImGuiCol_ButtonActive]    = CyberTheme::NeonCyan;
    colors[ImGuiCol_Header]          = ImVec4(0.12f, 0.12f, 0.15f, 1.0f);
    colors[ImGuiCol_HeaderHovered]   = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    colors[ImGuiCol_HeaderActive]    = CyberTheme::NeonCyan;
    colors[ImGuiCol_Separator]       = ImVec4(0.15f, 0.15f, 0.18f, 0.5f);
    colors[ImGuiCol_Text]            = CyberTheme::TextPrimary;
    colors[ImGuiCol_TextDisabled]    = CyberTheme::TextSecondary;
}
```

### Glow Effects (Advanced)
```cpp
// Add neon glow to active elements
ImVec4 glowCyan = ImVec4(0.0f, 0.96f, 1.0f, 0.4f);
ImVec4 glowMagenta = ImVec4(1.0f, 0.0f, 0.43f, 0.4f);

// Draw glow behind window
draw_list->AddRectFilled(
    ImVec2(p.x - 4, p.y - 4),
    ImVec2(p.x + w + 4, p.y + h + 4),
    IM_COL32(0, 245, 255, 30),  // subtle cyan glow
    12.0f
);
```

---

## 7. Touch-Friendly Design Rules

### Minimum Sizes
| Element | Minimum | Recommended |
|---------|---------|-------------|
| Button | 44x44dp | 48x48dp |
| Toggle | 48x28dp | 60x32dp |
| Slider | 44x24dp | 60x28dp |
| Tab | 44x44dp | 60x48dp |
| Checkbox | 44x44dp | 48x48dp |

### Spacing
- Between elements: 8dp minimum
- Group spacing: 16dp
- Section padding: 12dp

### Thumb Zone Layout
```
┌─────────────────────────┐
│  Hard to reach (top)     │  ← Infrequent actions
│                          │
│                          │
│  Easy to reach (middle)  │  ← Secondary actions
│                          │
│  Natural thumb arc       │  ← PRIMARY ACTIONS
│  (bottom)                │  ← Most-used buttons
└─────────────────────────┘
```

### Feedback
- **Tap feedback:** Visual depression or color change on press
- **Toggle feedback:** Smooth animation (80-100ms)
- **Slider feedback:** Real-time value update
- **Error feedback:** Red border flash or haptic

---

## 8. Animation Guidelines

### Timing
| Animation | Duration | Easing |
|-----------|----------|--------|
| Toggle switch | 80-100ms | Ease-out |
| Button press | 50-80ms | Linear |
| Window open | 150-200ms | Ease-in-out |
| Tab switch | 100-150ms | Ease-out |
| Slider drag | Real-time | None |
| Toast/notification | 2000-3000ms | Fade in/out |

### Implementation
```cpp
// Smooth animation using ImGui context
float t = *v ? 1.0f : 0.0f;
ImGuiContext& g = *GImGui;
float ANIM_SPEED = 0.085f;
if (g.LastActiveId == g.CurrentWindow->GetID(widget_id)) {
    float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
    t = *v ? (t_anim) : (1.0f - t_anim);
}
```

---

## 9. Specific Improvements for NovaX

### A. Main Menu Redesign
```
Current:                          Improved:
┌──────────────┐                 ┌──────────────────────────┐
│ HOME (text)  │                 │ 👁 NOVA X          [Hide] │
│ ESP (text)   │                 ├──────────────────────────┤
│ AIM (text)   │                 │ [🏠HOME] [👁ESP] [🎯AIM] │
│ Memory(text) │                 │ [💾MEM] [⚙SET]          │
│ Settings(txt)│                 ├──────────────────────────┤
│              │                 │                          │
│              │                 │   (content area)         │
└──────────────┘                 └──────────────────────────┘
```

### B. ESP Tab with Icons
```cpp
if (ImGui::BeginTabBar("##ESP", ImGuiTabBarFlags_FittingPolicyScroll)) {
    if (ImGui::BeginTabItem(ICON_FA_USER " Players")) {
        ImGui::Toggle(ICON_FA_EYE " ESP", &Cloud.Players);
        ImGui::Toggle(ICON_FA_SQUARE " Box", &Cloud.Box);
        ImGui::Toggle(ICON_FA_BONE " Skeleton", &Cloud.Skeleton);
        ImGui::Toggle(ICON_FA_HEART " Health", &Cloud.Health);
        ImGui::Toggle(ICON_FA_RULER " Distance", &Cloud.Distance);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(ICON_FA_CAR " Vehicles")) {
        ImGui::Toggle(ICON_FA_EYE " Vehicle ESP", &Cloud.Vehicle);
        ImGui::SliderFloat(ICON_FA_GAS_PUMP " Fuel", &fuel, 0, 100);
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
}
```

### C. Settings Tab with Sections
```cpp
if (ImGui::CollapsingHeader(ICON_FA_PALETTE " Theme", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::ColorEdit4(ICON_FA_PALETTE " Player Color", Cloud.PlayerColor);
    ImGui::ColorEdit4(ICON_FA_PALETTE " Bot Color", Cloud.BotColor);
    ImGui::ColorEdit4(ICON_FA_PALETTE " Aim Circle", Cloud.自瞄圈颜色);
}

if (ImGui::CollapsingHeader(ICON_FA_SLIDERS_H " Performance")) {
    ImGui::SliderInt(ICON_FA_TACHOMETER " Target FPS", &Cloud.目标帧率, 60, 165);
    ImGui::SliderInt(ICON_FA_HAND_POINTER " Touch Rate", &Cloud.TouchRate, 60, 5000);
}

if (ImGui::CollapsingHeader(ICON_FA_SHIELD_ALT " Anti-Detection")) {
    ImGui::Toggle(ICON_FA_VIDEO_SLASH " Anti-Screenshot", &Cloud.防录屏);
    ImGui::Toggle(ICON_FA_EYE_SLASH " Hide Menu", &Cloud.隐藏);
}
```

### D. Weapon Picker with Images
```cpp
// Grid of weapon images with labels
ImGui::Columns(4, "##weapons", true);
for (int i = 0; i < weaponCount; i++) {
    if (weaponTextures[i]) {
        if (ImGui::ImageButton("##wep", (ImTextureID)weaponTextures[i],
            ImVec2(60, 30))) {
            Cloud.selectedWeapon = i;
        }
        ImGui::SameLine();
        ImGui::Text("%s", weaponNames[i]);
        ImGui::NextColumn();
    }
}
ImGui::Columns(1);
```

### E. Status Bar
```cpp
// Bottom status bar
ImGui::SetNextWindowPos(ImVec2(0, screenHeight - 30));
ImGui::SetNextWindowSize(ImVec2(screenWidth, 30));
ImGui::Begin("##StatusBar", nullptr,
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
ImGui::Text(ICON_FA_WIFI " FPS: %d", fpsnum);
ImGui::SameLine();
ImGui::Text("| " ICON_FA_USERS " Players: %d", playerCount);
ImGui::SameLine();
ImGui::Text("| " ICON_FA_MOTORCYCLE " Vehicles: %d", vehicleCount);
ImGui::End();
```

---

## 10. Reference Projects

| Project | What to Steal |
|---------|--------------|
| [Android-Imgui-Template](https://github.com/Shad0w-KuNgen/Android-Imgui-Template) | Font Awesome 5+7 integration, CI/CD |
| [imgui_toggle](https://github.com/cmdwtf/imgui_toggle) | iOS/Material/Glowing toggle styles |
| [imgui-animated](https://github.com/interception-team/imgui-animated) | Animated widgets (Toggle, ButtonScrollable) |
| [leanUI](https://github.com/Geolm/leanUI) | Minimal animated UI (600 lines, smooth) |
| [CPStyling](https://github.com/Nats-ji/CPStyling.lua) | Cyberpunk theme colors + custom widgets |
| [RequestFX/ImGUI-Advanced-Cheat-Menu](https://github.com/RequestFX/ImGUI-Advanced-Cheat-Menu) | Hotkey buttons, console, font from memory |
| [ImHex color picker](https://github.com/WerWolv/ImHex) | Advanced color picker with drag-drop |

---

## 11. Implementation Priority

| Priority | Change | Effort | Impact |
|----------|--------|--------|--------|
| 1 | Add Font Awesome icons to tabs/buttons | Low | High — instant visual upgrade |
| 2 | Increase touch target sizes (48dp minimum) | Low | High — usability |
| 3 | Modern animated toggle (80-100ms) | Low | Medium — polish |
| 4 | Cyberpunk/neon color theme | Low | High — visual identity |
| 5 | Bottom tab bar navigation | Medium | High — better UX |
| 6 | Glow effects on active elements | Medium | Medium — visual flair |
| 7 | Knob/dial widgets for precise values | Medium | Medium — feature |
| 8 | Weapon image grid picker | Medium | Medium — convenience |
| 9 | Status bar with FPS/player count | Low | Medium — info |
| 10 | Window slide-in animations | Low | Low — polish |

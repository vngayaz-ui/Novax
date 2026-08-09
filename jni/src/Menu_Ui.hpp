// ============================================================
// NovaX  —  Menu_Ui.hpp
// Sidebar Layout | Card Sections | Neon Dark
// All 6 bugs fixed — see FIXES section below
// ============================================================
#include <Function.hpp>
#include <regex>
#include <sstream>

// ── FIXES applied in this version ────────────────────────────
// FIX-1: goto removed → proper bool flag used instead
// FIX-2: BeginCard/EndCard rewritten — body drawn BEFORE widgets
//         using ImDrawListSplitter (foreground/background split)
// FIX-3: FPS = *threadtime directly (not 1000/x)
//         Dead CardBG() and CardHdr() functions removed
// FIX-4: ITEMS tab each sub-list wrapped in BeginChild scroll
//         WeaponSettings slider IDs use PushID(idx) context
// FIX-5: Color pickers added to ESP tab
// FIX-6: Active sidebar nav button has pulsing neon glow
// ─────────────────────────────────────────────────────────────

bool  kernel_active    = true;
inline int   chargingPortIndex = 0;
inline float ChargingPortSide  = 0.0f;

enum NXTab {
    TAB_HOME=0, TAB_ESP, TAB_ITEMS,
    TAB_AIM, TAB_MEMORY, TAB_SETTINGS, TAB_COUNT
};
static int g_Tab = TAB_HOME;

// ── Sizing — computed at runtime from screen resolution ───────
static constexpr float ROW_H    = 52.0f;
static constexpr float SLD_H    = 62.0f;
static constexpr float HDR_H    = 44.0f;
static constexpr float GAP      =  8.0f;
static constexpr float CARD_PAD = 12.0f;
static constexpr float NAV_GAP  =  4.0f;

// Dynamic values — updated every frame from screen size
static float g_SB_W  = 240.0f;
static float g_NAV_H =  72.0f;

// ── Palette ───────────────────────────────────────────────────
#define NX_CYAN        IM_COL32(  0,200,230,255)
#define NX_CYAN_DIM    IM_COL32(  0,140,165,130)
#define NX_MAGENTA     IM_COL32(205,  0,165,255)
#define NX_GREEN       IM_COL32(  0,215,120,255)
#define NX_RED         IM_COL32(225, 45, 45,255)
#define NX_YELLOW      IM_COL32(235,195, 25,255)
#define NX_BG0         IM_COL32(  8,  8, 13,255)
#define NX_BG1         IM_COL32( 13, 13, 20,255)
#define NX_BG2         IM_COL32( 18, 18, 28,255)
#define NX_BG3         IM_COL32( 25, 25, 38,255)
#define NX_BORDER      IM_COL32( 42, 42, 65,190)
#define NX_TEXT        IM_COL32(225,230,242,255)
#define NX_TEXT_DIM    IM_COL32(105,110,125,255)

// ─────────────────────────────────────────────────────────────
// Sidebar font sizes — explicit, not dependent on default font
// ─────────────────────────────────────────────────────────────
static constexpr float SB_FONT_NAV  = 26.0f;  // nav button label
static constexpr float SB_FONT_LOGO = 30.0f;  // "NOVA X" title
static constexpr float SB_FONT_SUB  = 20.0f;  // "Professional" subtitle
static constexpr float SB_FONT_STAT = 19.0f;  // FPS/P/B status

// ─────────────────────────────────────────────────────────────
// FIX-6: SIDEBAR NAV BUTTON — pulsing glow on active
// ─────────────────────────────────────────────────────────────
static bool SideBtn(ImDrawList* dl, ImVec2 p, float w,
                    const char* label, bool active)
{
    ImGui::SetCursorScreenPos(p);
    char bid[64]; snprintf(bid, sizeof(bid), "##nb_%s", label);
    ImGui::InvisibleButton(bid, ImVec2(w, g_NAV_H));
    bool clicked = ImGui::IsItemClicked();
    bool hovered = ImGui::IsItemHovered();

    ImU32 bg = active  ? NX_BG3 :
               hovered ? IM_COL32(22,22,34,255) : NX_BG1;
    dl->AddRectFilled(p, ImVec2(p.x+w, p.y+g_NAV_H), bg, 7.0f);

    if (active) {
        dl->AddRectFilled(p, ImVec2(p.x+4, p.y+g_NAV_H),
                          NX_CYAN, 2.0f, ImDrawFlags_RoundCornersLeft);
        float pulse = (sinf((float)ImGui::GetTime() * 3.0f) + 1.0f) * 0.5f;
        ImU32 glow  = IM_COL32(0, 200, 230, (int)(20 + pulse * 35));
        dl->AddRect(ImVec2(p.x+1,p.y+1),
                    ImVec2(p.x+w-1,p.y+g_NAV_H-1),
                    glow, 7.0f, 0, 1.5f);
    }

    ImU32 col = active ? NX_CYAN : (hovered ? NX_TEXT : NX_TEXT_DIM);
    ImVec2 ts = ImGui::GetFont()->CalcTextSizeA(SB_FONT_NAV, FLT_MAX, 0.0f, label);
    dl->AddText(ImGui::GetFont(), SB_FONT_NAV,
                ImVec2(p.x+18, p.y+(g_NAV_H-ts.y)*0.5f), col, label);
    return clicked;
}

// ─────────────────────────────────────────────────────────────
// TOGGLE  —  ROW_H tall, 64×32 switch, smooth animation
// ─────────────────────────────────────────────────────────────
inline bool ToggleButton(const char* label, bool* v, const char* id)
{
    const float SW = 64.0f, SH = 32.0f;
    ImDrawList* dl  = ImGui::GetWindowDrawList();
    ImVec2 row_p    = ImGui::GetCursorScreenPos();
    float  avail    = ImGui::GetContentRegionAvail().x;

    // Switch position — right edge, vertically centred
    float  sx = row_p.x + avail - SW - 4;
    float  sy = row_p.y + (ROW_H - SH) * 0.5f;
    ImVec2 sp(sx, sy);

    // ── Left dead zone: label area — consumes input but does nothing ──
    // Without this, adjacent items extend into this gap and misfire.
    float label_w = sx - 4 - row_p.x;
    if (label_w > 0.0f) {
        ImGui::SetCursorScreenPos(row_p);
        char dead_id[128];
        snprintf(dead_id, sizeof(dead_id), "##dead_%s", id);
        ImGui::InvisibleButton(dead_id, ImVec2(label_w, ROW_H));
        // intentionally no action on click
    }

    // ── Draw label (pure draw over the dead zone) ─────────────
    ImVec2 ts = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(row_p.x + 4,
                       row_p.y + (ROW_H - ts.y) * 0.5f),
                NX_TEXT, label);

    // ── Right zone: the actual toggle switch ──────────────────
    ImGui::SetCursorScreenPos(ImVec2(sx - 4, row_p.y));
    ImGui::InvisibleButton(id, ImVec2(SW + 8, ROW_H));
    bool changed = false;
    if (ImGui::IsItemClicked()) { *v = !*v; changed = true; }

    // ── Smooth animation ──────────────────────────────────────
    float t = *v ? 1.0f : 0.0f;
    ImGuiContext& g = *GImGui;
    if (g.LastActiveId == g.CurrentWindow->GetID(id)) {
        float ta = ImSaturate(g.LastActiveIdTimer / 0.10f);
        t = *v ? ta : (1.0f - ta);
    }

    // Advance cursor to end of full row
    ImGui::SetCursorScreenPos(ImVec2(row_p.x, row_p.y + ROW_H));

    // Track
    ImU32 track = ImGui::GetColorU32(ImVec4(
        0.22f + (0.00f-0.22f)*t,
        0.22f + (0.76f-0.22f)*t,
        0.27f + (0.88f-0.27f)*t, 1.0f));
    dl->AddRectFilled(sp, ImVec2(sp.x+SW,sp.y+SH), track, SH*0.5f);
    dl->AddRect(sp, ImVec2(sp.x+SW,sp.y+SH),
                IM_COL32(255,255,255,(int)(18+t*30)), SH*0.5f, 0, 1.0f);
    if (t > 0.35f)
        dl->AddRect(ImVec2(sp.x-1,sp.y-1),
                    ImVec2(sp.x+SW+1,sp.y+SH+1),
                    IM_COL32(0,200,230,(int)(t*38)),
                    SH*0.5f+1, 0, 1.5f);
    // Knob
    float kx = sp.x + SH*0.5f + t*(SW-SH);
    dl->AddCircleFilled(ImVec2(kx,sp.y+SH*0.5f), SH*0.40f-1.0f,
                        IM_COL32(255,255,255,255));
    dl->AddCircleFilled(ImVec2(kx-1.5f,sp.y+SH*0.5f-2.0f),
                        SH*0.13f, IM_COL32(255,255,255,65));

    // Row separator
    dl->AddLine(ImVec2(row_p.x, row_p.y+ROW_H-1),
                ImVec2(row_p.x+avail, row_p.y+ROW_H-1),
                IM_COL32(40,40,60,110), 1.0f);

    ImGui::SetCursorScreenPos(ImVec2(row_p.x, row_p.y+ROW_H));
    return changed;
}

// ─────────────────────────────────────────────────────────────
// SLIDER  —  SLD_H tall, custom track, unique ID per label
// ─────────────────────────────────────────────────────────────
inline bool HorizontalSlider(const char* label, float* v,
                              float v_min, float v_max)
{
    const float TH = 6.0f, KR = 11.0f;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float avail    = ImGui::GetContentRegionAvail().x;
    ImVec2 rp      = ImGui::GetCursorScreenPos();
    float  half    = SLD_H * 0.5f;

    // Label (top half)
    ImVec2 lts = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(rp.x+4, rp.y+(half-lts.y)*0.5f), NX_TEXT, label);

    // Value badge (right of label row)
    char vb[32]; snprintf(vb, sizeof(vb), "%.2f", *v);
    ImVec2 vts = ImGui::CalcTextSize(vb);
    dl->AddText(ImVec2(rp.x+avail-vts.x-4,
                       rp.y+(half-vts.y)*0.5f), NX_MAGENTA, vb);

    // FIX-4: label-based unique ID works inside PushID context too
    char sid[128]; snprintf(sid, sizeof(sid), "##sl_%s", label);

    // Invisible slider on bottom half
    ImGui::SetCursorScreenPos(ImVec2(rp.x, rp.y+half));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,          ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,   ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,    ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,        ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,  ImVec4(0,0,0,0));
    ImGui::SetNextItemWidth(avail);
    bool changed = ImGui::SliderFloat(sid, v, v_min, v_max, "");
    ImGui::PopStyleColor(5);

    // Custom drawn track
    float ty    = rp.y + half + (half-TH)*0.5f;
    float tw    = avail - 8;
    float frac  = (v_max>v_min) ? (*v-v_min)/(v_max-v_min) : 0.f;
    frac        = frac<0.f?0.f:(frac>1.f?1.f:frac);
    float fill_x= rp.x + 4 + frac*tw;

    dl->AddRectFilled(ImVec2(rp.x+4, ty-TH*0.5f),
                      ImVec2(rp.x+4+tw, ty+TH*0.5f), NX_BG3, TH*0.5f);
    if (frac>0.001f)
        dl->AddRectFilled(ImVec2(rp.x+4, ty-TH*0.5f),
                          ImVec2(fill_x,  ty+TH*0.5f), NX_MAGENTA, TH*0.5f);
    dl->AddCircleFilled(ImVec2(fill_x,ty), KR,   NX_MAGENTA);
    dl->AddCircleFilled(ImVec2(fill_x,ty), KR-3, IM_COL32(28,8,22,255));
    dl->AddCircleFilled(ImVec2(fill_x,ty), 4.0f, NX_MAGENTA);

    // Row separator
    dl->AddLine(ImVec2(rp.x,rp.y+SLD_H-1),
                ImVec2(rp.x+avail,rp.y+SLD_H-1),
                IM_COL32(40,40,60,90), 1.0f);

    ImGui::SetCursorScreenPos(ImVec2(rp.x, rp.y+SLD_H));
    return changed;
}

// ─────────────────────────────────────────────────────────────
// FIX-4: WEAPON SETTINGS — sliders inside PushID(idx) so no clash
// ─────────────────────────────────────────────────────────────
static void WeaponSettings(const char* name, WeaponAimSettings* s, int idx)
{
    if (!s) return;
    ImGui::PushID(idx);  // all slider IDs inside this scope are unique
    ImVec2 rp   = ImGui::GetCursorScreenPos();
    float avail = ImGui::GetContentRegionAvail().x;
    ImGui::GetWindowDrawList()->AddText(
        ImVec2(rp.x+4, rp.y+(ROW_H-ImGui::GetTextLineHeight())*0.5f),
        NX_CYAN, name);
    ImGui::SetCursorScreenPos(
        ImVec2(rp.x+avail-38, rp.y+(ROW_H-24)*0.5f));
    ImGui::Checkbox("##en", &s->Enabled);
    ImGui::SetCursorScreenPos(ImVec2(rp.x, rp.y+ROW_H));

    if (s->Enabled) {
        ImGui::Indent(16.0f);
        HorizontalSlider("FOV Range",     &s->FOVRange,        0.f,1000.f);
        HorizontalSlider("No Scope Distance",  &s->HipfireDistance, 0.f,500.f);
        HorizontalSlider("Scope Distance",&s->ScopeDistance,   0.f,500.f);
        HorizontalSlider("No Scope Speed",     &s->HipfireSpeed,    0.f,20.f);
        HorizontalSlider("Scope Speed",   &s->ScopeSpeed,      0.f,20.f);
        HorizontalSlider("No Scope Recoil",    &s->HipfireRecoil,   0.f,5.f);
        HorizontalSlider("Scope Recoil",  &s->ScopeRecoil,     0.f,5.f);
        HorizontalSlider("Prediction",    &s->Prediction,      0.f,15.f);
        ImGui::Unindent(16.0f);
    }
    ImVec2 dp = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddLine(
        dp, ImVec2(dp.x+ImGui::GetContentRegionAvail().x, dp.y),
        NX_BORDER, 1.0f);
    ImGui::Dummy(ImVec2(0,4));
    ImGui::PopID();
}

// ─────────────────────────────────────────────────────────────
// FIX-2: CARD SYSTEM — ImDrawListSplitter so body is drawn
//         BEHIND widgets, not over them
//
// Usage:
//   BeginCard(title, accent);
//   ... widgets ...
//   EndCard(accent);
// ─────────────────────────────────────────────────────────────
struct NXCard {
    ImVec2             origin;
    float              width;
    ImDrawListSplitter splitter;
};
static NXCard s_card;

static void BeginCard(const char* title, ImU32 accent = NX_CYAN)
{
    s_card.origin = ImGui::GetCursorScreenPos();
    s_card.width  = ImGui::GetContentRegionAvail().x;
    ImDrawList* dl = ImGui::GetWindowDrawList();

    // FIX-2: Split into channel 0=background, channel 1=foreground
    s_card.splitter.Split(dl, 2);

    // ── Channel 1: header (always on top) ──
    s_card.splitter.SetCurrentChannel(dl, 1);
    float w = s_card.width;
    ImVec2 p = s_card.origin;
    dl->AddRectFilled(p, ImVec2(p.x+w, p.y+HDR_H),
                      NX_BG3, 9.0f, ImDrawFlags_RoundCornersTop);
    dl->AddLine(ImVec2(p.x,p.y+HDR_H),
                ImVec2(p.x+w,p.y+HDR_H), accent, 1.5f);
    dl->AddCircleFilled(ImVec2(p.x+18, p.y+HDR_H*0.5f), 5.0f, accent);
    ImVec2 ts = ImGui::CalcTextSize(title);
    dl->AddText(ImVec2(p.x+32, p.y+(HDR_H-ts.y)*0.5f), NX_TEXT, title);

    // ── Channel 1 stays active for widgets ──
    // (background drawn in EndCard on channel 0)

    // Advance cursor past header, apply inner padding
    ImGui::Dummy(ImVec2(s_card.width, HDR_H));
    ImGui::SetCursorScreenPos(
        ImVec2(s_card.origin.x + CARD_PAD,
               s_card.origin.y + HDR_H + CARD_PAD * 0.5f));
}

static void EndCard(ImU32 accent = NX_CYAN)
{
    // Bottom padding
    ImGui::SetCursorScreenPos(
        ImVec2(s_card.origin.x,
               ImGui::GetCursorScreenPos().y + CARD_PAD));

    float totalH = ImGui::GetCursorScreenPos().y - s_card.origin.y;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p = s_card.origin;
    float  w = s_card.width;
    float  h = totalH;

    // ── Channel 0: card body BEHIND widgets ──
    s_card.splitter.SetCurrentChannel(dl, 0);
    // Shadow
    dl->AddRectFilled(ImVec2(p.x+3,p.y+4),
                      ImVec2(p.x+w+3,p.y+h+4),
                      IM_COL32(0,0,0,55), 9.0f);
    // Body fill
    dl->AddRectFilled(p, ImVec2(p.x+w,p.y+h), NX_BG2, 9.0f);
    // Border
    dl->AddRect(p, ImVec2(p.x+w,p.y+h), NX_BORDER, 9.0f, 0, 1.0f);
    // Left accent stripe
    dl->AddRectFilled(p, ImVec2(p.x+4,p.y+h),
                      accent, 2.0f, ImDrawFlags_RoundCornersLeft);

    // Merge channels — background draws first, then foreground
    s_card.splitter.Merge(dl);

    ImGui::Dummy(ImVec2(0, GAP));
}

// ─────────────────────────────────────────────────────────────
// FIX-5: COMPACT COLOR PICKER ROW
// ─────────────────────────────────────────────────────────────
static void ColorRow(const char* label, float* col4)
{
    ImVec2 rp   = ImGui::GetCursorScreenPos();
    float avail = ImGui::GetContentRegionAvail().x;
    ImVec2 ts   = ImGui::CalcTextSize(label);
    ImGui::GetWindowDrawList()->AddText(
        ImVec2(rp.x+4, rp.y+(ROW_H-ts.y)*0.5f), NX_TEXT, label);

    ImGui::SetCursorScreenPos(ImVec2(rp.x+avail-160, rp.y+(ROW_H-28)*0.5f));
    ImGui::SetNextItemWidth(155.0f);
    char cid[64]; snprintf(cid, sizeof(cid), "##col_%s", label);
    ImGui::ColorEdit4(cid, col4,
        ImGuiColorEditFlags_NoInputs   |
        ImGuiColorEditFlags_NoLabel    |
        ImGuiColorEditFlags_AlphaBar   |
        ImGuiColorEditFlags_PickerHueBar);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddLine(ImVec2(rp.x,rp.y+ROW_H-1),
                ImVec2(rp.x+avail,rp.y+ROW_H-1),
                IM_COL32(40,40,60,110), 1.0f);
    ImGui::SetCursorScreenPos(ImVec2(rp.x, rp.y+ROW_H));
}

// ─────────────────────────────────────────────────────────────
// 主窗口  —  MAIN RENDER
// ─────────────────────────────────────────────────────────────
void 窗口函数()
{
    float targetW = (屏幕x > 0) ? 屏幕x * 0.88f : 1100.0f;
    float targetH = (屏幕y > 0) ? 屏幕y * 0.82f :  820.0f;

    // Scale sidebar and nav from screen width — works on 720p, 1080p, etc.
    // Base: 1080p wide screen → SB_W=240, NAV_H=72
    // On 720p landscape (~1280wide) these scale proportionally
    float scr_w = (屏幕x > 0) ? (float)屏幕x : 1280.0f;
    g_SB_W  = scr_w * 0.200f;   // 20% of screen width for sidebar
    g_SB_W  = g_SB_W  < 180.0f ? 180.0f : (g_SB_W  > 300.0f ? 300.0f : g_SB_W);
    g_NAV_H = g_SB_W  * 0.300f; // nav height proportional to sidebar width
    g_NAV_H = g_NAV_H < 56.0f  ? 56.0f  : (g_NAV_H > 88.0f  ? 88.0f  : g_NAV_H);

    float fps_now = ImGui::GetIO().Framerate;
    float step    = (fps_now > 0.5f) ? (1800.0f/fps_now) : 30.0f;

    if (Window) {
        MainWindow.x = (MainWindow.x < targetW) ? MainWindow.x+step : targetW;
        MainWindow.y = (MainWindow.y < targetH) ? MainWindow.y+step : targetH;
    } else {
        MainWindow.x = (MainWindow.x > 0) ? MainWindow.x-step : 0.0f;
        MainWindow.y = (MainWindow.y > 0) ? MainWindow.y-step : 0.0f;
    }

    // FIX-1: no goto — use bool to decide which windows to draw
    bool show_main = (MainWindow.x > 2 && MainWindow.y > 2);

    if (show_main)
    {
        if (MainMenuSwitch) {
            ImGui::SetNextWindowPos(SmallMenuPosition, ImGuiCond_Always);
            MainMenuSwitch = false;
        }
        ImGui::SetNextWindowSize(MainWindow);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg,
            ImVec4(0.032f,0.032f,0.050f,0.97f));

        bool open = true;
        ImGui::Begin("##NX", &open,
            ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        if (!open) Window = false;

        主菜单位置     = ImGui::GetWindowPos();
        ImDrawList* dl  = ImGui::GetWindowDrawList();
        ImVec2 WP       = ImGui::GetWindowPos();
        float  W        = MainWindow.x;
        float  H        = MainWindow.y;

        // ════════ SIDEBAR ════════════════════════════════════
        dl->AddRectFilled(WP, ImVec2(WP.x+g_SB_W,WP.y+H), NX_BG1);
        dl->AddLine(ImVec2(WP.x+g_SB_W,WP.y),
                    ImVec2(WP.x+g_SB_W,WP.y+H), NX_BORDER, 1.2f);

        // Logo strip — taller to fit 30px + 28px text comfortably
        float logo_h = 76.0f;
        dl->AddRectFilled(WP, ImVec2(WP.x+g_SB_W,WP.y+logo_h), NX_BG0);
        dl->AddLine(ImVec2(WP.x,WP.y+logo_h),
                    ImVec2(WP.x+g_SB_W,WP.y+logo_h), NX_CYAN_DIM, 1.0f);
        float lcy = WP.y + logo_h*0.45f;
        dl->AddCircleFilled(ImVec2(WP.x+24,lcy), 9.0f, NX_CYAN);
        dl->AddCircleFilled(ImVec2(WP.x+24,lcy), 4.5f, NX_BG0);
        dl->AddText(ImGui::GetFont(), SB_FONT_LOGO,
                    ImVec2(WP.x+42,WP.y+12), NX_CYAN,    "NOVA X");
        dl->AddText(ImGui::GetFont(), SB_FONT_SUB,
                    ImVec2(WP.x+42,WP.y+42), NX_TEXT_DIM,"Professional");

        // FIX-3 + FPS SMOOTHING: AotuFPS() returns ms-per-frame, convert to FPS
        {
            static float fps_smooth = 60.0f;
            // threadtime stores ms-per-frame from AotuFPS() — divide to get FPS
            float fps_raw;
            if (定义.threadtime && *定义.threadtime > 0.0f)
                fps_raw = 1000.0f / *定义.threadtime;  // ms → FPS
            else
                fps_raw = fps_now;
            // EMA alpha=0.05: updates slowly → no blinking
            fps_smooth = fps_smooth * 0.95f + fps_raw * 0.05f;

            float cy2 = WP.y+logo_h+10;
            char buf[48];
            dl->AddRectFilled(ImVec2(WP.x+6,cy2),
                              ImVec2(WP.x+g_SB_W-6,cy2+30),
                              NX_BG3, 5.0f);
            snprintf(buf,sizeof(buf),"FPS %.0f   P %d   B %d",
                     fps_smooth, 定义.真人数量, 定义.人机数量);
            // Use CalcTextSizeA for explicit font size centering
            ImVec2 st = ImGui::GetFont()->CalcTextSizeA(SB_FONT_STAT, FLT_MAX, 0.0f, buf);
            dl->AddText(ImGui::GetFont(), SB_FONT_STAT,
                        ImVec2(WP.x+(g_SB_W-st.x)*0.5f, cy2+5),
                        NX_GREEN, buf);
        }

        // Nav buttons
        {
            const char* tabs[TAB_COUNT]={
                "HOME","ESP","ITEMS","AIM","MEMORY","SETTINGS"};
            // logo_h=70 + status chip=30 + gap=14 = 114
            float ny = WP.y + logo_h + 44;
            for (int i=0;i<TAB_COUNT;i++) {
                if (SideBtn(dl,ImVec2(WP.x+5,ny),g_SB_W-10,tabs[i],g_Tab==i))
                    g_Tab = i;
                ny += g_NAV_H+NAV_GAP;
            }
            if (SideBtn(dl,ImVec2(WP.x+5,WP.y+H-g_NAV_H-8),
                        g_SB_W-10,"HIDE MENU",false)) {
                Window = false;
                SmallMenuToggle = true;
            }
        }

        // ════════ CONTENT AREA — scrollable ══════════════════
        float cx = WP.x+g_SB_W+12;
        float cw = W-g_SB_W-24;
        float ch = H-16;

        ImGui::SetCursorScreenPos(ImVec2(cx, WP.y+8));
        ImGui::PushStyleColor(ImGuiCol_ChildBg,
            ImVec4(0.032f,0.032f,0.050f,0.0f));
        ImGui::PushStyleColor(ImGuiCol_ScrollbarBg,
            ImVec4(0.04f,0.04f,0.07f,1.0f));
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab,
            ImVec4(0.0f,0.55f,0.65f,0.70f));
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered,
            ImVec4(0.0f,0.70f,0.82f,0.85f));
        ImGui::BeginChild("##content", ImVec2(cw,ch), false,
            ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::PopStyleColor(4);

        ImGui::SetCursorPosX(CARD_PAD);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY()+CARD_PAD*0.5f);
        float pw = cw - ImGui::GetStyle().ScrollbarSize - CARD_PAD*2 - 8;

        // ─── TAB: HOME ───────────────────────────────────────
        if (g_Tab == TAB_HOME) {
            BeginCard("HOME", NX_CYAN);
            ImGui::PushStyleColor(ImGuiCol_Button,
                kernel_active ? ImVec4(0.60f,0.04f,0.04f,1.0f)
                              : ImVec4(0.04f,0.38f,0.17f,1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                kernel_active ? ImVec4(0.82f,0.08f,0.08f,1.0f)
                              : ImVec4(0.06f,0.55f,0.24f,1.0f));
            if (ImGui::Button(
                    kernel_active ? "  STOP KERNEL" : "  START KERNEL",
                    ImVec2(pw, 54))) {
                kernel_active = !kernel_active;
                if (!kernel_active) {
                    定义.数组数量=0; 定义.真人数量=0;
                    定义.人机数量=0; 定义.自瞄人数=0;
                }
            }
            ImGui::PopStyleColor(2);
            EndCard(NX_CYAN);
        }

        // ─── TAB: ESP ────────────────────────────────────────
        if (g_Tab == TAB_ESP) {
            BeginCard("ESP  —  Enemy Visibility", NX_CYAN);
            ToggleButton("Player Line",     &Cloud.Line,         "##espLine");
            ToggleButton("Player Box",      &Cloud.Box,          "##espBox");
            ToggleButton("Skeleton",        &Cloud.Skeleton,     "##espSkel");
            ToggleButton("Player Name",     &Cloud.Name,         "##espName");
            ToggleButton("Team ID",         &Cloud.TeamID,       "##espTeam");
            ToggleButton("Distance",        &Cloud.Distance,     "##espDist");
            ToggleButton("Weapon Icon",     &Cloud.Weapon,       "##espWeap");
            ToggleButton("Health Bar",      &Cloud.Health,       "##espHP");
            ToggleButton("Alert 360",       &Cloud.Alert,        "##espAlert");
            ToggleButton("Landing Status",  &Cloud.EnemyLanding, "##espLand");
            ToggleButton("Grenade Warning", &Cloud.Grenade,      "##espGren");
            ToggleButton("Vehicle ESP",     &Cloud.Vehicle,      "##espVeh");
            ToggleButton("Timer Revive",    &Cloud.TimerRevive,  "##espRev");
            ToggleButton("Hide Bots",       &Cloud.HideBots,     "##espHideB");
            ToggleButton("Decryption",      &Cloud.Decryption,   "##espDecr");
            EndCard(NX_CYAN);

            // FIX-5: Color pickers for ESP colors
            BeginCard("ESP Colors", NX_CYAN);
            ColorRow("Player Line",   (float*)&Cloud.PlayerLineColor.Value);
            ColorRow("Bot Line",      (float*)&Cloud.BotLineColor.Value);
            ColorRow("Player Box",    (float*)&Cloud.PlayerBoxColor.Value);
            ColorRow("Bot Box",       (float*)&Cloud.BotBoxColor.Value);
            ColorRow("Player Health", (float*)&Cloud.PlayerHealthColor.Value);
            ColorRow("Bot Health",    (float*)&Cloud.BotHealthColor.Value);
            ColorRow("Team ID",       (float*)&Cloud.TeamIDColor.Value);
            ColorRow("Vehicle",       (float*)&Cloud.载具颜色.Value);
            EndCard(NX_CYAN);
        }

        // ─── TAB: ITEMS ──────────────────────────────────────
        if (g_Tab == TAB_ITEMS) {
            // Manual card header — no BeginCard/EndCard here because
            // ImDrawListSplitter + nested BeginChild can conflict.
            // Instead draw the header strip manually then tab bar directly.
            ImVec2 ihp = ImGui::GetCursorScreenPos();
            float  ihw = pw;
            ImDrawList* idl = ImGui::GetWindowDrawList();
            // Shadow
            idl->AddRectFilled(ImVec2(ihp.x+3,ihp.y+4),
                               ImVec2(ihp.x+ihw+3,ihp.y+HDR_H+4),
                               IM_COL32(0,0,0,50), 9.0f);
            // Header strip
            idl->AddRectFilled(ihp, ImVec2(ihp.x+ihw,ihp.y+HDR_H),
                               NX_BG3, 9.0f);
            idl->AddLine(ImVec2(ihp.x,ihp.y+HDR_H),
                         ImVec2(ihp.x+ihw,ihp.y+HDR_H), NX_YELLOW, 1.5f);
            idl->AddCircleFilled(ImVec2(ihp.x+18,ihp.y+HDR_H*0.5f),
                                 5.0f, NX_YELLOW);
            idl->AddRectFilled(ihp, ImVec2(ihp.x+4,ihp.y+HDR_H),
                               NX_YELLOW, 2.0f, ImDrawFlags_RoundCornersLeft);
            ImVec2 its = ImGui::CalcTextSize("ITEMS  \xe2\x80\x94  Loot Filter");
            idl->AddText(ImVec2(ihp.x+32,ihp.y+(HDR_H-its.y)*0.5f),
                         NX_TEXT, "ITEMS  \xe2\x80\x94  Loot Filter");
            // Advance cursor past header
            ImGui::Dummy(ImVec2(ihw, HDR_H));
            ImGui::SetCursorPosX(CARD_PAD);
            // FIX-4: each tab item wrapped in BeginChild for scroll
            ImGui::PushStyleColor(ImGuiCol_Tab,
                ImVec4(0.08f,0.08f,0.12f,1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabActive,
                ImVec4(0.0f,0.50f,0.60f,0.75f));
            ImGui::PushStyleColor(ImGuiCol_TabHovered,
                ImVec4(0.0f,0.36f,0.44f,0.45f));
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 6.0f);

            if (ImGui::BeginTabBar("##it",
                    ImGuiTabBarFlags_FittingPolicyScroll)) {

                if (ImGui::BeginTabItem("Weapons")) {
                    ImGui::BeginChild("##wl",ImVec2(0,0),false);
                    ToggleButton("M416",&Cloud.M416,"##M416");
                    ToggleButton("M16A4",&Cloud.M16A4,"##M16A4");
                    ToggleButton("SCAR-L",&Cloud.SCARL,"##SCARL");
                    ToggleButton("AKM",&Cloud.AKM,"##AKM");
                    ToggleButton("AUG",&Cloud.AUG,"##AUG");
                    ToggleButton("QBZ",&Cloud.QBZ,"##QBZ");
                    ToggleButton("G36C",&Cloud.G36C,"##G36C");
                    ToggleButton("M762",&Cloud.M762,"##M762");
                    ToggleButton("MK47",&Cloud.MK47,"##MK47");
                    ToggleButton("ACE32",&Cloud.ACE32,"##ACE32");
                    ToggleButton("UMP45",&Cloud.UMP,"##UMP45");
                    ToggleButton("Vector",&Cloud.Vector,"##VecI");
                    ToggleButton("UZI",&Cloud.UZI,"##UZI");
                    ToggleButton("Tommy Gun",&Cloud.TommyGun,"##TG");
                    ToggleButton("PP-19 Bizon",&Cloud.Bison,"##Bison");
                    ToggleButton("MP5K",&Cloud.MP5K,"##MP5K");
                    ToggleButton("P90",&Cloud.P90,"##P90");
                    ToggleButton("JS9",&Cloud.JS9,"##JS9");
                    ToggleButton("S12K",&Cloud.S12K,"##S12Ki");
                    ToggleButton("S1897",&Cloud.S1897,"##S1897i");
                    ToggleButton("S686",&Cloud.S686,"##S686i");
                    ToggleButton("DBS",&Cloud.DBS,"##DBSi");
                    ToggleButton("NS2000",&Cloud.NS2000,"##NS2000");
                    ToggleButton("Kar98K",&Cloud.Kar98K,"##K98");
                    ToggleButton("M24",&Cloud.M24,"##M24i");
                    ToggleButton("Win94",&Cloud.Win94,"##Win94");
                    ToggleButton("Mosin",&Cloud.Mosin,"##Mosin");
                    ToggleButton("Mini14",&Cloud.Mini14,"##Min14");
                    ToggleButton("SKS",&Cloud.SKS,"##SKSi");
                    ToggleButton("SLR",&Cloud.SLR,"##SLRi");
                    ToggleButton("QBU",&Cloud.QBU,"##QBU");
                    ToggleButton("Mk12",&Cloud.Mk12,"##Mk12");
                    ToggleButton("VSS",&Cloud.VSS,"##VSS");
                    ToggleButton("DP-28",&Cloud.DP28,"##DP28i");
                    ToggleButton("M249",&Cloud.M249,"##M249i");
                    ToggleButton("Pan",&Cloud.Pan,"##Pan");
                    ToggleButton("Flare Gun",&Cloud.Flare,"##Flare");
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Equipment")) {
                    ImGui::BeginChild("##eql",ImVec2(0,0),false);
                    ToggleButton("Backpack Lv.1",&Cloud.Backpack1,"##BP1");
                    ToggleButton("Backpack Lv.2",&Cloud.Backpack2,"##BP2");
                    ToggleButton("Backpack Lv.3",&Cloud.Backpack3,"##BP3");
                    ToggleButton("Helmet Lv.1",&Cloud.MotorcycleHelmet1,"##H1");
                    ToggleButton("Helmet Lv.2",&Cloud.MilitaryHelmet2,"##H2");
                    ToggleButton("Helmet Lv.3",&Cloud.SpetsnazHelmet3,"##H3");
                    ToggleButton("Vest Lv.1",&Cloud.PoliceVest1,"##V1");
                    ToggleButton("Vest Lv.2",&Cloud.PoliceVest2,"##V2");
                    ToggleButton("Vest Lv.3",&Cloud.MilitaryVest3,"##V3");
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Consumables")) {
                    ImGui::BeginChild("##cnl",ImVec2(0,0),false);
                    ToggleButton("Energy Drink",&Cloud.EnergyDrink,"##ED");
                    ToggleButton("Adrenaline",&Cloud.AdrenalineSyringe,"##AS");
                    ToggleButton("Painkillers",&Cloud.Painkillers,"##PK");
                    ToggleButton("Bandage",&Cloud.Bandage,"##BND");
                    ToggleButton("First Aid Kit",&Cloud.FirstAidKit,"##FAK");
                    ToggleButton("Med Kit",&Cloud.MedKit,"##MKi");
                    ToggleButton("Self AED",&Cloud.SelfAED,"##AED");
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Throwables")) {
                    ImGui::BeginChild("##thl",ImVec2(0,0),false);
                    ToggleButton("Frag Grenade",&Cloud.FragGrenade,"##FG");
                    ToggleButton("Smoke",&Cloud.SmokeGrenade,"##SG");
                    ToggleButton("Stun",&Cloud.StunGrenade,"##STG");
                    ToggleButton("Molotov",&Cloud.MolotovCocktail,"##MC");
                    ToggleButton("Sticky Bomb",&Cloud.StickyBomb,"##SB");
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Ammo")) {
                    ImGui::BeginChild("##aml",ImVec2(0,0),false);
                    ToggleButton("9mm",&Cloud.MM9,"##9mm");
                    ToggleButton("7.62mm",&Cloud.MM762,"##762");
                    ToggleButton("5.56mm",&Cloud.MM556,"##556");
                    ToggleButton("45 ACP",&Cloud.ACP45,"##45");
                    ToggleButton("12 Gauge",&Cloud.Gauge12,"##12g");
                    ToggleButton("300 Magnum",&Cloud.Magnum300,"##300");
                    ToggleButton("Bolt",&Cloud.Bolt,"##Bolt");
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Attachments")) {
                    ImGui::BeginChild("##atl",ImVec2(0,0),false);
                    ToggleButton("Scope x3",&Cloud.ScopeX3,"##s3");
                    ToggleButton("Scope x4",&Cloud.ScopeX4,"##s4");
                    ToggleButton("Scope x6",&Cloud.ScopeX6,"##s6");
                    ToggleButton("Scope x8",&Cloud.ScopeX8,"##s8");
                    ToggleButton("Ext Mag (AR)",&Cloud.ExtendedMagAR,"##emAR");
                    ToggleButton("Quick Mag (AR)",&Cloud.QuickDrawMagAR,"##qmAR");
                    ToggleButton("Ext+Quick Mag",&Cloud.ExtendedQuickDrawMagAR,"##eqm");
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Vehicles")) {
                    ImGui::BeginChild("##vhl",ImVec2(0,0),false);
                    ToggleButton("Buggy",&Cloud.Buggy,"##Bggy");
                    ToggleButton("Dacia",&Cloud.Dacia,"##Dacia");
                    ToggleButton("Coupe RB",&Cloud.CoupeRB,"##CRB");
                    ToggleButton("UAZ Open",&Cloud.UAZOpen,"##UO");
                    ToggleButton("UAZ Closed",&Cloud.UAZClosed,"##UC");
                    ToggleButton("Mirado",&Cloud.Mirado,"##Mir");
                    ToggleButton("Pickup Open",&Cloud.PickupOpen,"##PO");
                    ToggleButton("Pickup Closed",&Cloud.PickupClosed,"##PC");
                    ToggleButton("Van",&Cloud.Van,"##Van");
                    ToggleButton("Rony",&Cloud.Rony,"##Rony");
                    ToggleButton("Tukshai",&Cloud.Tukshai,"##Tuk");
                    ToggleButton("Scooter",&Cloud.Scooter,"##Scoo");
                    ToggleButton("Monster Truck",&Cloud.MonsterTruck,"##MT");
                    ToggleButton("Motor Glider",&Cloud.MotorGlider,"##MG");
                    ToggleButton("Snowmobile",&Cloud.Snowmobile,"##Snow");
                    ToggleButton("Zima",&Cloud.Zima,"##Zima");
                    ToggleButton("PG-117",&Cloud.PG117,"##PG");
                    ToggleButton("Jet Ski",&Cloud.JetSki,"##JSki");
                    ToggleButton("BRDM-2",&Cloud.BRDM2,"##BRDM");
                    ToggleButton("UTV",&Cloud.UTV,"##UTV");
                    ToggleButton("Roadster",&Cloud.Roadster,"##Road");
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            // Close the card body manually (no EndCard — see BeginCard removal above)
            ImGui::Dummy(ImVec2(0, GAP));
        }

        // ─── TAB: AIM ────────────────────────────────────────
        if (g_Tab == TAB_AIM) {
            BeginCard("AIM  —  Targeting", NX_MAGENTA);
            ToggleButton("Aimbot",         &Cloud.自瞄,         "##ab");
            ToggleButton("Aim Snipers",    &Cloud.狙击,         "##sn");
            ToggleButton("Aim Shotguns",   &Cloud.喷子,         "##sg");
            ToggleButton("Aim Line",       &Cloud.AimLine,      "##al");
            ToggleButton("Touch Position", &Cloud.TouchPosition,"##tp");
            ToggleButton("Ignore Knocked", &Cloud.IgnoreKnocked,"##ik");
            ToggleButton("Ignore Bots",    &Cloud.IgnoreBots,   "##ib");
            ToggleButton("Dynamic FOV",    &Cloud.DynamicFOV,   "##df");
            ToggleButton("Hide FOV Ring",  &Cloud.隐藏圈,        "##hf");
            ToggleButton("Auto Fire",      &Cloud.开火,          "##afire");
            ToggleButton("Auto Clicker",   &Cloud.AutoClicker,  "##ac");
            EndCard(NX_MAGENTA);

            if (Cloud.AutoClicker) {
                BeginCard("Auto Clicker Settings", NX_YELLOW);
                ToggleButton("Position Clicker",&Cloud.AutoClicker2,"##ac2");
                HorizontalSlider("Position X", &Cloud.ClickPositionX,0.f,3000.f);
                HorizontalSlider("Position Y", &Cloud.ClickPositionY,0.f,2000.f);
                HorizontalSlider("Click Radius",&Cloud.ClickRadius,1.f,500.f);
                HorizontalSlider("Click Speed ms",&Cloud.ClickSpeed,0.1f,10.f);
                if (ImGui::Combo("Port",&chargingPortIndex,"Right\0Left\0\0"))
                    ChargingPortSide = chargingPortIndex==0?0.f:1.f;
                EndCard(NX_YELLOW);
            }

            BeginCard("Aim Config", NX_CYAN);
            ImGui::Dummy(ImVec2(0,4));
            ImGui::Text("Target:");
            ImGui::SameLine(130);
            ImGui::SetNextItemWidth(200.0f);
            const char* tgts[]={"Head","Chest"};
            ImGui::Combo("##at",&Cloud.部位,tgts,2);
            ImGui::Dummy(ImVec2(0,4));
            ImGui::Text("Mode:");
            ImGui::SameLine(130);
            ImGui::SetNextItemWidth(200.0f);
            const char* modes[]={"Automatic","Per-Weapon"};
            ImGui::Combo("##am",&g_AimMode,modes,2);
            ImGui::Dummy(ImVec2(0,4));
            EndCard(NX_CYAN);

            if (g_AimMode == 0) {
                BeginCard("Global Aim Settings", NX_GREEN);
                HorizontalSlider("FOV Range",      &Cloud.自瞄范围, 0,1000);
                HorizontalSlider("No Scope Distance",   &Cloud.腰射距离, 0,500);
                HorizontalSlider("Scope Distance", &Cloud.开镜距离, 0,500);
                HorizontalSlider("No Scope Speed",      &Cloud.腰射速度, 0,20);
                HorizontalSlider("Scope Speed",    &Cloud.开镜速度, 0,20);
                HorizontalSlider("No Scope Recoil",     &Cloud.腰射压枪, 0,5);
                HorizontalSlider("Scope Recoil",   &Cloud.开镜压枪, 0,5);
                HorizontalSlider("Prediction",     &Cloud.预判,     0,15);
                HorizontalSlider("Vehicle Prediction",  &Cloud.载具预判, 0,15);
                EndCard(NX_GREEN);

                // FIX-5: Aim circle color picker
                BeginCard("Aim Circle Color", NX_GREEN);
                ColorRow("FOV Circle", (float*)&Cloud.自瞄圈颜色.Value);
                EndCard(NX_GREEN);
            } else {
                BeginCard("Per-Weapon Settings", NX_GREEN);
                WeaponSettings("M416",   &Cloud.M416_Settings,   1);
                WeaponSettings("AKM",    &Cloud.AKM_Settings,    2);
                WeaponSettings("M762",   &Cloud.M762_Settings,   3);
                WeaponSettings("SCAR-L", &Cloud.SCAR_L_Settings, 4);
                WeaponSettings("AUG",    &Cloud.AUG_Settings,    5);
                WeaponSettings("Groza",  &Cloud.Groza_Settings,  6);
                WeaponSettings("M24",    &Cloud.M24_Settings,    7);
                WeaponSettings("AWM",    &Cloud.AWM_Settings,    8);
                WeaponSettings("Kar98k", &Cloud.Kar98k_Settings, 9);
                WeaponSettings("Mosin",  &Cloud.Mosin_Settings,  10);
                WeaponSettings("MK14",   &Cloud.MK14_Settings,   11);
                WeaponSettings("Mini14", &Cloud.Mini14_Settings, 12);
                WeaponSettings("SLR",    &Cloud.SLR_Settings,    13);
                WeaponSettings("SKS",    &Cloud.SKS_Settings,    14);
                WeaponSettings("DP28",   &Cloud.DP28_Settings,   15);
                WeaponSettings("M249",   &Cloud.M249_Settings,   16);
                WeaponSettings("UMP45",  &Cloud.UMP45_Settings,  17);
                WeaponSettings("Vector", &Cloud.Vector_Settings, 18);
                WeaponSettings("S12K",   &Cloud.S12K_Settings,   24);
                WeaponSettings("DBS",    &Cloud.DBS_Settings,    25);
                WeaponSettings("S686",   &Cloud.S686_Settings,   26);
                WeaponSettings("S1897",  &Cloud.S1897_Settings,  27);
                EndCard(NX_GREEN);
            }
        }

        // ─── TAB: MEMORY ─────────────────────────────────────
        if (g_Tab == TAB_MEMORY) {
            BeginCard("MEMORY  —  Game Patches", NX_RED);
            ToggleButton("No Recoil",          &Cloud.无后, "##nr");
            ToggleButton("FOV Hack (iPad View)",&Cloud.广角,"##fovh");
            ImGui::Dummy(ImVec2(0,6));
            HorizontalSlider("TPP FOV",    &Cloud.三称广角大小, 0,150);
            HorizontalSlider("FPP FOV",    &Cloud.一称广角大小, 0,150);
            HorizontalSlider("Scope FOV",  &Cloud.开镜广角大小, 0,25);
            HorizontalSlider("Camera FOV", &Cloud.相机广角大小, 0,500);
            EndCard(NX_RED);
        }

        // ─── TAB: SETTINGS ───────────────────────────────────
        if (g_Tab == TAB_SETTINGS) {
            BeginCard("Performance", NX_CYAN);
            HorizontalSlider("Target FPS",   &Cloud.目标帧率,  30,240);
            HorizontalSlider("Touch Rate Hz",&Cloud.TouchRate, 60,5000);
            EndCard(NX_CYAN);

            BeginCard("Anti-Detection", NX_YELLOW);
            ToggleButton("Anti Screenshot",&Cloud.防录屏,"##ascr");
            EndCard(NX_YELLOW);

            BeginCard("Config", NX_GREEN);
            ImGui::Dummy(ImVec2(0,4));
            ImGui::PushStyleColor(ImGuiCol_Button,
                ImVec4(0.03f,0.38f,0.44f,1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                ImVec4(0.0f,0.55f,0.64f,1.0f));
            if (ImGui::Button("  SAVE CONFIG", ImVec2(pw,52)))
                SaveConfig("NovaXConfig.ini");
            ImGui::PopStyleColor(2);
            ImGui::Dummy(ImVec2(0,8));
            ImGui::PushStyleColor(ImGuiCol_Button,
                ImVec4(0.48f,0.04f,0.04f,1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                ImVec4(0.72f,0.06f,0.06f,1.0f));
            if (ImGui::Button("  EXIT", ImVec2(pw,52)))
                定义.Thread = false;
            ImGui::PopStyleColor(2);
            ImGui::Dummy(ImVec2(0,4));
            EndCard(NX_GREEN);
        }

        ImGui::EndChild();  // ##content

        // Window border glow
        dl->AddRect(WP, ImVec2(WP.x+W,WP.y+H),
                    NX_CYAN_DIM, 5.0f, 0, 1.2f);

        ImGui::End();
        ImGui::PopStyleColor();  // WindowBg
        ImGui::PopStyleVar(2);   // WindowPadding, WindowBorderSize

    } // end show_main

    // ── MINI PILL (when hidden) ───────────────────────────────
    if (!Window || MainWindow.x < 2)
    {
        if (SmallMenuToggle) SmallMenuToggle = false;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   22.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,  1.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(0,0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg,
            ImVec4(0.04f,0.04f,0.07f,0.95f));
        ImGui::PushStyleColor(ImGuiCol_Border,
            ImVec4(0.0f,0.76f,0.88f,0.55f));

        ImGui::SetNextWindowSize(ImVec2(185,62), ImGuiCond_Always);
        ImGui::Begin("##pill", nullptr,
            ImGuiWindowFlags_NoTitleBar        |
            ImGuiWindowFlags_NoScrollbar        |
            ImGuiWindowFlags_NoScrollWithMouse  |
            ImGuiWindowFlags_NoResize);

        SmallMenuPosition  = ImGui::GetWindowPos();
        ImDrawList* pdl    = ImGui::GetWindowDrawList();
        ImVec2 pp          = SmallMenuPosition;

        // Pulsing dot
        float pulse = (sinf((float)ImGui::GetTime() * 2.5f) + 1.0f) * 0.5f;
        ImU32 dot_col = IM_COL32(0, (int)(160+pulse*70), (int)(200+pulse*30), 255);
        pdl->AddCircleFilled(ImVec2(pp.x+22, pp.y+28), 7.0f, dot_col);
        pdl->AddCircleFilled(ImVec2(pp.x+22, pp.y+28), 3.5f, NX_BG0);

        pdl->AddText(ImGui::GetFont(), 24.0f,
                     ImVec2(pp.x+38, pp.y+8),  NX_CYAN,    "NOVA X");
        pdl->AddText(ImGui::GetFont(), 18.0f,
                     ImVec2(pp.x+38, pp.y+34), NX_TEXT_DIM,"tap to open");

        if (WindowClick()) {
            Window = true;
            SmallMenuToggle = true;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);
        ImGui::End();
    }

    // ── TOUCH ANCHOR ─────────────────────────────────────────
    if (Cloud.TouchPosition)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg,
            ImVec4(0.52f,0.04f,0.04f,0.80f));
        ImGui::PushStyleColor(ImGuiCol_Border,
            ImVec4(1.0f,0.22f,0.22f,0.65f));

        ImGui::SetNextWindowSize(ImVec2(76,46), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(Cloud.Touch.x, Cloud.Touch.y),
                                ImGuiCond_FirstUseEver);
        ImGui::Begin("##touch", nullptr,
            ImGuiWindowFlags_NoTitleBar  |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoResize);

        ImVec2 tp = ImGui::GetWindowPos();
        Cloud.Touch.x = tp.x;
        Cloud.Touch.y = tp.y;
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(tp.x+8, tp.y+14),
            IM_COL32(255,70,70,255), "TOUCH");

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

    // ── Feed aim/touch thread ─────────────────────────────────
    定义.采样率 = Cloud.TouchRate;
    定义.触摸x  = 屏幕y - Cloud.Touch.y + 21;
    定义.触摸y  = Cloud.Touch.x + 30;
}

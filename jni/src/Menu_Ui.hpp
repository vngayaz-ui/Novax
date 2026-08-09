// ============================================================
// NovaX — Menu_Ui.hpp
// Style: RIYAZXERO Menu.h — Purple | Top Tab Bar
// Exact sizes from Menu.h: window=783x603, tabs=146.5x23,
// child panels matching Menu.h exactly
// All NovaX Cloud.xxx functions 100% intact
// ============================================================
#include <Function.hpp>
#include <ctime>

// ── State ────────────────────────────────────────────────────
enum NXTab { TAB_HOME=0, TAB_ESP, TAB_ITEMS, TAB_AIM, TAB_MEMORY, TAB_SETTINGS, TAB_COUNT };
static int  g_Tab         = TAB_HOME;
bool        kernel_active = true;

// ── Live clock title (exact Menu.h style) ────────────────────
static char g_title[128] = "NOVA X ###nxtitle";
static void UpdateTitle() {
    static std::time_t last = 0;
    std::time_t now = std::time(nullptr);
    if (now == last) return;
    last = now;
    std::tm* t = std::localtime(&now);
    char ts[64];
    std::strftime(ts, sizeof(ts), "%H:%M:%S %b %d %Y", t);
    snprintf(g_title, sizeof(g_title), "NOVA X  (%s) ###nxtitle", ts);
}

// ── Exact palette from Menu.h ─────────────────────────────────
static const ImVec4 COL_TAB_ACTIVE   = {160/255.f, 50/255.f, 240/255.f, 0.95f};
static const ImVec4 COL_TAB_INACTIVE = { 45/255.f, 15/255.f,  70/255.f, 0.80f};

// ── Push full professional purple theme (matching need ui styling) ─────────────
static void PushPurpleStyle() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg,       ImVec4(0.08f, 0.02f, 0.13f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg,        ImVec4(0.10f, 0.03f, 0.16f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_Border,         ImVec4(0.55f, 0.18f, 0.85f, 0.75f));
    ImGui::PushStyleColor(ImGuiCol_Button,         ImVec4(0.35f, 0.08f, 0.55f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  ImVec4(0.65f, 0.18f, 0.95f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,   ImVec4(0.80f, 0.25f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,        ImVec4(0.16f, 0.05f, 0.24f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.40f, 0.12f, 0.65f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  ImVec4(0.65f, 0.18f, 0.95f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,     ImVec4(0.65f, 0.18f, 0.95f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,ImVec4(0.85f, 0.30f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark,      ImVec4(0.85f, 0.30f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Header,         ImVec4(0.35f, 0.08f, 0.55f, 0.70f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,  ImVec4(0.65f, 0.18f, 0.95f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Tab,            ImVec4(0.18f, 0.06f, 0.28f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_TabActive,      ImVec4(0.65f, 0.18f, 0.95f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TabHovered,     ImVec4(0.50f, 0.12f, 0.80f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg,        ImVec4(0.12f, 0.03f, 0.20f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive,  ImVec4(0.25f, 0.06f, 0.40f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Separator,      ImVec4(0.55f, 0.18f, 0.85f, 0.50f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab,  ImVec4(0.45f, 0.12f, 0.70f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_Text,           ImVec4(0.96f, 0.90f, 1.00f, 1.00f));
    // 22 colors
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,  14.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,   12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,   10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding,     10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding,    10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.2f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign,ImVec2(0.5f, 0.5f));
}
static void PopPurpleStyle() {
    ImGui::PopStyleColor(22);
    ImGui::PopStyleVar(7);
}

// ── Widget helpers ────────────────────────────────────────────
// Row height for toggles — same visual height as Menu.h checkbox rows
static constexpr float ROW_H = 32.0f;
static constexpr float SLD_H = 52.0f;

// TOGGLE — pill switch, 52x26, purple ON
inline bool ToggleButton(const char* label, bool* v, const char* id) {
    const float SW = 52.0f, SH = 26.0f;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 rp   = ImGui::GetCursorScreenPos();
    float avail = ImGui::GetContentRegionAvail().x;
    float sx    = rp.x + avail - SW - 4.f;
    float sy    = rp.y + (ROW_H - SH) * 0.5f;

    // Label dead zone
    float lw = sx - 6.f - rp.x;
    if (lw > 0.f) {
        ImGui::SetCursorScreenPos(rp);
        char did[128]; snprintf(did,sizeof(did),"##dead_%s",id);
        ImGui::InvisibleButton(did, ImVec2(lw, ROW_H));
    }
    ImVec2 ts = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(rp.x+6, rp.y+(ROW_H-ts.y)*0.5f),
                IM_COL32(230,230,242,255), label);

    ImGui::SetCursorScreenPos(ImVec2(sx-4.f, rp.y));
    ImGui::InvisibleButton(id, ImVec2(SW+8.f, ROW_H));
    bool changed = false;
    if (ImGui::IsItemClicked()) { *v=!*v; changed=true; }

    float t = *v ? 1.0f : 0.0f;
    ImGuiContext& g = *GImGui;
    if (g.LastActiveId == g.CurrentWindow->GetID(id)) {
        float ta = ImSaturate(g.LastActiveIdTimer / 0.10f);
        t = *v ? ta : (1.f-ta);
    }
    ImGui::SetCursorScreenPos(ImVec2(rp.x, rp.y+ROW_H));

    // Track: grey->purple
    ImU32 track = ImGui::GetColorU32(ImVec4(
        0.22f+(0.47f-0.22f)*t, 0.f, 0.22f+(0.78f-0.22f)*t, 1.f));
    ImVec2 sp(sx, sy);
    dl->AddRectFilled(sp, ImVec2(sp.x+SW,sp.y+SH), track, SH*0.5f);
    dl->AddRect(sp, ImVec2(sp.x+SW,sp.y+SH),
                IM_COL32(180,80,255,(int)(40+t*80)), SH*0.5f, 0, 1.0f);
    float kx = sp.x + SH*0.5f + t*(SW-SH);
    dl->AddCircleFilled(ImVec2(kx,sp.y+SH*0.5f), SH*0.40f-1.f,
                        IM_COL32(255,255,255,255));
    // separator line
    dl->AddLine(ImVec2(rp.x,rp.y+ROW_H-1),
                ImVec2(rp.x+avail,rp.y+ROW_H-1),
                IM_COL32(100,50,160,70), 1.f);
    ImGui::SetCursorScreenPos(ImVec2(rp.x, rp.y+ROW_H));
    return changed;
}

// SLIDER — PushItemWidth(650) style like Menu.h, value on right
// Uses standard ImGui SliderFloat with purple style already pushed
inline bool NXSlider(const char* label, float* v, float mn, float mx) {
    ImGui::Text("%s", label);
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 60.f);
    ImGui::Text("%.2f", *v);
    char sid[128]; snprintf(sid,sizeof(sid),"###sl_%s",label);
    ImGui::PushItemWidth(-1);
    bool ch = ImGui::SliderFloat(sid, v, mn, mx, "");
    ImGui::PopItemWidth();
    return ch;
}

// COLOR ROW — label left, picker right, ROW_H tall
static void ColorRow(const char* label, float* col4) {
    ImVec2 rp   = ImGui::GetCursorScreenPos();
    float avail = ImGui::GetContentRegionAvail().x;
    ImVec2 ts   = ImGui::CalcTextSize(label);
    ImGui::GetWindowDrawList()->AddText(
        ImVec2(rp.x+6, rp.y+(ROW_H-ts.y)*0.5f),
        IM_COL32(230,230,242,255), label);
    ImGui::SetCursorScreenPos(ImVec2(rp.x+avail-160.f, rp.y+(ROW_H-28.f)*0.5f));
    ImGui::SetNextItemWidth(155.f);
    char cid[64]; snprintf(cid,sizeof(cid),"##col_%s",label);
    ImGui::ColorEdit4(cid, col4,
        ImGuiColorEditFlags_NoInputs|ImGuiColorEditFlags_NoLabel|
        ImGuiColorEditFlags_AlphaBar|ImGuiColorEditFlags_PickerHueBar);
    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(rp.x,rp.y+ROW_H-1), ImVec2(rp.x+avail,rp.y+ROW_H-1),
        IM_COL32(100,50,160,70), 1.f);
    ImGui::SetCursorScreenPos(ImVec2(rp.x, rp.y+ROW_H));
}

// WEAPON SETTINGS
static void WeaponSettings(const char* name, WeaponAimSettings* s, int idx) {
    if (!s) return;
    ImGui::PushID(idx);
    ImGui::Checkbox("##en", &s->Enabled);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(200/255.f,150/255.f,255/255.f,1.f), "%s", name);
    if (s->Enabled) {
        ImGui::Indent(14.f);
        ImGui::PushItemWidth(376.f);
        ImGui::SliderFloat("###fov",  &s->FOVRange,       0.f,1000.f); ImGui::SameLine(); ImGui::Text("FOV Range");
        ImGui::SliderFloat("###hd",   &s->HipfireDistance,0.f,500.f);  ImGui::SameLine(); ImGui::Text("No Scope Dist");
        ImGui::SliderFloat("###sd",   &s->ScopeDistance,  0.f,500.f);  ImGui::SameLine(); ImGui::Text("Scope Dist");
        ImGui::SliderFloat("###hs",   &s->HipfireSpeed,   0.f,5.f);    ImGui::SameLine(); ImGui::Text("No Scope Speed");
        ImGui::SliderFloat("###ss",   &s->ScopeSpeed,     0.f,5.f);    ImGui::SameLine(); ImGui::Text("Scope Speed");
        ImGui::SliderFloat("###hr",   &s->HipfireRecoil,  0.f,5.f);    ImGui::SameLine(); ImGui::Text("No Scope Recoil");
        ImGui::SliderFloat("###sr",   &s->ScopeRecoil,    0.f,5.f);    ImGui::SameLine(); ImGui::Text("Scope Recoil");
        ImGui::SliderFloat("###pred", &s->Prediction,     0.f,5.f);    ImGui::SameLine(); ImGui::Text("Prediction");
        ImGui::PopItemWidth();
        ImGui::Unindent(14.f);
    }
    ImGui::Separator();
    ImGui::PopID();
}

// ─────────────────────────────────────────────────────────────
// MAIN RENDER — 窗口函数
// Window: 783x603 (exact Menu.h)
// Tabs: 5 tabs = ImVec2(146.5, 23) each (exact Menu.h)
// ─────────────────────────────────────────────────────────────
void 窗口函数()
{
    UpdateTitle();

    ImGuiIO& io = ImGui::GetIO();
    float disp_w = (io.DisplaySize.x > 0.0f) ? io.DisplaySize.x : 1920.0f;
    float disp_h = (io.DisplaySize.y > 0.0f) ? io.DisplaySize.y : 1080.0f;

    // Window target dimensions (783x603 exact Menu.h, responsive to display size)
    float targetW = ImMin(783.0f, disp_w * 0.95f);
    float targetH = ImMin(603.0f, disp_h * 0.90f);
    float fps_now = io.Framerate;
    float step    = (fps_now > 0.5f) ? (1800.0f / fps_now) : 30.0f;

    if (Window) {
        MainWindow.x = (MainWindow.x < targetW) ? MainWindow.x+step : targetW;
        MainWindow.y = (MainWindow.y < targetH) ? MainWindow.y+step : targetH;
    } else {
        MainWindow.x = (MainWindow.x > 0) ? MainWindow.x-step : 0.0f;
        MainWindow.y = (MainWindow.y > 0) ? MainWindow.y-step : 0.0f;
    }

    bool show_main = (MainWindow.x > 2 && MainWindow.y > 2);

    if (show_main)
    {
        PushPurpleStyle();

        if (MainMenuSwitch) {
            if (Cloud.PillPositionX > 5.0f && Cloud.PillPositionY > 5.0f) {
                ImGui::SetNextWindowPos(
                    ImVec2(Cloud.PillPositionX, Cloud.PillPositionY),
                    ImGuiCond_Always);
            } else {
                ImGui::SetNextWindowPos(
                    ImVec2(disp_w * 0.5f, disp_h * 0.5f),
                    ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            }
            MainMenuSwitch = false;
        }
        ImGui::SetNextWindowSize(MainWindow, ImGuiCond_Always);

        static auto Flags = ImGuiWindowFlags_NoResize      |
                            ImGuiWindowFlags_NoSavedSettings|
                            ImGuiWindowFlags_NoCollapse     |
                            ImGuiWindowFlags_NoScrollbar;

        bool open = true;
        if (ImGui::Begin(g_title, &open, Flags))
        {
            if (!open) Window = false;
            主菜单位置 = ImGui::GetWindowPos();

            // ── TAB BUTTONS — exact Menu.h ───────────────────────────
            ImGuiStyle& sty = ImGui::GetStyle();
            sty.FrameRounding = 4.0f;

            float tbw = (MainWindow.x / (float)TAB_COUNT) - 6.0f;

            const char* tab_names[TAB_COUNT] = {
                "HOME","ESP","ITEMS","AIM","MEMORY","SETTINGS"
            };
            for (int i = 0; i < TAB_COUNT; i++) {
                ImGui::PushStyleColor(ImGuiCol_Button,
                    g_Tab == i ? COL_TAB_ACTIVE : COL_TAB_INACTIVE);
                if (ImGui::Button(tab_names[i], ImVec2(tbw, 23.0f)))
                    g_Tab = i;
                ImGui::PopStyleColor();
                if (i < TAB_COUNT-1) ImGui::SameLine();
            }
            sty.FrameRounding = 0.0f;
            ImGui::Separator();

            // ── TAB CONTENT ──────────────────────────────────────────
            const float CW = (MainWindow.x > 30.0f) ? (MainWindow.x - 17.0f) : 766.0f;
            const float CH = (MainWindow.y > 70.0f) ? (MainWindow.y - 69.0f) : 534.0f;

            // ════════════════════════════════════════════════
            // TAB: HOME
            // ════════════════════════════════════════════════
            if (g_Tab == TAB_HOME)
            {
                ImGui::BeginChild("##home", ImVec2(CW, CH), true);

                ImGui::Spacing();
                // FPS line
                {
                    static float fps_s = 60.f;
                    float fps_raw = fps_now;
                    if (定义.threadtime && *定义.threadtime > 0.f)
                        fps_raw = 1000.f / *定义.threadtime;
                    fps_s = fps_s*0.95f + fps_raw*0.05f;
                    ImGui::Text("Ping: %.1fms", ImGui::GetIO().Framerate);
                    ImGui::SameLine(300);
                    ImGui::Text("(%.2f) FPS", fps_s);
                    ImGui::SameLine(550);
                    ImGui::TextColored(ImVec4(0,1,0.5f,1),
                        "P:%d  B:%d", 定义.真人数量, 定义.人机数量);
                }
                ImGui::Separator();
                ImGui::Spacing();

                // KERNEL button — full width
                ImGui::PushStyleColor(ImGuiCol_Button,
                    kernel_active ? ImVec4(0.55f,0.02f,0.02f,1.f)
                                  : ImVec4(0.03f,0.35f,0.15f,1.f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                    kernel_active ? ImVec4(0.75f,0.06f,0.06f,1.f)
                                  : ImVec4(0.05f,0.50f,0.22f,1.f));
                if (ImGui::Button(
                        kernel_active ? "  STOP KERNEL" : "  START KERNEL",
                        ImVec2(CW-16.f, 40.f))) {
                    kernel_active = !kernel_active;
                    if (!kernel_active) {
                        定义.数组数量=0; 定义.真人数量=0;
                        定义.人机数量=0; 定义.自瞄人数=0;
                    }
                }
                ImGui::PopStyleColor(2);
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::BulletText("Play Safe Avoid Reports");
                ImGui::BulletText("Use features on Gold Tier+ only");
                ImGui::BulletText("All Features Working Properly");
                ImGui::BulletText("Enhanced Security");
                ImGui::Separator();
                ImGui::TextColored(ImVec4(224/255.f,189/255.f,255/255.f,0.8f),
                    "NOVA X — Professional");

                ImGui::EndChild();
            }

            // ════════════════════════════════════════════════
            // TAB: ESP
            // exact Menu.h: BeginChild(0,0), buttons(595,32),
            // checkboxes with SameLine(300) and SameLine(600)
            // ════════════════════════════════════════════════
            if (g_Tab == TAB_ESP)
            {
                ImGui::BeginChild("#ESPTAB", ImVec2(0,0), true);
                sty.ScrollbarSize = 0.f;
                ImGui::SetCursorPos(ImVec2(8,8));

                // Activate/Deactivate — exact Menu.h size (595,32)
                if (ImGui::Button("Activate All ESP", ImVec2(595.f, 32.f))) {
                    Cloud.Line=Cloud.Box=Cloud.Skeleton=Cloud.Name=
                    Cloud.Health=Cloud.Distance=Cloud.Weapon=
                    Cloud.Alert=Cloud.EnemyLanding=Cloud.Grenade=
                    Cloud.Vehicle=Cloud.TimerRevive=Cloud.TeamID=true;
                }
                if (ImGui::Button("Deactivate All ESP", ImVec2(595.f, 32.f))) {
                    Cloud.Line=Cloud.Box=Cloud.Skeleton=Cloud.Name=
                    Cloud.Health=Cloud.Distance=Cloud.Weapon=
                    Cloud.Alert=Cloud.EnemyLanding=Cloud.Grenade=
                    Cloud.Vehicle=Cloud.TimerRevive=Cloud.TeamID=
                    Cloud.HideBots=Cloud.Decryption=false;
                }
                ImGui::Separator();

                // Checkboxes — Menu.h 3-column style with SameLine(300),SameLine(600)
                ImGui::Checkbox("Player Line",     &Cloud.Line);
                ImGui::SameLine(300); ImGui::Checkbox("Player Box",      &Cloud.Box);
                ImGui::SameLine(600); ImGui::Checkbox("Skeleton",        &Cloud.Skeleton);

                ImGui::Checkbox("Player Name",     &Cloud.Name);
                ImGui::SameLine(300); ImGui::Checkbox("Health Bar",      &Cloud.Health);
                ImGui::SameLine(600); ImGui::Checkbox("Distance",        &Cloud.Distance);

                ImGui::Checkbox("Team ID",         &Cloud.TeamID);
                ImGui::SameLine(300); ImGui::Checkbox("Weapon Icon",     &Cloud.Weapon);
                ImGui::SameLine(600); ImGui::Checkbox("Alert 360",       &Cloud.Alert);

                ImGui::Checkbox("Landing Status",  &Cloud.EnemyLanding);
                ImGui::SameLine(300); ImGui::Checkbox("Grenade Warning", &Cloud.Grenade);
                ImGui::SameLine(600); ImGui::Checkbox("Vehicle ESP",     &Cloud.Vehicle);

                ImGui::Checkbox("Timer Revive",    &Cloud.TimerRevive);
                ImGui::SameLine(300); ImGui::Checkbox("Hide Bots",       &Cloud.HideBots);
                ImGui::SameLine(600); ImGui::Checkbox("Decryption",      &Cloud.Decryption);

                ImGui::Separator();
                ImGui::Text("                                                                    ESP Colors");

                // Color pickers in tab bar
                if (ImGui::BeginTabBar("EspColors")) {
                    if (ImGui::BeginTabItem("Player Line")) {
                        ImGui::Separator();
                        ImGui::PushItemWidth(650);
                        ImGui::ColorEdit4("###plc",(float*)&Cloud.PlayerLineColor.Value,ImGuiColorEditFlags_NoLabel);
                        ImGui::SameLine(); ImGui::Text("Player Line Color");
                        ImGui::ColorEdit4("###blc",(float*)&Cloud.BotLineColor.Value,ImGuiColorEditFlags_NoLabel);
                        ImGui::SameLine(); ImGui::Text("Bot Line Color");
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Box")) {
                        ImGui::Separator();
                        ImGui::PushItemWidth(650);
                        ImGui::ColorEdit4("###pbc",(float*)&Cloud.PlayerBoxColor.Value,ImGuiColorEditFlags_NoLabel);
                        ImGui::SameLine(); ImGui::Text("Player Box Color");
                        ImGui::ColorEdit4("###bbc",(float*)&Cloud.BotBoxColor.Value,ImGuiColorEditFlags_NoLabel);
                        ImGui::SameLine(); ImGui::Text("Bot Box Color");
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Health")) {
                        ImGui::Separator();
                        ImGui::PushItemWidth(650);
                        ImGui::ColorEdit4("###phc",(float*)&Cloud.PlayerHealthColor.Value,ImGuiColorEditFlags_NoLabel);
                        ImGui::SameLine(); ImGui::Text("Player Health Color");
                        ImGui::ColorEdit4("###bhc",(float*)&Cloud.BotHealthColor.Value,ImGuiColorEditFlags_NoLabel);
                        ImGui::SameLine(); ImGui::Text("Bot Health Color");
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Team/Vehicle")) {
                        ImGui::Separator();
                        ImGui::PushItemWidth(650);
                        ImGui::ColorEdit4("###tidc",(float*)&Cloud.TeamIDColor.Value,ImGuiColorEditFlags_NoLabel);
                        ImGui::SameLine(); ImGui::Text("Team ID Color");
                        ImGui::ColorEdit4("###vehc",(float*)&Cloud.载具颜色.Value,ImGuiColorEditFlags_NoLabel);
                        ImGui::SameLine(); ImGui::Text("Vehicle Color");
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::EndChild();
            }

            // ════════════════════════════════════════════════
            // TAB: ITEMS — sub-tab bar like Menu.h
            // ════════════════════════════════════════════════
            if (g_Tab == TAB_ITEMS)
            {
                ImGui::BeginChild("##itemstab", ImVec2(CW, CH), true);
                if (ImGui::BeginTabBar("##items_tabs", ImGuiTabBarFlags_FittingPolicyScroll))
                {
                    if (ImGui::BeginTabItem("Weapons")) {
                        ImGui::BeginChild("##wl",ImVec2(0,0),false);
                        ImGui::Checkbox("M416",&Cloud.M416);       ImGui::SameLine(200);
                        ImGui::Checkbox("M16A4",&Cloud.M16A4);     ImGui::SameLine(400);
                        ImGui::Checkbox("SCAR-L",&Cloud.SCARL);
                        ImGui::Checkbox("AKM",&Cloud.AKM);         ImGui::SameLine(200);
                        ImGui::Checkbox("AUG",&Cloud.AUG);         ImGui::SameLine(400);
                        ImGui::Checkbox("QBZ",&Cloud.QBZ);
                        ImGui::Checkbox("G36C",&Cloud.G36C);       ImGui::SameLine(200);
                        ImGui::Checkbox("M762",&Cloud.M762);       ImGui::SameLine(400);
                        ImGui::Checkbox("MK47",&Cloud.MK47);
                        ImGui::Checkbox("ACE32",&Cloud.ACE32);     ImGui::SameLine(200);
                        ImGui::Checkbox("UMP45",&Cloud.UMP);       ImGui::SameLine(400);
                        ImGui::Checkbox("Vector",&Cloud.Vector);
                        ImGui::Checkbox("UZI",&Cloud.UZI);         ImGui::SameLine(200);
                        ImGui::Checkbox("Tommy Gun",&Cloud.TommyGun); ImGui::SameLine(400);
                        ImGui::Checkbox("PP-19 Bizon",&Cloud.Bison);
                        ImGui::Checkbox("MP5K",&Cloud.MP5K);       ImGui::SameLine(200);
                        ImGui::Checkbox("P90",&Cloud.P90);         ImGui::SameLine(400);
                        ImGui::Checkbox("JS9",&Cloud.JS9);
                        ImGui::Checkbox("S12K",&Cloud.S12K);       ImGui::SameLine(200);
                        ImGui::Checkbox("S1897",&Cloud.S1897);     ImGui::SameLine(400);
                        ImGui::Checkbox("S686",&Cloud.S686);
                        ImGui::Checkbox("DBS",&Cloud.DBS);         ImGui::SameLine(200);
                        ImGui::Checkbox("NS2000",&Cloud.NS2000);   ImGui::SameLine(400);
                        ImGui::Checkbox("Kar98K",&Cloud.Kar98K);
                        ImGui::Checkbox("M24",&Cloud.M24);         ImGui::SameLine(200);
                        ImGui::Checkbox("Win94",&Cloud.Win94);     ImGui::SameLine(400);
                        ImGui::Checkbox("Mosin",&Cloud.Mosin);
                        ImGui::Checkbox("Mini14",&Cloud.Mini14);   ImGui::SameLine(200);
                        ImGui::Checkbox("SKS",&Cloud.SKS);         ImGui::SameLine(400);
                        ImGui::Checkbox("SLR",&Cloud.SLR);
                        ImGui::Checkbox("QBU",&Cloud.QBU);         ImGui::SameLine(200);
                        ImGui::Checkbox("Mk12",&Cloud.Mk12);       ImGui::SameLine(400);
                        ImGui::Checkbox("VSS",&Cloud.VSS);
                        ImGui::Checkbox("DP-28",&Cloud.DP28);      ImGui::SameLine(200);
                        ImGui::Checkbox("M249",&Cloud.M249);       ImGui::SameLine(400);
                        ImGui::Checkbox("Pan",&Cloud.Pan);
                        ImGui::Checkbox("Flare Gun",&Cloud.Flare);
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Equipment")) {
                        ImGui::BeginChild("##eql",ImVec2(0,0),false);
                        ImGui::Checkbox("Backpack Lv.1",&Cloud.Backpack1); ImGui::SameLine(200);
                        ImGui::Checkbox("Backpack Lv.2",&Cloud.Backpack2); ImGui::SameLine(400);
                        ImGui::Checkbox("Backpack Lv.3",&Cloud.Backpack3);
                        ImGui::Checkbox("Helmet Lv.1",&Cloud.MotorcycleHelmet1); ImGui::SameLine(200);
                        ImGui::Checkbox("Helmet Lv.2",&Cloud.MilitaryHelmet2);   ImGui::SameLine(400);
                        ImGui::Checkbox("Helmet Lv.3",&Cloud.SpetsnazHelmet3);
                        ImGui::Checkbox("Vest Lv.1",&Cloud.PoliceVest1); ImGui::SameLine(200);
                        ImGui::Checkbox("Vest Lv.2",&Cloud.PoliceVest2); ImGui::SameLine(400);
                        ImGui::Checkbox("Vest Lv.3",&Cloud.MilitaryVest3);
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Consumables")) {
                        ImGui::BeginChild("##cnl",ImVec2(0,0),false);
                        ImGui::Checkbox("Energy Drink",&Cloud.EnergyDrink); ImGui::SameLine(200);
                        ImGui::Checkbox("Adrenaline",&Cloud.AdrenalineSyringe); ImGui::SameLine(400);
                        ImGui::Checkbox("Painkillers",&Cloud.Painkillers);
                        ImGui::Checkbox("Bandage",&Cloud.Bandage); ImGui::SameLine(200);
                        ImGui::Checkbox("First Aid Kit",&Cloud.FirstAidKit); ImGui::SameLine(400);
                        ImGui::Checkbox("Med Kit",&Cloud.MedKit);
                        ImGui::Checkbox("Self AED",&Cloud.SelfAED);
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Throwables")) {
                        ImGui::BeginChild("##thl",ImVec2(0,0),false);
                        ImGui::Checkbox("Frag Grenade",&Cloud.FragGrenade); ImGui::SameLine(200);
                        ImGui::Checkbox("Smoke",&Cloud.SmokeGrenade);       ImGui::SameLine(400);
                        ImGui::Checkbox("Stun",&Cloud.StunGrenade);
                        ImGui::Checkbox("Molotov",&Cloud.MolotovCocktail);  ImGui::SameLine(200);
                        ImGui::Checkbox("Sticky Bomb",&Cloud.StickyBomb);
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Ammo")) {
                        ImGui::BeginChild("##aml",ImVec2(0,0),false);
                        ImGui::Checkbox("9mm",&Cloud.MM9);         ImGui::SameLine(200);
                        ImGui::Checkbox("7.62mm",&Cloud.MM762);    ImGui::SameLine(400);
                        ImGui::Checkbox("5.56mm",&Cloud.MM556);
                        ImGui::Checkbox("45 ACP",&Cloud.ACP45);    ImGui::SameLine(200);
                        ImGui::Checkbox("12 Gauge",&Cloud.Gauge12); ImGui::SameLine(400);
                        ImGui::Checkbox("300 Magnum",&Cloud.Magnum300);
                        ImGui::Checkbox("Bolt",&Cloud.Bolt);
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Attachments")) {
                        ImGui::BeginChild("##atl",ImVec2(0,0),false);
                        ImGui::Checkbox("Scope x3",&Cloud.ScopeX3);   ImGui::SameLine(200);
                        ImGui::Checkbox("Scope x4",&Cloud.ScopeX4);   ImGui::SameLine(400);
                        ImGui::Checkbox("Scope x6",&Cloud.ScopeX6);
                        ImGui::Checkbox("Scope x8",&Cloud.ScopeX8);   ImGui::SameLine(200);
                        ImGui::Checkbox("Ext Mag",&Cloud.ExtendedMagAR); ImGui::SameLine(400);
                        ImGui::Checkbox("Quick Mag",&Cloud.QuickDrawMagAR);
                        ImGui::Checkbox("Ext+Quick Mag",&Cloud.ExtendedQuickDrawMagAR);
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Vehicles")) {
                        ImGui::BeginChild("##vhl",ImVec2(0,0),false);
                        ImGui::Checkbox("Buggy",&Cloud.Buggy);       ImGui::SameLine(200);
                        ImGui::Checkbox("Dacia",&Cloud.Dacia);       ImGui::SameLine(400);
                        ImGui::Checkbox("Coupe RB",&Cloud.CoupeRB);
                        ImGui::Checkbox("UAZ Open",&Cloud.UAZOpen);  ImGui::SameLine(200);
                        ImGui::Checkbox("UAZ Closed",&Cloud.UAZClosed); ImGui::SameLine(400);
                        ImGui::Checkbox("Mirado",&Cloud.Mirado);
                        ImGui::Checkbox("Pickup Open",&Cloud.PickupOpen); ImGui::SameLine(200);
                        ImGui::Checkbox("Pickup Closed",&Cloud.PickupClosed); ImGui::SameLine(400);
                        ImGui::Checkbox("Van",&Cloud.Van);
                        ImGui::Checkbox("Rony",&Cloud.Rony);         ImGui::SameLine(200);
                        ImGui::Checkbox("Tukshai",&Cloud.Tukshai);   ImGui::SameLine(400);
                        ImGui::Checkbox("Scooter",&Cloud.Scooter);
                        ImGui::Checkbox("Monster Truck",&Cloud.MonsterTruck); ImGui::SameLine(200);
                        ImGui::Checkbox("Motor Glider",&Cloud.MotorGlider);   ImGui::SameLine(400);
                        ImGui::Checkbox("Snowmobile",&Cloud.Snowmobile);
                        ImGui::Checkbox("Zima",&Cloud.Zima);         ImGui::SameLine(200);
                        ImGui::Checkbox("PG-117",&Cloud.PG117);      ImGui::SameLine(400);
                        ImGui::Checkbox("Jet Ski",&Cloud.JetSki);
                        ImGui::Checkbox("BRDM-2",&Cloud.BRDM2);      ImGui::SameLine(200);
                        ImGui::Checkbox("UTV",&Cloud.UTV);           ImGui::SameLine(400);
                        ImGui::Checkbox("Roadster",&Cloud.Roadster);
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::EndChild();
            }

            // ════════════════════════════════════════════════
            // TAB: AIM — exact Menu.h: Columns(2), col1=257
            // left child(242,518), right child(492,518)
            // ════════════════════════════════════════════════
            if (g_Tab == TAB_AIM)
            {
                ImGui::BeginChild("##aimtab", ImVec2(CW, CH), true);
                sty.ScrollbarSize = 0.f;
                ImGui::Columns(2);
                ImGui::SetColumnOffset(1, 257.f);
                ImGui::SetCursorPos(ImVec2(8,8));

                // LEFT column — toggles (242x518 like Menu.h)
                ImGui::BeginChild("##aimL", ImVec2(242.f, 518.f), true);
                ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(255,0,0,255));
                ImGui::Checkbox("Aimbot",         &Cloud.自瞄);
                ImGui::Checkbox("Aim Snipers",    &Cloud.狙击);
                ImGui::Checkbox("Aim Shotguns",   &Cloud.喷子);
                ImGui::Checkbox("Aim Line",       &Cloud.AimLine);
                ImGui::Checkbox("Touch Position", &Cloud.TouchPosition);
                ImGui::Checkbox("Ignore Knocked", &Cloud.IgnoreKnocked);
                ImGui::Checkbox("Ignore Bots",    &Cloud.IgnoreBots);
                ImGui::Checkbox("Dynamic FOV",    &Cloud.DynamicFOV);
                ImGui::Checkbox("Hide FOV Ring",  &Cloud.隐藏圈);
                ImGui::Checkbox("Auto Fire",      &Cloud.开火);
                ImGui::Checkbox("Auto Clicker",   &Cloud.AutoClicker);
                ImGui::PopStyleColor();
                ImGui::EndChild();

                ImGui::NextColumn();

                // RIGHT column — settings (492x518 like Menu.h)
                ImGui::SetCursorPos(ImVec2(265.f, 8.f));
                ImGui::BeginChild("##aimR", ImVec2(492.f, 518.f), true);
                if (ImGui::BeginTabBar("##aimtabs")) {

                    if (ImGui::BeginTabItem("Config")) {
                        ImGui::Spacing();
                        ImGui::PushItemWidth(376.f);
                        const char* tgts[]={"Head","Chest"};
                        ImGui::Combo("###at",&Cloud.部位,tgts,2);
                        ImGui::SameLine(); ImGui::Text("Target Bone");
                        const char* modes[]={"Automatic","Per-Weapon"};
                        ImGui::Combo("###am",&Cloud.AimMode,modes,2);
                        ImGui::SameLine(); ImGui::Text("Aim Mode");
                        ImGui::PopItemWidth();
                        ImGui::EndTabItem();
                    }

                    if (Cloud.AimMode == 0 && ImGui::BeginTabItem("Global Aim")) {
                        ImGui::PushItemWidth(376.f);
                        ImGui::SliderFloat("###fovr", &Cloud.自瞄范围, 0,1000); ImGui::SameLine(); ImGui::Text("FOV Range");
                        ImGui::SliderFloat("###hd",   &Cloud.腰射距离, 0,500);  ImGui::SameLine(); ImGui::Text("No Scope Distance");
                        ImGui::SliderFloat("###sd",   &Cloud.开镜距离, 0,500);  ImGui::SameLine(); ImGui::Text("Scope Distance");
                        ImGui::SliderFloat("###hs",   &Cloud.腰射速度, 0,5);    ImGui::SameLine(); ImGui::Text("No Scope Speed");
                        ImGui::SliderFloat("###ss",   &Cloud.开镜速度, 0,5);    ImGui::SameLine(); ImGui::Text("Scope Speed");
                        ImGui::SliderFloat("###hr",   &Cloud.腰射压枪, 0,5);    ImGui::SameLine(); ImGui::Text("No Scope Recoil");
                        ImGui::SliderFloat("###sr",   &Cloud.开镜压枪, 0,5);    ImGui::SameLine(); ImGui::Text("Scope Recoil");
                        ImGui::SliderFloat("###pred", &Cloud.预判,     0,5);    ImGui::SameLine(); ImGui::Text("Prediction");
                        ImGui::SliderFloat("###vpred",&Cloud.载具预判, 0,5);    ImGui::SameLine(); ImGui::Text("Vehicle Pred");
                        ImGui::Separator();
                        ImGui::ColorEdit4("###fovcol",(float*)&Cloud.自瞄圈颜色.Value,ImGuiColorEditFlags_NoLabel);
                        ImGui::SameLine(); ImGui::Text("FOV Circle Color");
                        ImGui::PopItemWidth();
                        ImGui::EndTabItem();
                    }

                    if (Cloud.AimMode == 1 && ImGui::BeginTabItem("Per-Weapon")) {
                        ImGui::BeginChild("##pwl",ImVec2(0,0),false);
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
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }

                    if (Cloud.AutoClicker && ImGui::BeginTabItem("Auto Clicker")) {
                        ImGui::PushItemWidth(376.f);
                        ImGui::Checkbox("Position Clicker",&Cloud.AutoClicker2);
                        ImGui::SliderFloat("###cx",&Cloud.ClickPositionX,0.f,3000.f); ImGui::SameLine(); ImGui::Text("Position X");
                        ImGui::SliderFloat("###cy",&Cloud.ClickPositionY,0.f,2000.f); ImGui::SameLine(); ImGui::Text("Position Y");
                        ImGui::SliderFloat("###cr",&Cloud.ClickRadius,   1.f,500.f);  ImGui::SameLine(); ImGui::Text("Click Radius");
                        ImGui::SliderFloat("###cs",&Cloud.ClickSpeed,  0.1f,10.f);   ImGui::SameLine(); ImGui::Text("Click Speed ms");
                        ImGui::Combo("###port",&Cloud.ChargingPortIndex,"Right\0Left\0\0");
                        ImGui::SameLine(); ImGui::Text("Port");
                        if (ImGui::IsItemDeactivatedAfterEdit())
                            Cloud.ChargingPortSide = Cloud.ChargingPortIndex==0?0.f:1.f;
                        ImGui::PopItemWidth();
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }
                ImGui::EndChild(); // aimR
                ImGui::Columns(1);
                ImGui::EndChild(); // aimtab
            }

            // ════════════════════════════════════════════════
            // TAB: MEMORY — exact Menu.h layout
            // Left child(242,518), Right child(492,518)
            // ════════════════════════════════════════════════
            if (g_Tab == TAB_MEMORY)
            {
                ImGui::BeginChild("##MemoryTab", ImVec2(CW, CH), true);
                sty.ScrollbarSize = 0.f;
                ImGui::Columns(2);
                ImGui::SetColumnOffset(1, 257.f);
                ImGui::SetCursorPos(ImVec2(8,8));

                ImGui::BeginChild("##memL", ImVec2(242.f, 518.f), true);
                ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(255,0,0,255));
                ImGui::Checkbox("No Recoil",           &Cloud.无后);
                ImGui::Checkbox("FOV Hack (iPad View)", &Cloud.广角);
                ImGui::PopStyleColor();
                ImGui::EndChild();

                ImGui::NextColumn();
                ImGui::SetCursorPos(ImVec2(265.f, 8.f));
                ImGui::BeginChild("##memR", ImVec2(0,0), true);

                ImGui::PushItemWidth(376.f);
                ImGui::SliderFloat("###tppfov", &Cloud.三称广角大小, 0,150); ImGui::SameLine(); ImGui::Text("TPP FOV");
                ImGui::SliderFloat("###fppfov", &Cloud.一称广角大小, 0,150); ImGui::SameLine(); ImGui::Text("FPP FOV");
                ImGui::SliderFloat("###scpfov", &Cloud.开镜广角大小, 0,25);  ImGui::SameLine(); ImGui::Text("Scope FOV");
                ImGui::SliderFloat("###camfov", &Cloud.相机广角大小, 0,500); ImGui::SameLine(); ImGui::Text("Camera FOV");
                ImGui::PopItemWidth();

                ImGui::EndChild();
                ImGui::Columns(1);
                ImGui::EndChild();
            }

            // ════════════════════════════════════════════════
            // TAB: SETTINGS — exact Menu.h layout
            // Top-left child(371,125), Top-right child(371,125)
            // Bottom special features child(750,373)
            // ════════════════════════════════════════════════
            if (g_Tab == TAB_SETTINGS)
            {
                ImGui::BeginChild("##Settings", ImVec2(CW, CH), true);
                sty.ScrollbarSize = 0.f;
                ImGui::SetCursorPos(ImVec2(8,8));

                // TOP-LEFT box (371x125) — exact Menu.h
                ImGui::BeginChild("##cfg1", ImVec2(371.f, 125.f), true);
                ImGui::SetCursorPos(ImVec2(8,8));
                ImGui::Separator();

                // Actor Dump — exact Menu.h pos and size (357x21)
                ImGui::SetCursorPos(ImVec2(8,40));
                if (ImGui::Button("Actor Dump", ImVec2(357.f, 21.f)))
                    定义.数组数量 = 0;

                ImGui::Separator();

                // Save / Load — exact Menu.h (173x21) each
                ImGui::SetCursorPos(ImVec2(8,72));
                static float saveTimer = 0.f;
                static bool  saveOK    = false;
                if (ImGui::Button("Save Config", ImVec2(173.f, 21.f))) {
                    saveOK    = SaveConfig("config.bin");
                    saveTimer = 3.0f;
                }
                ImGui::SameLine();
                if (ImGui::Button("Load Config", ImVec2(173.f, 21.f))) {
                    ReadConfig("config.bin");
                    saveOK    = true;
                    saveTimer = 3.0f;
                }
                if (saveTimer > 0.f) {
                    saveTimer -= ImGui::GetIO().DeltaTime;
                    ImGui::SetCursorPos(ImVec2(8,104));
                    ImGui::TextColored(
                        saveOK ? ImVec4(0,1,0.5f,1) : ImVec4(1,0.2f,0.2f,1),
                        saveOK ? "Config saved /sdcard/NovaX/config.bin"
                               : "SAVE FAILED — check permission");
                }
                ImGui::EndChild();

                ImGui::SameLine();

                // TOP-RIGHT box (371x125) — EXIT button like Menu.h Unload
                ImGui::BeginChild("##cfg2", ImVec2(371.f, 125.f), true);
                ImGui::SetCursorPos(ImVec2(8,8));
                ImGui::PushStyleColor(ImGuiCol_Button,
                    ImVec4(0.48f,0.04f,0.04f,1.f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                    ImVec4(0.72f,0.06f,0.06f,1.f));
                // exact Menu.h: Unload button ImVec2(355,109)
                if (ImGui::Button("  EXIT", ImVec2(355.f, 109.f))) {
                    SaveConfig("config.bin");
                    定义.Thread = false;
                }
                ImGui::PopStyleColor(2);
                ImGui::EndChild();

                // BOTTOM — Performance + Anti-Detection (750x373)
                ImGui::Text("                                                                                         Performance  ");
                ImGui::BeginChild("##cfg3", ImVec2(750.f, 373.f), true);

                ImGui::PushItemWidth(650.f);
                ImGui::SliderFloat("###fps",  &Cloud.目标帧率, 60,165, "%.0f FPS");
                ImGui::SameLine(); ImGui::Text("Target FPS");
                ImGui::SliderFloat("###tr",   &Cloud.TouchRate,60,5000,"%.0f Hz");
                ImGui::SameLine(); ImGui::Text("Touch Rate Hz");
                ImGui::PopItemWidth();

                ImGui::Separator();

                // Anti-Detection
                ImGui::Checkbox("Anti Screenshot", &Cloud.防录屏);

                ImGui::EndChild();
                ImGui::EndChild(); // Settings outer
            }

            // ── End Begin/End ────────────────────────────────
        } // end ImGui::Begin(g_title)
        ImGui::End();
        PopPurpleStyle();
    } // end show_main

    // ── MINI PILL ─────────────────────────────────────────────
    if (!Window || MainWindow.x < 2)
    {
        if (SmallMenuToggle) SmallMenuToggle = false;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,  20.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,   ImVec2(0,0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg,
            ImVec4(60/255.f,0,100/255.f,0.95f));
        ImGui::PushStyleColor(ImGuiCol_Border,
            ImVec4(150/255.f,50/255.f,255/255.f,0.7f));

        ImGui::SetNextWindowSize(ImVec2(160,52), ImGuiCond_Always);
        ImGui::Begin("##pill", nullptr,
            ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoScrollbar|
            ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoResize);

        ImVec2 pp       = ImGui::GetWindowPos();
        Cloud.PillPositionX = pp.x;
        Cloud.PillPositionY = pp.y;
        ImDrawList* pdl = ImGui::GetWindowDrawList();
        float pulse = (sinf((float)ImGui::GetTime()*2.5f)+1.f)*0.5f;
        pdl->AddCircleFilled(ImVec2(pp.x+18,pp.y+26), 7.f,
            IM_COL32((int)(120+pulse*40),0,(int)(200+pulse*30),255));
        pdl->AddCircleFilled(ImVec2(pp.x+18,pp.y+26), 3.5f,
            IM_COL32(20,0,40,255));
        pdl->AddText(ImGui::GetFont(),22.f,ImVec2(pp.x+32,pp.y+6),
            IM_COL32(200,150,255,255),"NOVA X");
        pdl->AddText(ImGui::GetFont(),16.f,ImVec2(pp.x+32,pp.y+30),
            IM_COL32(160,120,200,180),"tap to open");

        // Open on tap via WindowClick() (tap-without-drag detector: pressing the
        // movable pill window body sets ActiveId == window MoveId). Do NOT add a
        // widget here — a widget (e.g. InvisibleButton) would capture ActiveId to
        // its own ID and WindowClick() would never fire.
        if (WindowClick()) {
            Window = true;
            SmallMenuToggle = true;
            MainMenuSwitch = true;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);
        ImGui::End();
    }

    // ── TOUCH ANCHOR ─────────────────────────────────────────
    if (Cloud.TouchPosition)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   8.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg,
            ImVec4(0.40f,0.f,0.60f,0.80f));
        ImGui::PushStyleColor(ImGuiCol_Border,
            ImVec4(0.60f,0.20f,1.f,0.70f));

        ImGui::SetNextWindowSize(ImVec2(76,46), ImGuiCond_Always);
        if (g_TouchPosNeedsRestore) {
            ImGui::SetNextWindowPos(
                ImVec2(Cloud.Touch.x,Cloud.Touch.y), ImGuiCond_Always);
            g_TouchPosNeedsRestore = false;
        } else {
            ImGui::SetNextWindowPos(
                ImVec2(Cloud.Touch.x,Cloud.Touch.y), ImGuiCond_FirstUseEver);
        }
        ImGui::Begin("##touch", nullptr,
            ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoScrollbar|
            ImGuiWindowFlags_NoResize);
        ImVec2 tp = ImGui::GetWindowPos();
        Cloud.Touch.x = tp.x; Cloud.Touch.y = tp.y;
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(tp.x+8,tp.y+14), IM_COL32(200,100,255,255),"TOUCH");
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

    // ── Feed aim/touch thread ─────────────────────────────────
    定义.采样率 = Cloud.TouchRate;
    定义.触摸x  = 屏幕y - Cloud.Touch.y + 21;
    定义.触摸y  = Cloud.Touch.x + 30;
}

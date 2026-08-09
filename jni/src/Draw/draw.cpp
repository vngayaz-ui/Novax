#define LOGTAG "ANativeWindow_EGL"
#define ANDROID 1

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/log.h>
#include <cstdlib>
#include <cstring>
#include <string>

#include <draw.h>
#include <utils.h>
#include <Funs.h>
#include "Font.hpp"
#include "ANativeWindowCreator.hpp"

EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;

ANativeWindow* native_window = nullptr;

Screen full_screen;

int 方向 = -1;
int 屏幕x = 0;
int 屏幕y = 0;
int init_屏幕x = 0;
int init_屏幕y = 0;

bool g_Initialized = false;

std::string MyImGui::exec(std::string command)
{
    char buffer[128];
    std::string result;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
        return "popen failed!";

    while (!feof(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe))
            result += buffer;
    }

    pclose(pipe);
    return result;
}

int MyImGui::init_egl(int _屏幕x, int _屏幕y, bool log, bool flp)
{
    native_window = android::ANativeWindowCreator::Create("Distant", _屏幕x, _屏幕y, flp);
    ANativeWindow_acquire(native_window);

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY)
    {
        printf("eglGetDisplay error=%u\n", glGetError());
        return -1;
    }

    if (log)
        printf("eglGetDisplay ok\n");

    if (eglInitialize(display, 0, 0) != EGL_TRUE)
    {
        printf("eglInitialize error=%u\n", glGetError());
        return -1;
    }

    if (log)
        printf("eglInitialize ok\n");

    EGLint num_config = 0;

    const EGLint attribList[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 5,
        EGL_GREEN_SIZE, 6,
        EGL_RED_SIZE, 5,
        EGL_BUFFER_SIZE, 32,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    if (eglChooseConfig(display, attribList, nullptr, 0, &num_config) != EGL_TRUE)
    {
        printf("eglChooseConfig error=%u\n", glGetError());
        return -1;
    }

    if (log)
        printf("num_config=%d\n", num_config);

    if (!eglChooseConfig(display, attribList, &config, 1, &num_config))
    {
        printf("eglChooseConfig error=%u\n", glGetError());
        return -1;
    }

    if (log)
        printf("eglChooseConfig ok\n");

    EGLint egl_format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);

    ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);

    const EGLint attrib_list[] =
    {
        EGL_CONTEXT_CLIENT_VERSION,
        3,
        EGL_NONE
    };

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    if (context == EGL_NO_CONTEXT)
    {
        printf("eglCreateContext error=%u\n", glGetError());
        return -1;
    }

    if (log)
        printf("eglCreateContext ok\n");

    surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    if (surface == EGL_NO_SURFACE)
    {
        printf("eglCreateWindowSurface error=%u\n", glGetError());
        return -1;
    }

    if (log)
        printf("eglCreateWindowSurface ok\n");

    if (!eglMakeCurrent(display, surface, surface, context))
    {
        printf("eglMakeCurrent error=%u\n", glGetError());
        return -1;
    }

    if (log)
        printf("eglMakeCurrent ok\n");

    MyImGui::ImGui_init();

    return 1;
}

void MyImGui::ImGui_init()
{
    if (g_Initialized)
        return;

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui_ImplAndroid_Init(native_window);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    ImFontConfig fontConfig;
    ImFontConfig config;
    ImFontConfig font_cfg;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    // Window style
    style.WindowBorderSize = 0;
    style.WindowRounding = 8;
    style.WindowPadding = ImVec2(12, 12);

    // Frame style
    style.FrameRounding = 6;
    style.FramePadding = ImVec2(10, 6);

    // Scrollbar style — mobile needs large scrollbar for touch
    style.ScrollbarRounding = 8;
    style.ScrollbarSize = 28;    // was 16 — too small on phone screen

    // Grab style
    style.GrabRounding = 4;
    style.GrabMinSize = 14;

    // Tab style
    style.TabRounding = 6;
    style.TabBorderSize = 0;

    // ===== Cyberpunk/Neon Color Theme =====
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg]           = ImVec4(0.06f, 0.06f, 0.08f, 0.95f);
    colors[ImGuiCol_ChildBg]            = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
    colors[ImGuiCol_PopupBg]            = ImVec4(0.06f, 0.06f, 0.08f, 1.0f);
    colors[ImGuiCol_Border]             = ImVec4(0.20f, 0.20f, 0.25f, 0.50f);
    colors[ImGuiCol_FrameBg]            = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]     = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
    colors[ImGuiCol_FrameBgActive]      = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    colors[ImGuiCol_TitleBg]            = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
    colors[ImGuiCol_TitleBgActive]      = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]   = ImVec4(0.04f, 0.04f, 0.06f, 1.0f);
    colors[ImGuiCol_MenuBarBg]          = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
    colors[ImGuiCol_ScrollbarBg]        = ImVec4(0.06f, 0.06f, 0.08f, 0.8f);
    colors[ImGuiCol_ScrollbarGrab]      = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.20f, 0.20f, 0.25f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.25f, 0.30f, 1.0f);
    colors[ImGuiCol_CheckMark]          = ImVec4(0.00f, 0.96f, 1.00f, 1.0f);
    colors[ImGuiCol_SliderGrab]         = ImVec4(1.00f, 0.00f, 0.43f, 0.80f);
    colors[ImGuiCol_SliderGrabActive]   = ImVec4(1.00f, 0.20f, 0.55f, 1.00f);
    colors[ImGuiCol_Button]             = ImVec4(0.12f, 0.12f, 0.15f, 1.0f);
    colors[ImGuiCol_ButtonHovered]      = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    colors[ImGuiCol_ButtonActive]       = ImVec4(0.00f, 0.70f, 0.80f, 1.0f);
    colors[ImGuiCol_Header]             = ImVec4(0.12f, 0.12f, 0.15f, 1.0f);
    colors[ImGuiCol_HeaderHovered]      = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    colors[ImGuiCol_HeaderActive]       = ImVec4(0.00f, 0.70f, 0.80f, 1.0f);
    colors[ImGuiCol_Separator]          = ImVec4(0.15f, 0.15f, 0.18f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]   = ImVec4(0.00f, 0.96f, 1.00f, 0.50f);
    colors[ImGuiCol_SeparatorActive]    = ImVec4(0.00f, 0.96f, 1.00f, 0.80f);
    colors[ImGuiCol_ResizeGrip]         = ImVec4(0.00f, 0.96f, 1.00f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]  = ImVec4(0.00f, 0.96f, 1.00f, 0.40f);
    colors[ImGuiCol_ResizeGripActive]   = ImVec4(0.00f, 0.96f, 1.00f, 0.60f);
    colors[ImGuiCol_Tab]               = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    colors[ImGuiCol_TabHovered]         = ImVec4(0.00f, 0.96f, 1.00f, 0.30f);
    colors[ImGuiCol_TabActive]          = ImVec4(1.00f, 0.00f, 0.43f, 0.60f);
    colors[ImGuiCol_TabUnfocused]       = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    colors[ImGuiCol_Text]               = ImVec4(0.95f, 0.95f, 0.93f, 1.0f);
    colors[ImGuiCol_TextDisabled]       = ImVec4(0.50f, 0.50f, 0.55f, 1.0f);
    colors[ImGuiCol_PlotLines]          = ImVec4(0.60f, 0.60f, 0.65f, 1.0f);
    colors[ImGuiCol_PlotLinesHovered]   = ImVec4(1.00f, 0.00f, 0.43f, 1.0f);
    colors[ImGuiCol_PlotHistogram]      = ImVec4(0.00f, 0.96f, 1.00f, 1.0f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.43f, 1.0f);
    colors[ImGuiCol_TextSelectedBg]     = ImVec4(0.00f, 0.96f, 1.00f, 0.30f);
    colors[ImGuiCol_DragDropTarget]     = ImVec4(0.00f, 0.96f, 1.00f, 0.60f);
    colors[ImGuiCol_NavHighlight]       = ImVec4(0.00f, 0.96f, 1.00f, 1.0f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]  = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]   = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    // ===== Load Font =====
    font_cfg.SizePixels = 32;

    io.Fonts->Clear();

    static ImVector<ImWchar> ranges;

    if (ranges.empty())
    {
        ImFontGlyphRangesBuilder builder;

        constexpr ImWchar Ranges[] =
        {
            0x0020, 0x00FF,
            0x0100, 0x024F,
            0x0300, 0x03FF,
            0x0400, 0x052F,
            0x0600, 0x06FF,
            0xFE00, 0xFEFF,
            0x0E00, 0x0E7F,
            0x2DE0, 0x2DFF,
            0x2000, 0x206F,
            0x25A0, 0x25FF,  // Geometric Shapes (for icons like ◉ ◎ ▣ ⚙)
            0x2600, 0x26FF,  // Misc Symbols (★ ♥ ⚠ ⚙ etc)
            0x2700, 0x27BF,  // Dingbats (✔ ✖ ✚ etc)
            0x3000, 0x30FF,
            0x31F0, 0x31FF,
            0xFF00, 0xFFEF,
            0x4E00, 0x9FAF,
            0xA640, 0xA69F,
            0x3131, 0x3163,
            0xAC00, 0xD7A3,
            0
        };
        builder.AddRanges(Ranges);
        builder.BuildRanges(&ranges);
    }

    io.Fonts->AddFontFromMemoryTTF((void*)字体数组, 字体字节, font_cfg.SizePixels, &font_cfg, ranges.Data);

    g_Initialized = true;  // mark init complete so shutdown() works correctly
}

void MyImGui::shutdown()
{
    if (!g_Initialized)
        return;

    // Wait for all GPU commands to finish
    glFinish();

    // Unbind EGL context before destroying resources
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();

    if (display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (context != EGL_NO_CONTEXT)
            eglDestroyContext(display, context);

        if (surface != EGL_NO_SURFACE)
            eglDestroySurface(display, surface);

        eglTerminate(display);
    }

    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;

    if (native_window)
    {
        android::ANativeWindowCreator::Destroy(native_window);
        ANativeWindow_release(native_window);
        native_window = nullptr;
    }

    g_Initialized = false;
}

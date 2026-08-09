#ifndef DRAW_H
#define DRAW_H

#include <iostream>
#include <thread>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <android/native_window.h>
#include <EGL/eglext.h>
#include <GLES3/gl3platform.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl32.h>
#include <ANativeWindowCreator.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_android.h>

using namespace std;
using namespace std::chrono_literals;

extern EGLDisplay display;
extern EGLConfig config;
extern EGLSurface surface;
extern ANativeWindow *native_window;
extern EGLContext context;

struct Screen {
    float ScreenX;
    float ScreenY;
};

extern Screen full_screen;
extern int 方向;                 // Screen orientation
extern int 屏幕x, 屏幕y;         // Screen width and height
extern int init_屏幕x, init_屏幕y; // Initial screen width and height
extern bool g_Initialized;       // Initialization status

class MyImGui {
public:
    string exec(string command);
    int init_egl(int _屏幕x, int _屏幕y, bool log, bool flp = false);
    void screen_config();
    void ImGui_init();
    void shutdown();
};

extern MyImGui Initptr;

#endif
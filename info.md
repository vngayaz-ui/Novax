# NovaX — Project Info & Full Code Analysis

## 1. Project Overview

- **Type:** Android game cheat/overlay (external memory + ImGui overlay) for PUBG Mobile (Tencent `/ PUBG Mobile KR `com_tencent_ig`), plus variants:
  - `com.tencent.ig` (Global)
  - `com.pubg.krmobile` (Korea)
  - `com.rekoo.pubgm` (Vietnam/Rekoo)
  - `com.vng.pubgmobile` (VN - sets `isVNG`)
  - `com.pubg.imobile` (India/BGMI - sets `isBGMI`)
- **Product name:** `NovaX` (menu title), module `NovaX.sh`
- **Language:** C++20, NDK build, `arm64-v8a` only, Android 5.0+ (APP_PLATFORM android-21)
- **Tech stack:**
  - External memory read/write via a **kernel driver** (syscall + ioctl based)
  - ImGui (Dear ImGui) rendered with **OpenGL ES 3.0** over a custom **native Surface** created directly through libgui (`SurfaceComposerClient`) — no Java/Activity needed
  - Touch input captured from `/dev/input/event*` and re-injected via **uinput**
- **Code style:** Chinese identifiers used throughout (`屏幕x`, `定义`, `绘制函数`, `窗口函数`, `矩阵转换`, etc.)

> ⚠️ Educational/analysis only. This modifies another game's memory and input; use at your own risk.

---

## 2. Directory / File Structure

```
New hk/
├── info.md                      ← this file
└── jni/
    ├── Android.mk               build script (executable, C++20, arm64)
    ├── Application.mk           STL/ABI/platform settings
    ├── lib/
    │   └── libdeclassify.a      prebuilt static lib (deobfuscation helpers)
    ├── include/
    │   ├── Main.h               main header, global func decls
    │   ├── Draw/draw.h          EGL + MyImGui class + screen globals
    │   ├── Fun/
    │   │   ├── Funs.h           定义结构 (global game-state struct), bone/draw structs
    │   │   ├── Member.h         math structs 二维/三维/四维, FMatrix/FTransform, bone math
    │   │   ├── Kernel.hpp       proKpm (syscall driver) + c_driver (ioctl driver)
    │   │   └── timer.h          high-precision FPS/sleep/affinity utilities
    │   ├── ImGui/               Dear ImGui headers (imgui.h, imgui_internal.h, backends)
    │   │   ├── imgui_image.h    texture creation decls
    │   │   ├── Touch/touch.h    touch types + decls (FingerStatus, TouchFinger)
    │   │   └── native_surface/  ANativeWindowCreator.hpp (SurfaceComposerClient wrappers)
    │   └── (embree3, foundation — referenced in Android.mk, not present in repo)
    └── src/
        ├── Main.cpp             entry point `main()`, render loop, ESP/aim drawing
        ├── Menu_Ui.hpp          full ImGui menu (窗口函数), custom widgets
        ├── Function.hpp         SDK offsets, config struct, item/weapon tables, helpers
        ├── Definitions.h        stb_image + all weapon/throwable image headers
        ├── Funs.cpp             definitions of the global structs
        ├── GrenadeTrajectory.cpp/.h   grenade path prediction + drawing
        ├── SDK.hpp              (~5 MB) UE4 SDK dump
        ├── Draw/
        │   ├── draw.cpp         init_egl, ImGui_init (font/style), shutdown
        │   └── Font.hpp         (~6 MB) embedded TTF font byte array (字体数组)
        ├── ImGui/               ImGui implementation + imgui_impl_android/opengl3 backends
        │   ├── imgui_switch.cpp ImGui::SwitchButton custom widget
        │   ├── Touch/touch.cpp  touch capture + uinput injection + aim touch drive
        │   ├── native_surface/utils.cpp  dlblob/fdlopen helpers
        │   └── shm_open_anon.cpp         anonymous shared memory
        ├── image/
        │   ├── stb_image.h      image loader
        │   ├── Weapons/*.h      ~80 weapon PNG byte arrays
        │   └── Throwables/*.h   5 grenade PNG byte arrays
        └── curl/                libcurl + OpenSSL static prebuilts + json.hpp (unused in current build path)
```

---

## 3. Build Configuration

### `jni/Android.mk`
- Prebuilt static lib `libdeclassify` (`lib/libdeclassify.a`)
- Executable module `NovaX.sh`
- Flags: `-std=c++20 -O3 -fomit-frame-pointer -fno-rtti -fpermissive -fvisibility=hidden -ffunction-sections -fdata-sections`, `-s` strip
- Linker: `--gc-sections --strip-debug -z,relro,-z,now -s -pthread`, strips everything
- Sources: all `*.c*` under `src/`, `src/Draw/`, `src/ImGui/`, `src/ImGui/Touch/`, `src/ImGui/native_surface/`
- Link: `libdeclassify` + `$(LOCAL_PATH)/lib/*.a`, `-llog -landroid -lEGL -lGLESv2 -lGLESv3 -lGLESv1_CM -lz`
- Includes: `include`, `include/Draw`, `include/ImGui`, `include/ImGui/Touch`, `include/Fun`, `include/embree3`, `include/foundation`, `include/ImGui/native_surface`, `src`, `src/image`

### `jni/Application.mk`
- `APP_STL := c++_static`
- `APP_ABI := arm64-v8a`
- `APP_PLATFORM := android-21`

---

## 4. Memory Access (Driver)

### `include/Fun/Kernel.hpp`

**`proKpm` class** — raw Linux `syscall(18, flags=616, ...)` interface to a kernel driver (`proKpm` style "KPM" driver):
- `syscall(18, 616, &ptr, 0x400)` → READMEM
- `syscall(18, 616, &ptr, 0x401)` → READMEMLIST
- `syscall(18, 616, &ptr, 0x200)` → WRITEMEM
- `syscall(18, 616, name, 0x50)` → PROCPID (get pid by package name)
- `syscall(18, 616, name, 0x900)` → CALLFUNC_1
- `get_module_base` scans `/proc/<pid>/maps` for `libUE4.so`

**`c_driver` class** — ioctl-based driver interface (the one actually used as global `Driver`):
- Custom structs `COPY_MEMORY`, `MODULE_BASE`
- Operations: `OP_INIT_KEY 0x800`, `OP_READ_MEM 0x801`, `OP_WRITE_MEM 0x802`, `OP_MODULE_BASE 0x803`
- Driver auto-discovery (in order):
  1. `QX()` — shell trick that finds a deleted `/dev/*` driver char device referenced by a deleted process FD and recreates it with `mknod`
  2. `dev()` — scans `/dev` for a 6-char char/block device with uid/gid 0, size 0, ctime year ≤ 1980
  3. `devproc()` — scans `/proc/<6char>` entries
- Template helpers: `Read<T>(addr)`, `Write<T>(addr, value)`, `getPID(package)`, `获取模块头` (via ioctl OP_MODULE_BASE), `获取模块头2` (via /proc maps)

**Global:** `static c_driver Driver;`

### Process/module init (`Function.hpp:GetModuleBase`)
- Picks first matching package pid, sets `isBGMI`/`isVNG` flags
- Kernel < 6.0 → `Driver.获取模块头(pid, "libUE4.so")` (ioctl), else `/proc maps` (`获取模块头2`)

---

## 5. Native Overlay Surface (no Activity)

### `include/ImGui/native_surface/ANativeWindowCreator.hpp` (~919 lines)
- Dynamically resolves libgui symbols via mangled names (e.g. `_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8Ejjij...`) across Android versions
- Wraps `SurfaceComposerClient`, `SurfaceControl`, `Transaction` (setLayer, setTrustedOverlay, setInputWindowInfo, setFlags, show/hide, reparent, setMatrix, setPosition, mirrorSurface)
- **`Create(name, w, h, skipScreenshot)`** — creates a full-screen `Surface`, returns `ANativeWindow*`
- **`GetDisplayInfo()`** — returns `{width, height, 方向(rotation), ...}` via `getDisplayInfo`
- **`ProcessMirrorDisplay()`** — mirrors the overlay surface onto other display layer-stacks (Android 11–15) so the overlay shows on all displays, with scale/offset transform
- **`SetSkipScreenshot(win, skip)`** — Android 12+: sets surface flags `0x40|0x80` so the overlay is hidden from screenshots (anti-capture). Controlled by config `Cloud.防录屏` ("anti screen-record").

### `src/Draw/draw.cpp:init_egl`
1. `ANativeWindowCreator::Create("Distant", 屏幕x+屏幕y, 屏幕y+屏幕x, 防录屏)` — square oversized window covering both orientations
2. EGL init: `eglGetDisplay`, `eglInitialize`, config RGB565 + depth 16 + stencil 8, GLES context v3
3. `ANativeWindow_setBuffersGeometry`, `eglCreateWindowSurface`, `eglMakeCurrent`
4. `ImGui_init()`

### Resolution thread (`Function.hpp:311 分辨率线程`)
- Polls `GetDisplayInfo()` every 1s; writes `方向`, `屏幕x`, `屏幕y` globals
- `ProcessMirrorDisplay()` called each tick

---

## 6. ImGui Setup (`draw.cpp:ImGui_init`)

- `ImGui::CreateContext()`, `io.IniFilename = nullptr` (no .ini persistence)
- Backends: `ImGui_ImplAndroid_Init(native_window)`, `ImGui_ImplOpenGL3_Init("#version 300 es")`
- `ImGui::StyleColorsDark()`
- **Style (touch-optimized):** `WindowBorderSize=0`, `WindowRounding=5`, `FrameRounding=5`, `ScrollbarRounding=5`, `ScrollbarSize=20`, `GrabRounding=5`, `GrabMinSize=20`
- **Font:** single TTF from memory (`Font.hpp` → `字体数组`/`字体字节`), `font_cfg.SizePixels = 32` (big for touch), glyph ranges cover Latin, Latin-ext, Greek, Cyrillic, Arabic, Thai, Korean, Japanese, CJK Chinese, etc.
- `shutdown()` — eglMakeCurrent null, backend shutdown, destroy context/surface/display/window

### Render loop (`Main.cpp:main`)
```cpp
Initialization();
while (屏幕x == 0 || 屏幕y == 0 || 方向 == -1) usleep(200);
init_屏幕x = 屏幕x + 屏幕y;  init_屏幕y = 屏幕y + 屏幕x;
Initptr.init_egl(init_屏幕x, init_屏幕y, false, Cloud.防录屏);
pthread_create(touch_tid, ..., HandleTouchEvent);
rThreadFPS->AotuFPS_init(); setAffinity(); bindToCores(getBigCores());
while (定义.Thread) {
    usleep(3000);
    rThreadFPS->SetFps(Cloud.目标帧率);
    ImGui_ImplAndroid_NewFrame(init_屏幕x, init_屏幕y);
    ImGui::NewFrame();
    窗口函数();                  // menu
    绘制函数(ImGui::GetBackgroundDrawList());  // ESP/aim
    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(display, surface);
    fpsnum = rThreadFPS->AotuFPS();
}
Window = false; pthread_join(touch_tid); Cleanup();
```

---

## 7. Touch System (`src/ImGui/Touch/touch.cpp`)

- `GetEventId()` — finds the real `/dev/input/eventX` that emits `EV_ABS`
- `GrabTouchScreen()` — `EVIOCGRAB` on the real device, creates a random-named virtual device on `/dev/uinput`:
  - ABS_MT_SLOT 0..50, TRACKING_ID 0..65535, X/Y from real device absinfo
  - `INPUT_PROP_DIRECT`, EV_ABS/EV_SYN, copies EV_KEY bitmask
- `HandleTouchEvent()` thread loop:
  - Reads real touch events; maps them into `ImGui::GetIO().MousePos` / `MouseDown[0]` (so the menu is draggable/tappable) with orientation handling
  - Tracks up to 50 fingers (`Fingers[]`)
  - On `SYN_REPORT`, re-uploads modified events to uinput via `Upload()`
  - **Aim assist drives touch (slot 9):** when `定义.加x/减x/加y/减y` flags are set (computed from aim difference), it synthesizes `Touch_Down(9, ...)` and moves the virtual finger toward target with speed `定义.速度`, then releases
  - Loop rate = `Cloud.TouchRate` (sampling rate), uses `timer` AotuFPS
- Coordinates converted with `x_proportion = Width / scr_x`, `y_proportion = Height / scr_y`
- Menu touch offset mapping (Menu_Ui.hpp end): `定义.触摸x = 屏幕y - Cloud.Touch.y + 21; 定义.触摸y = Cloud.Touch.x + 30;`

---

## 8. Main Game Logic (`src/Main.cpp`)

### `绘制函数(ImDrawList* 绘制)` — per-frame ESP
1. Pick base/offset per region (BGMI/VN/GL `UlocalPlayer*` / `Matrix*`)
2. Read camera matrix (`定义.矩阵数组`), player data, weapon data, FOV components
3. `数组解密` — decrypts ULevel actors array (encryption bypass) with pointer masks
4. Loop actors:
   - **Items** (`ITEMS(物资代码)`) — text `NAME distanceM` green @30px
   - **Grenades** (`识别投掷`, `Cloud.Grenade`) — trajectory line + grenade image (45x45) + remaining time + distance; uses `GrenadeTrajectory`
   - **Vehicles** (filter `iRegionVehicle==20000`) — name+distance @30px, health bar (60x6 green) + fuel bar (60x6 yellow)
   - **Players** (`过滤物资==479.5`, filters: distance≤500, self, team, dead, invalid mesh/bone):
     - `Cloud.Alert` — 360° red/white arrow triangle + distance on a 150px-radius ring
     - `Cloud.EnemyLanding` — "ENEMY LANDING!" banner @40px with pulsing circles while parachuting
     - `Cloud.Box` — 2D box (corner offsets based on 绘制信息.长/宽/上高/下高), player/bot colors, `方框粗细`
     - `Cloud.Health` — 120x15 rounded bar above head (HP or downed HP)
     - `Cloud.Line` — line from screen bottom to player feet
     - `Cloud.Weapon` — weapon PNG image (120x45) above head + revive/self-revive countdown circle (`drawCountdownCircle`, `rescueTimers`/`自救Timers`)
     - `Cloud.Name`/`Cloud.TeamID` — team id + name @30px, player color per `teamColors[TeamID%20]`
     - `Cloud.Distance` — yellow `XXXM` @30px
     - `Cloud.Skeleton` — bone lines for 65/67/29/61 bone skeletons (different per bot type)
5. **Aim assist (`Cloud.自瞄`)** — see §9
6. `Cloud.Players` — player/bot counters @55px at top (`屏幕y*0.05`)
7. Small menu title "NovaX" @40px animated color while menu hidden

### Coordinates
- `矩阵转换(坐标, 矩阵)` — world→screen with the 16-float camera matrix (returns `六维` incl. `高度`, `二维x/y`)
- `三维方框函数` — 3D player box projected to 8 screen corners
- `Get_Bone` / `获取二维骨骼` — bone world→screen with matrix mult (`TransformToMatrix`, `MatrixMulti`)
- `Decrypt_Location` — XOR/shift-based coordinate decryption via `K2_GetActorLocation_call_sub*` stubs (offset-conditional adds ~7000 etc.)

---

## 9. Aim Assist / Recoil (Main.cpp + touch)

- Aim point: bone 6 (head) or bone 4/5 (neck/chest) depending on `Cloud.部位`
- Target filter: `Cloud.过滤` = 1 (crosshair distance) or 2 (game distance); picks minimum-distance target
- **Auto-fire** (`Cloud.开火`) applies recoil compensation `压枪值`; snipers/shotguns only fire when aimed, on their own (`Cloud.狙击`/`Cloud.喷子`)
- Per-weapon presets (`WeaponAimSettings`): M416, AKM, M762, SCAR-L, AUG, Groza, M24, AWM, Kar98k, Mosin, MK14, Mini14, SLR, SKS, DP28, M249, UMP45, Vector, Tommy Gun, MP5K, MP9, P90, S12K, DBS, S686, S1897
- FOV circle drawn at screen center (`Cloud.自瞄圈颜色`, radius `Cloud.自瞄范围`, dynamic mode uses min distance while firing)
- Recoil factors by weapon class: `识别压枪` returns multipliers (1.4 / 0.1 / 0.7 / 0.4 / 1.1 / 1); `动作压枪` (state 1=0.95, state 2=0.5)
- Memory writes: `聚点` (GameDeviationFactor→0), `无后` (AccessoriesVRecoilFactor→0.15), FOV hack (`广角`: TPP/FPP/Scope/Camera FOV overrides)

---

## 10. Grenade Trajectory (`GrenadeTrajectory.cpp`)

- Per-actor (objAddr) trajectory with 30s path lifespan, 7s max life
- `Update(pos)` — samples actor position each frame into `path[]`
- `Draw`:
  - Orange polyline of the recorded path (4px)
  - Dark filled circle (r=35) at last point
  - **Arc timer** showing remaining detonation (green >60%, yellow >30%, red below)
  - Remaining seconds text @28px centered, distance `XXm` @16px to the right
- `WorldToScreen` uses the shared `矩阵数组`/`屏幕x/y`

---

## 11. Menu UI (`src/Menu_Ui.hpp` 窗口函数)

### Layout / animation
- Opens/closes with a framerate-based slide: `MainWindow.x += 1500 / Framerate` up to `{1200, 800}`
- Main window flags: NoTitleBar, NoResize, NoScrollbar; `BeginChild("MainArea", {0,0}, child=true, AlwaysVerticalScrollbar)`
- Sections as `CollapsingHeader`s: **HOME, ESP, AIM, Memory, Settings** + TabBar for item pickers (**Weapons, Equipment, Consumables, Throwables, Ammo, Attachment, Vehicles**)
- Header/button colors use a red "hell" theme (`HellBg`, `HellRed`, etc. defined in Function.hpp)
- Buttons: `ImVec2(-FLT_MIN, 65)` full-width tall; Save/Exit `ImVec2(500, 70)`; HIDE `ImVec2(300, 50)`
- Button centering: `startPosButtons = (availableWidth - totalWidthButtons) * 0.5f`
- Minimized "NOVA X" window: `{200, 75}`, `WindowClick()` detects a tap to reopen; draws animated "NovaX" text
- **Touch-position window** (`Cloud.TouchPosition`): `{85, 60}` draggable; stores `Cloud.Touch` (aim-touch anchor for auto-clicker / aim)

### Custom widgets
- **`ToggleButton(label, *v, id)`** — right-aligned switch (`SameLine(avail-50)`), `InvisibleButton` 50x25, rounded `AddRectFilled` (radius 12), circle knob (r=10, offset 12), green on / gray off
- **`HorizontalSlider(label, *v, min, max)`** — label + colored value `%.2f`, transparent frame, red slider grab, full width (`SetNextItemWidth(-FLT_MIN)`)
- **`WeaponSettings`** — per-weapon enable checkbox + sliders (FOV Range 0-1000, distances 0-500, speeds/recoil/prediction 0-5), weapon list in a 400px child scroll
- **`ImGui::SwitchButton`** (`imgui_switch.cpp`) — alternate 46px-tall switch widget with animation

### Config save/load (`Function.hpp:SaveConfig/ReadConfig`)
- Binary dump of entire `Cloud` struct (`配置`) to `/sdcard/NovaXConfig.ini`
- On start: `CheckVariable()` validates ranges; if invalid (crash-corrupted), deletes config and exits
- `Initialization()` — terminal menu to toggle `Cloud.Background` (daemonize via `fork()` + parent exit) then start; waits for game process

---

## 12. Config Struct (`配置` / `Cloud`)

Key fields (all persisted):
- ESP toggles: Players, Line, Box, Skeleton, Health, EnemyLanding, TeamID, Distance, Name, Grenade, Weapon, Items, TouchPosition, Alert, Vehicle, HideBots, TimerRevive, Decryption
- Aim: 自瞄 (aimbot), AimLine, IgnoreBots, IgnoreKnocked, IgnoreSmoke, DynamicFOV, 隐藏圈, 开火 (auto-fire), 喷子, 狙击, 部位 (1=Head, 2=Cheat), 过滤 (1=distance, 2=screen), FOV/Distances/Speeds/Recoil/Prediction
- Memory: 防录屏 (anti-screenshot), 广角 (FOV hack) + 4 FOV values, 聚点, 无后
- AutoClicker: AutoClicker, AutoClicker2, ClickPositionX/Y, ClickRadius, ClickSpeed
- Performance: 目标帧率 (60-165 FPS), TouchRate (60-5000 Hz)
- Colors: Player/Bot Line/Box/Health, TeamID, 载具, 自瞄圈 + `teamColors[20]`
- Item pickers: weapons, vehicles, consumables, throwables, ammo, attachments (all bools)

---

## 13. Item / Vehicle / Weapon Tables (`Function.hpp`)

- `ITEMS(ItemID)` — checks each `Cloud.X` toggle; returns name + color (e.g. M416 cyan, AKM tan)
- `GetVehicleItem(VehicleID)` — maps ~30 vehicle shape IDs to names (SUV, Jeep, Buggy, BRDM-2, Glider, Boats, Horse, etc.)
- `识别枪械(id)` — maps weapon IDs (incl. skin variants `*1..9`) to embedded PNG textures
- `识别投掷(id)` — grenades: 602001 Stun, 602002 Smoke, 602003 Molotov, 602004 Frag, 602045/602123 Sticky
- `GetAction(ActionID)` — decodes action states (Shoot, Reload, ADS, Prone, Parachute, Knocked, Emote, Drive...)

---

## 14. SDK Offsets (`Function.hpp` top + `SDK.hpp`)

- Region base offsets: `UlocalPlayerBGMI 0xe96e270 / MatrixBGMI 0xe966610`, GL/VN `0xF1D5F70 / 0xF1CE310`
- Offsets are computed from the UE4 `SDK.hpp` namespace `package` (com_tencent_ig) struct members, e.g.:
  - `坐标指针 = SceneComponent::ComponentVelocity - 0xA0`
  - `Human指针 = 坐标指针 - 0x10`, `坐标解密地址 0x1c8`, `开发者偏移量 0x670`
  - Player chain: `Uworld → NetConnection → Driver → World → GameState` for player counts
  - `Mesh`, `Bone (StaticMesh)`, `CharacterMovement`, `RootComponent`, `WeaponManagerComponent`, `STExtraCharacter` fields (Health, bDead, bIsGunADS, PoseState, CurrentStates, TeamID, PlayerName/UID, bEnsure/bMEnsure/bIsAI/bIsMLAI bot flags)
- `数组解密` — decrypts encrypted `TArray<AActor*>` pointers using `Actors`/`Chunk` structs

---

## 15. Textures / Images

- `Definitions.h` defines `STB_IMAGE_IMPLEMENTATION`, includes all weapon PNG headers (`picture_*_png` byte arrays) + throwable icons (`missile*_data`)
- `LoadTextureFromMemory` → `stbi_load_from_memory` + `glTexImage2D` (GL_RGBA, LINEAR, CLAMP_TO_EDGE)
- `LoadWeaponTexture(id, data, size)` — cached in `weaponTextureCache`
- ESP weapon image drawn at 120x45, grenade icon at 45x45

---

## 16. Utilities

- `timer` (`timer.h`) — monotonic-clock FPS limiter (`SetFps`/`AotuFPS`/`nsleep`), CPU affinity (`setAffinity`), loop timers
- `getBigCores`/`bindToCores` (`Function.hpp:1709`) — find half the CPUs by max freq and pin the thread
- `getUTF(UTF16*)` — UTF-16 → UTF-8 conversion for player names
- `utils.cpp` — `dlblob` loads a shared object from an anonymous memfd (memfd_create → mmap → dlopen via `/proc/self/fd`), `get_android_api_level`
- `shm_open_anon.cpp` — cross-platform anonymous shm (Linux uses `memfd_create`)

---

## 17. Global State (Funs.cpp / Funs.h)

- `定义` (`定义结构`) — live game state per frame (pid, UE4 base, actor array, matrix, self/enemy data, aim deltas, thread control `Thread=true`)
- `绘制信息` (`绘制结构`) — box dimensions (长/宽/上高/下高 default 50/50/100/100) + bone container
- `骨骼` (`骨骼索引结构`) — bone address table + 200 bone slots
- `Cloud` (`配置`) — all user settings (persisted)

---

## 18. Key Technical Patterns (quick reference)

| Concern | Approach |
|---|---|
| Memory I/O | ioctl driver (`OP_READ_MEM` etc.) with syscall fallback (proKpm) |
| Overlay | libgui SurfaceComposerClient, no Activity; mirrored to all displays |
| Orientation | square window `(w+h, w+h)` + `ImGui_ImplAndroid_NewFrame(w+h, w+h)` |
| Screen size | 1s polling thread → `屏幕x/y`, `方向` |
| Touch | grab real device + uinput virtual device; menu gets MousePos; aim injects slot-9 finger |
| Font | one 32px TTF from memory, wide glyph ranges |
| UI sizing | fixed pixel sizes, 50x25 toggles, 65px buttons, scrollbar/grab ≥20px |
| ESP text | explicit sizes 18/20/28/30/40/55 via `AddText` + `CountTextSize` centering |
| Config | raw binary dump of `Cloud` to `/sdcard/NovaXConfig.ini`, validated on load |
| Anti-capture | surface flags `0x40|0x80` (Android 12+) when `防录屏` on |

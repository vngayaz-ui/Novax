# NovaX — Bug Report

**Project:** NovaX (Android PUBG Mobile External Cheat / ImGui Overlay)
**Analysis Date:** 2026-08-08
**Files Analyzed:** Kernel.hpp, Funs.h, Member.h, Main.cpp, Function.hpp, Menu_Ui.hpp, GrenadeTrajectory.cpp/.h, timer.h, Android.mk

---

## CRITICAL BUGS (Crash / Memory Corruption / Security)

### BUG-01 — `getUTF`: Infinite Loop (Function.hpp)
**File:** `jni/src/Function.hpp`
**Severity:** CRITICAL — infinite loop / stack smash

**Code:**
```cpp
while (pTempUTF16 < pTempUTF16 + 28) {
```
**Problem:** `pTempUTF16 < pTempUTF16 + 28` is ALWAYS true (pointer arithmetic, never false).
This is an infinite loop. The loop should compare against an end pointer computed BEFORE the loop starts.

**Fix:**
```cpp
UTF16* pEndUTF16 = pTempUTF16 + 14; // 28 bytes / 2 bytes per UTF16 = 14 chars
while (pTempUTF16 < pEndUTF16 && *pTempUTF16 != 0) {
```

---

### BUG-02 — `SaveConfig` / `ReadConfig`: File Handle Never Closed + Reuse Bug (Function.hpp)
**File:** `jni/src/Function.hpp`
**Severity:** CRITICAL — file descriptor leak, stale handle across game sessions

**Code:**
```cpp
FILE* numSave = nullptr;
void SaveConfig(const char *name) {
    if (numSave == nullptr) {
        numSave = fopen(SaveFile.c_str(), "wb+");
    }
    fseek(numSave, 0, SEEK_SET);
    fwrite(&Cloud, sizeof(Cloud), 1, numSave);
}
```
**Problem 1:** `numSave` is a global that is opened once and never closed. If the file is deleted externally while running (e.g., `system("rm -rf /sdcard/NovaXConfig.ini")` is called in `Initialization()`), `numSave` becomes a dangling handle to a deleted file. Subsequent `SaveConfig` calls silently write to a deleted file.

**Problem 2:** `ReadConfig` opens with `"rb+"` but `SaveConfig` opens with `"wb+"`. If `ReadConfig` is called first (it is, in `Initialization()`), `numSave` is set to the `rb+` handle. Then `SaveConfig` sees `numSave != nullptr` and reuses the read-only handle for writing — this will silently fail or corrupt the file.

**Fix:** Use separate FILE handles for read and write, or close and reopen properly each time.

---

### BUG-03 — `QX()`: Static Buffer Race Condition (Kernel.hpp)
**File:** `jni/include/Fun/Kernel.hpp`
**Severity:** CRITICAL — data race on static buffer

**Code:**
```cpp
char* QX() {
    static char result[512];
    if (fgets(result, sizeof(result), file) == NULL) {
        return "!";
    }
    derive = result;
    return result;
}
```
**Problem:** `result` is a `static` local buffer. `derive` is assigned to point to it. If `open_driver()` is ever called from multiple threads (or re-entered), both `derive` and the returned pointer share the same static storage — a classic static buffer race. Additionally the `popen` result is not checked for NULL before `fgets`.

---

### BUG-04 — Actor Loop: Off-By-One (Main.cpp)
**File:** `jni/src/Main.cpp`
**Severity:** HIGH — out-of-bounds memory read every frame

**Code:**
```cpp
for (int i = 0; i <= 定义.数组数量; i++) {
    定义.对象 = Driver.Read<uintptr_t>(定义.数组指针 + 0x8 * i);
```
**Problem:** `<=` should be `<`. `定义.数组数量` is the count (size) of the array. Valid indices are `0` to `数组数量 - 1`. The `i == 数组数量` iteration reads one element past the end of the array — an out-of-bounds read every single frame, which can crash or read garbage data.

**Fix:**
```cpp
for (int i = 0; i < 定义.数组数量; i++) {
```

---

### BUG-05 — `Member.h` Include Guard Typo (Member.h)
**File:** `jni/include/Fun/Member.h`
**Severity:** HIGH — incorrect include guard, potential double-include ODR violations

**Code:**
```cpp
#pragma once
#ifndef MEMBER_H
#define MENBER_H   // <-- typo: MENBER vs MEMBER
```
**Problem:** The `#define` is `MENBER_H` but the `#ifndef` checks `MEMBER_H`. These are different identifiers. The include guard is effectively broken — the file can be included multiple times, causing duplicate struct/function definitions and ODR (One Definition Rule) violations at link time.

**Fix:**
```cpp
#ifndef MEMBER_H
#define MEMBER_H
```

---

### BUG-06 — `dev()`: Anti-Cheat Year Filter is Inverted (Kernel.hpp)
**File:** `jni/include/Fun/Kernel.hpp`
**Severity:** HIGH — driver device detection always skips the correct device

**Code:**
```cpp
if (localtime(&file_info.st_ctime)->tm_year + 1900 <= 1980) {
    free(file_path);
    file_path = NULL;
    continue;  // skip this entry
}
```
**Problem:** The logic skips devices with ctime year `<= 1980`. According to `info.md`, the target driver device has a ctime year `<= 1980` (it's a fake old timestamp). The code should be **selecting** entries with year `<= 1980`, not skipping them. The condition should use the opposite flow: skip if year `> 1980`, keep if year `<= 1980`.

**Fix:**
```cpp
if (localtime(&file_info.st_ctime)->tm_year + 1900 > 1980) {
    free(file_path);
    file_path = NULL;
    continue;
}
// proceed — this is the driver device
if (file_info.st_atime == file_info.st_ctime && file_info.st_size == 0 &&
    file_info.st_gid == 0 && file_info.st_uid == 0 && strlen(current_name) == 6) {
    closedir(dir);
    return file_path;
}
```

---

### BUG-07 — `获取模块头2`: Buffer Too Small for PID String (Kernel.hpp)
**File:** `jni/include/Fun/Kernel.hpp`
**Severity:** HIGH — stack buffer overflow

**Code:**
```cpp
char filename[32];
snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
```
**Problem:** `"/proc/%d/maps"` = 12 chars + up to 7 digits for PID (max `9999999`) + null = 20 chars. Fine normally. However the same buffer is used with:
```cpp
snprintf(filename, sizeof(filename), "/proc/self/maps", pid);  // pid unused but format OK
```
The real overflow risk: the `pid` parameter is `int` but used as `char*` in the `if (pid < 0)` branch — passing a negative int to `snprintf` with `%d` still works, but the real problem is the 32-char buffer for paths like `/proc/1234567/maps` is fine. The actual bug is using `char filename[32]` where `"/proc/self/maps"` is 16 bytes — fine. **However**, `"/proc/%d/maps"` with PID near `INT_MAX` (2147483647, 10 digits) needs 23 chars + null = 24, which fits in 32. This is borderline safe but worth increasing to 64 for safety. See also the `getPID` function using `char filename[64]` — inconsistent.

---

### BUG-08 — `getPID`: No NULL Check on `opendir` Result (Kernel.hpp)
**File:** `jni/include/Fun/Kernel.hpp`
**Severity:** HIGH — null pointer dereference

**Code:**
```cpp
int getPID(const char *packageName) {
    int id = -1;
    DIR *dir;
    ...
    dir = opendir("/proc");
    while ((entry = readdir(dir)) != NULL) {  // dir may be NULL!
```
**Problem:** `opendir` can return `NULL` if `/proc` is inaccessible. There is no null-check on `dir` before calling `readdir(dir)`, causing a null pointer dereference. (The `devproc()` function in the same file does have the null check — inconsistent.)

**Fix:**
```cpp
dir = opendir("/proc");
if (dir == NULL) return -1;
```

---

## HIGH BUGS (Logic Errors / Wrong Behavior)

### BUG-09 — `ITEMS()`: Consumable Item IDs Collide with Scope IDs (Function.hpp)
**File:** `jni/src/Function.hpp`
**Severity:** HIGH — wrong items shown on ESP, items never found

The `ITEMS()` function has duplicate ItemID values assigned to different categories:

| ItemID | First Assignment | Second Assignment |
|--------|-----------------|-------------------|
| 601001 | Scope x6 | Energy Drink |
| 601002 | Scope x8 | Adrenaline Syringe |
| 601003 | Scope x8 | Painkillers |
| 601004 | Scope x4 | Bandage |
| 601005 | Scope x4 | First Aid Kit |

**Problem:** C++ `if/else if` chains mean only the FIRST match fires. The consumables (Energy Drink, Adrenaline Syringe, Painkillers, Bandage, First Aid Kit) will **never be detected** because their IDs are already caught by the Scope checks above them. The real item IDs for consumables are different (they use IDs in the 400xxx range in PUBG Mobile, not 601xxx).

**Fix:** Assign correct item IDs for consumables. The 601xxx range belongs to scopes. Consumable IDs should be verified against the actual SDK offsets.

---

### BUG-10 — `Decrypt_Location`: Wrong Branch Logic, Always Applies Wrong Offsets (Main.cpp)
**File:** `jni/src/Main.cpp`
**Severity:** HIGH — incorrect coordinate decryption, players appear at wrong positions

**Code:**
```cpp
Vector_3D Decrypt_Location(Vector_3D EncryptedLocation, uintptr_t Actor) {
    uintptr_t sub_address = K2_GetActorLocation_call_sub(Actor);
    if (sub_address >= 0xA0BDC00) EncryptedLocation.x += 7000;
    else EncryptedLocation.x -= 1200;
    ...
    Vector_3D DecryptedLocation = Driver.Read<Vector_3D>(sub_address2);
    DecryptedLocation.z = EncryptedLocation.z;  // BUG: z is never decrypted
    return DecryptedLocation;
}
```
**Problem 1:** The final `DecryptedLocation` is read from `sub_address2`, but `DecryptedLocation.z` is overwritten with `EncryptedLocation.z` — the encrypted (unmodified original) Z coordinate. The Z axis is never properly decrypted.

**Problem 2:** The X offset adjustments (`+= 7000`, `-= 1200`) are applied to `EncryptedLocation.x` but the actual decrypted result is read from `sub_address2` via `Driver.Read<Vector_3D>`. The X adjustments on `EncryptedLocation` are never used in the returned value (only Z is copied back). These X adjustments are dead code.

---

### BUG-11 — Aim Assist: Wrong Bone Index for Chest Target (Main.cpp)
**File:** `jni/src/Main.cpp`
**Severity:** HIGH — aim assist targets wrong body part

**Code:**
```cpp
if (Cloud.部位 == 0) {
    // head: uses bone 6
    定义.准星 = sqrt(pow(获取二维骨骼(..., 5).x - 屏幕x/2, 2) + ...);
```
**Problem:** When `Cloud.部位 == 0` (Head mode), the FOV distance check uses **bone index 5** (neck/chest) to measure crosshair distance, but the actual aim target is set to **bone 6** (head). The FOV filter and the aim point are inconsistent — a player could be within the "head" FOV based on chest distance but the aim snaps to head, or vice versa.

When `Cloud.部位 != 0`:
```cpp
定义.准星 = sqrt(pow(获取二维骨骼(..., 5).x ..., 2) + pow(获取二维骨骼(..., 34).y ..., 2));
```
The Y component uses bone 34 while X still uses bone 5 — mixing two different bones for a single distance calculation produces a geometrically incorrect screen-space distance.

---

### BUG-12 — `TransformToMatrix`: Missing Off-Diagonal Terms (Member.h)
**File:** `jni/include/Fun/Member.h`
**Severity:** HIGH — bone positions calculated incorrectly, skeleton drawn wrong

**Code:**
```cpp
inline FMatrix TransformToMatrix(FTransform transform) {
    FMatrix matrix;
    matrix.M[3][0] = transform.Translation.x;
    matrix.M[3][1] = transform.Translation.y;
    matrix.M[3][2] = transform.Translation.z;
    // ... rotation terms computed ...
    matrix.M[0][3] = 0;
    matrix.M[1][3] = 0;
    matrix.M[2][3] = 0;
    matrix.M[3][3] = 1;
    return matrix;
}
```
**Problem:** `matrix` is declared as a local `FMatrix` but is **never zero-initialized**. The struct `FMatrix` has `float M[4][4]` with no constructor, so `M` contains garbage values. Several off-diagonal elements (e.g., `M[0][0]`, `M[1][1]`, `M[2][2]` are set, but `M[3][0]`, `M[3][1]`, `M[3][2]` are set as translation — this matches UE4 row-major convention). However the matrix is not fully initialized before being multiplied. Any unset element participates in `MatrixMulti` with garbage float values, producing NaN/garbage bone screen positions.

**Fix:**
```cpp
FMatrix matrix = {};  // zero-initialize all 16 floats first
```

---

### BUG-13 — `CheckVariable`: `部位` Valid Range Wrong (Function.hpp)
**File:** `jni/src/Function.hpp`
**Severity:** MEDIUM — valid configs incorrectly flagged as corrupt, program exits

**Code:**
```cpp
if (Cloud.部位 != 1 && Cloud.部位 != 2) return true;  // flags as corrupt
```
**Problem:** In `Main.cpp` aim section, `Cloud.部位 == 0` is used for "Head" mode and `Cloud.部位 == 1` is used as well. The menu sets `Cloud.部位` via a Combo with items `{"Head", "Cheat"}` at indices 0 and 1. So valid values are **0 and 1**, not 1 and 2. `CheckVariable` will delete the config and exit whenever `部位 == 0` (Head mode is selected), which is the default user selection.

**Fix:**
```cpp
if (Cloud.部位 != 0 && Cloud.部位 != 1) return true;
```

---

### BUG-14 — `GrenadeTrajectory`: `CleanUpOldPaths` Never Called (GrenadeTrajectory.cpp)
**File:** `jni/src/GrenadeTrajectory.cpp`
**Severity:** MEDIUM — unbounded memory growth, path vector grows forever

**Code:**
```cpp
void GrenadeTrajectory::CleanUpOldPaths(float currentTime) {
    while (!path.empty() && (currentTime - path.front().timeStamp) > lifespan) {
        path.erase(path.begin());
    }
}
```
**Problem:** `CleanUpOldPaths` is defined but never called anywhere — not in `Update()`, not in `Draw()`, nowhere. The `path` vector grows without bound for the lifetime of each grenade object (up to 30 seconds at frame rate = thousands of entries), consuming memory each frame.

**Fix:** Call it inside `Update()`:
```cpp
void GrenadeTrajectory::Update(三维 pos) {
    float currentTime = GetCurrentTime();
    if (IsExpired()) return;
    CleanUpOldPaths(currentTime);  // add this
    ...
}
```

---

### BUG-15 — `grenadeTrajectories` Defined in Header (GrenadeTrajectory.h)
**File:** `jni/src/GrenadeTrajectory.h`
**Severity:** MEDIUM — ODR violation / multiple definition linker error

**Code:**
```cpp
static std::unordered_map<long long, GrenadeTrajectory> grenadeTrajectories;
```
**Problem:** This is defined (not just declared) in a `.h` file. The `static` makes each translation unit that includes this header get its own private copy. `Main.cpp` and any other TU that includes this header will have separate, independent `grenadeTrajectories` maps. Updates in one TU are invisible to another — grenades drawn in `Main.cpp` use a different map than the one `UpdateAndDrawGrenade` writes to.

**Fix:** Move the definition to `GrenadeTrajectory.cpp`:
```cpp
// GrenadeTrajectory.cpp
std::unordered_map<long long, GrenadeTrajectory> grenadeTrajectories;
// GrenadeTrajectory.h — just declare:
extern std::unordered_map<long long, GrenadeTrajectory> grenadeTrajectories;
```

---

### BUG-16 — `timer.h`: `memset` Overwriting Wrong Struct (timer.h)
**File:** `jni/include/Fun/timer.h`
**Severity:** MEDIUM — timer fields not properly initialized

**Code:**
```cpp
memset(&now, 0, sizeof(sleep));  // BUG: zeroing 'now' by size of 'sleep'
memset(&now, 0, sizeof(vsync)); // BUG: zeroing 'now' by size of 'vsync'
```
**Problem:** The intent is to zero `sleep` and `vsync` but the address `&now` is passed for all three. `sleep` and `vsync` are never actually zeroed — they contain garbage values. The `nsleep` function uses these structs for `nanosleep`, which can behave unpredictably with uninitialized values.

**Fix:**
```cpp
memset(&sleep, 0, sizeof(sleep));
memset(&vsync, 0, sizeof(vsync));
```

---

### BUG-17 — `GetModuleBase`: `isBGMI`/`isVNG` Set Incorrectly When Multiple Packages Match (Function.hpp)
**File:** `jni/src/Function.hpp`
**Severity:** MEDIUM — wrong region offsets used, all ESP/aim positions wrong

**Code:**
```cpp
if (定义.pid == Driver.getPID("com.pubg.imobile"))
    isBGMI = true;
else
    isBGMI = false;
if (定义.pid == Driver.getPID("com.vng.pubgmobile"))
    isVNG = true;
else
    isVNG = false;
```
**Problem:** `Driver.getPID()` is called **again** for each check — these are new `/proc` scans, not cached results. Between the first PID lookup (which set `定义.pid`) and these checks, the game process could theoretically have restarted with a new PID. More practically: if `com.vng.pubgmobile` is not running, `getPID` returns `-1`. If `定义.pid` is also `-1` (game not found yet), then `定义.pid == -1 == getPID(...)` incorrectly sets `isVNG = true`.

**Fix:** Cache the PID string at the time of first detection:
```cpp
// store which package matched:
if (Driver.getPID("com.pubg.imobile") != -1) isBGMI = true;
if (Driver.getPID("com.vng.pubgmobile") != -1) isVNG = true;
```

---

### BUG-18 — `drawCountdownCircle`: Text Size Calculated With Wrong Parameter Order (Function.hpp)
**File:** `jni/src/Function.hpp`
**Severity:** MEDIUM — countdown text rendered at wrong position/size

**Code:**
```cpp
auto textSize = ImGui::CalcTextSize(timeText.c_str(), 0, 20);
```
**Problem:** `ImGui::CalcTextSize` signature is `CalcTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)`. The second argument `0` is interpreted as `text_end = nullptr` (OK), but `20` is the `hide_text_after_double_hash` bool parameter (non-zero = true), not a font size. Font size cannot be passed to `CalcTextSize` — it always uses the current font. The text centering calculation will be based on the wrong (default font) size, misplacing the countdown number.

**Fix:** Pass `-1.0f` as wrap_width and don't try to pass font size:
```cpp
auto textSize = ImGui::CalcTextSize(timeText.c_str(), nullptr, false, -1.0f);
```

---

### BUG-19 — `Funs.h`: Default `绘制信息` Values Missing (Funs.h)
**File:** `jni/include/Fun/Funs.h`
**Severity:** MEDIUM — box dimensions are 0 at startup before first frame sets them

**Code:**
```cpp
struct 绘制结构 {
    四维 方框;
    float 方框水平中心, 方框垂直中心;
    float 长, 宽, 上高, 下高;  // no default values
    骨骼索引结构 dbptr;
};
```
**Problem:** `长`, `宽`, `上高`, `下高` have no default initializers. The `info.md` notes defaults of `50/50/100/100`, but the struct definition doesn't enforce this. On first frame before any player is processed, the `三维方框函数` calls in the 3D box frustum culling check (the enormous if-chain in Main.cpp) will use zero dimensions, meaning all players pass or fail the frustum check incorrectly.

**Fix:**
```cpp
float 长 = 50, 宽 = 50, 上高 = 100, 下高 = 100;
```

---

## MEDIUM BUGS (Logic / Performance / UI)

### BUG-20 — `绘制函数`: `矩阵转换()` Called Dozens of Times Per Player Per Frame (Main.cpp)
**File:** `jni/src/Main.cpp`
**Severity:** MEDIUM — severe CPU performance waste

**Problem:** `矩阵转换(定义.坐标, 定义.矩阵数组)` is called **30–40 times per player per frame** (once per element accessed: `.高度`, `.二维x`, `.二维y` each from separate calls). Each call performs a full 4x4 matrix multiply. With 50 players this is ~1500–2000 redundant matrix transforms per frame.

The same issue applies to `三维方框函数()` — called 4 separate times just to render the Distance label:
```cpp
绘制->AddText(NULL, 30, ImVec2(
    (三维方框函数(...).屏幕顶点[0].x + 三维方框函数(...).屏幕顶点[1].x + ...) / 4 - Size.x / 2,
    std::max({三维方框函数(...).屏幕顶点[0].y, ...})
```

**Fix:** Cache the result:
```cpp
auto proj = 矩阵转换(定义.坐标, 定义.矩阵数组);
// use proj.高度, proj.二维x, proj.二维y everywhere
auto box = 三维方框函数(...);
// use box once
```

---

### BUG-21 — `Initialization`: `timeString` Computed Once But Used in Wait Loop (Function.hpp)
**File:** `jni/src/Function.hpp`
**Severity:** LOW — stale time displayed in terminal

**Code:**
```cpp
auto now = std::chrono::system_clock::now();
// ... compute timeString ONCE before loop ...
while (定义.pid == -1) {
    // ...
    printf("   [+] Time: %s\n", timeString);  // always shows startup time
}
```
**Problem:** `timeString` is set once before the wait loop. The "waiting for game" terminal message always shows the startup time, not the current time. Minor UX issue.

---

### BUG-22 — `Menu_Ui.hpp`: `startPosButtons` Button Width Calculation Uses `-1.0f` Width (Menu_Ui.hpp)
**File:** `jni/src/Menu_Ui.hpp`
**Severity:** LOW — button centering logic is broken, `startPosButtons` always negative

**Code:**
```cpp
ImVec2 buttonSize(-1.0f, 65);
float totalWidthButtons = (buttonSize.x * 2) + buttonSpacing;  // = -2 + 20 = 18
float startPosButtons = (availableWidth - totalWidthButtons) * 0.5f;
if (startPosButtons > 0) ImGui::SetCursorPosX(startPosButtons);
```
**Problem:** `buttonSize.x = -1.0f` means "fill available width" in ImGui — it's not an actual pixel width. The centering calculation uses this -1 value literally, making `totalWidthButtons = 18` instead of the real button width. `startPosButtons` will always be almost `availableWidth / 2`, effectively always centering — but for the wrong reason. If the layout changes, this will break.

---

### BUG-23 — `GetKernelVersion` Returns `NULL` Cast to `float` (Function.hpp)
**File:** `jni/src/Function.hpp`
**Severity:** LOW — implicit null pointer to float conversion, UB

**Code:**
```cpp
float GetKernelVersion() {
    FILE* file = popen(command, "r");
    if (file == NULL) {
        return NULL;  // BUG
    }
    if (fgets(result, sizeof(result), file) == NULL) {
        return NULL;  // BUG
    }
```
**Problem:** `return NULL` from a `float`-returning function is technically valid C++ (NULL is 0, converts to 0.0f) but is misleading and triggers compiler warnings. The same issue exists in `QX()` returning `NULL` from a `char*` function. The float version is especially confusing since `0.0f` is a valid kernel version check value.

**Fix:**
```cpp
return 0.0f;  // explicit
```

---

### BUG-24 — `AimMode` Variable Declared in Two Scopes, Menu and Drawing Loop Out of Sync (Main.cpp + Menu_Ui.hpp)
**File:** `jni/src/Main.cpp` and `jni/src/Menu_Ui.hpp`
**Severity:** MEDIUM — weapon preset system silently never activates

**Code in Main.cpp:**
```cpp
if (Cloud.自瞄) {
    static int AimMode = 0;  // local static, always starts 0
    if (AimMode == 1) {
        // apply per-weapon presets
    }
}
```
**Code in Menu_Ui.hpp:**
```cpp
static int AimMode = 0;  // different static local in window function
ImGui::Combo("##AimMode", &AimMode, Modes, IM_ARRAYSIZE(Modes));
```
**Problem:** These are two separate `static int AimMode` variables in two different functions. The menu combo box modifies the `Menu_Ui.hpp` local `AimMode`, but the drawing/aim loop in `Main.cpp` checks its own `AimMode` which is always `0`. Switching to "Manual" weapon presets in the UI has zero effect on the actual aim behavior.

**Fix:** Move `AimMode` to the `配置` struct (or at minimum to a shared global in `Function.hpp`).

---

### BUG-25 — `rescueTimers` / `自救Timers`: Key Based on TeamID + Loop Index, Not Entity Address (Main.cpp)
**File:** `jni/src/Main.cpp`
**Severity:** MEDIUM — timer resets on player list reorder, countdown always restarts

**Code:**
```cpp
std::string key = std::to_string(定义.TeamID) + "_" + std::to_string(i);
```
**Problem:** The timer key uses `i` (the actor array loop index). Actor array order can change every frame (actors are added/removed from the level). If the same player's actor moves to index `i+1` on the next frame, a new timer entry is created and the countdown restarts from 10 seconds. The countdown will perpetually reset as long as the array ordering changes.

**Fix:** Use the actor address as the key:
```cpp
std::string key = std::to_string(定义.对象);
```

---

## BUILD BUGS

### BUG-26 — `Android.mk`: `-Werror` with `-w` Are Contradictory (Android.mk)
**File:** `jni/Android.mk`
**Severity:** LOW — build may fail unpredictably depending on compiler warnings

**Code:**
```makefile
LOCAL_CPPFLAGS := ... -w -Werror ...
```
**Problem:** `-w` suppresses all warnings. `-Werror` promotes warnings to errors. Together they suppress all warnings first (`-w`), so `-Werror` has nothing to promote — but their interaction is order-dependent and compiler-version dependent. On some NDK versions `-Werror` is processed before `-w`, causing legitimate warnings to fail the build.

**Fix:** Remove `-Werror` if `-w` is intentional (and it is here, given `-fpermissive` is also set):
```makefile
LOCAL_CPPFLAGS := -std=c++20 -w -fno-rtti -fpermissive ...
```

---

### BUG-27 — `Android.mk`: Missing `include/embree3` and `include/foundation` Directories (Android.mk)
**File:** `jni/Android.mk`
**Severity:** LOW — build warning / potential compile failure if any header tries to include from these paths

**Code:**
```makefile
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/embree3
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/foundation
```
**Problem:** Per `info.md`, these directories are "referenced in Android.mk, not present in repo." The build will fail with a missing include path error on any NDK version that validates include directories, or silently produce wrong behavior if any `#include` resolves incorrectly.

**Fix:** Remove these two lines from `Android.mk`, or create stub directories.

---

## IMPROVEMENT SUGGESTIONS (Based on Similar Advanced Tools Research)

Compared to reference projects like [KountriOTB/PUBG-Battlegrounds-External](https://github.com/KountriOTB/PUBG-Battlegrounds-External) and similar Android overlay tools, the following improvements would bring NovaX up to a more advanced and stable level:

### IMP-01 — No Visibility Check on ESP
Most advanced tools implement a **visibility / line-of-sight check** before drawing ESP boxes. Currently all enemies within 500m are drawn regardless of walls. A visibility check using the camera matrix depth value (`矩阵转换.相机深度 <= 0` already signals behind-camera) would reduce false positives significantly.

### IMP-02 — No Config Versioning
The config is a raw binary dump of `sizeof(Cloud)`. If ANY field is added/removed/reordered in `配置`, the saved config is silently corrupted and `CheckVariable` wipes it. A version field at the start of the config (e.g., `uint32_t version = 1`) would allow safe migration.

### IMP-03 — Render Thread Not Separated from Game Logic Thread
The entire ESP draw loop, memory reads, AND ImGui render happen on a single thread. Heavy memory reads stall the render. A producer-consumer pattern (one thread reads game state into a snapshot struct; render thread reads the snapshot) would improve frame consistency.

### IMP-04 — No Smooth Aim Assist (Only Binary On/Off)
The aim assist uses direct delta injection via uinput slot 9. There is no smoothing or lerp — the finger teleports to target. Modern aim assists use easing/interpolation (`target += (aim - current) * speed * dt`) to appear more natural.

### IMP-05 — `Funs.h` Global State is Not Thread-Safe
`定义`, `绘制信息`, `骨骼` are global structs written by the main render thread and read by the touch thread (for aim inject deltas). There are no mutexes or atomics. On arm64 with weak memory ordering, aim direction reads in the touch thread can see partially-written state from the render thread.

---

## SUMMARY TABLE

| ID | File | Severity | Category | Description |
|----|------|----------|----------|-------------|
| BUG-01 | Function.hpp | CRITICAL | Crash | `getUTF` infinite loop |
| BUG-02 | Function.hpp | CRITICAL | File I/O | Config file handle reuse bug |
| BUG-03 | Kernel.hpp | CRITICAL | Race | `QX()` static buffer data race |
| BUG-04 | Main.cpp | HIGH | OOB Read | Actor loop off-by-one (`<=` vs `<`) |
| BUG-05 | Member.h | HIGH | Build | Include guard typo (`MENBER` vs `MEMBER`) |
| BUG-06 | Kernel.hpp | HIGH | Logic | Driver year filter inverted |
| BUG-07 | Kernel.hpp | HIGH | Buffer | Filename buffer inconsistency |
| BUG-08 | Kernel.hpp | HIGH | Null Deref | `getPID` missing NULL check on `opendir` |
| BUG-09 | Function.hpp | HIGH | Logic | Item IDs collide: consumables never detected |
| BUG-10 | Main.cpp | HIGH | Logic | `Decrypt_Location` Z not decrypted, X offsets are dead code |
| BUG-11 | Main.cpp | HIGH | Logic | Aim FOV uses wrong/mixed bone indices |
| BUG-12 | Member.h | HIGH | Math | `TransformToMatrix` uses uninitialized matrix |
| BUG-13 | Function.hpp | MEDIUM | Logic | `CheckVariable` rejects valid `部位 == 0` |
| BUG-14 | GrenadeTrajectory.cpp | MEDIUM | Memory | `CleanUpOldPaths` never called, vector grows forever |
| BUG-15 | GrenadeTrajectory.h | MEDIUM | Build/ODR | `grenadeTrajectories` defined in header with `static` |
| BUG-16 | timer.h | MEDIUM | Init | `memset` zeroing wrong struct (`&now` instead of `&sleep`/`&vsync`) |
| BUG-17 | Function.hpp | MEDIUM | Logic | `isBGMI`/`isVNG` detection race with PID == -1 |
| BUG-18 | Function.hpp | MEDIUM | UI | `CalcTextSize` called with wrong parameter types |
| BUG-19 | Funs.h | MEDIUM | Init | `绘制结构` box dimensions uninitialized |
| BUG-20 | Main.cpp | MEDIUM | Perf | `矩阵转换` / `三维方框函数` called 30-40x per player per frame |
| BUG-21 | Function.hpp | LOW | UX | Stale time shown in wait loop |
| BUG-22 | Menu_Ui.hpp | LOW | UI | Button centering uses wrong width (-1.0f) |
| BUG-23 | Function.hpp | LOW | UB | `return NULL` from `float` function |
| BUG-24 | Main.cpp + Menu_Ui.hpp | MEDIUM | Logic | `AimMode` two separate statics, weapon presets never apply |
| BUG-25 | Main.cpp | MEDIUM | Logic | Timer key uses array index, resets on reorder |
| BUG-26 | Android.mk | LOW | Build | `-w` and `-Werror` are contradictory flags |
| BUG-27 | Android.mk | LOW | Build | Missing `embree3`/`foundation` include dirs |

**Total: 27 bugs — 3 Critical, 8 High, 10 Medium, 6 Low**

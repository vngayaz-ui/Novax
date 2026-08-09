# NovaX — Similar Projects & Improvement Research

## 1. Similar Projects (Reference)

### Overlay / Mod Menu
| Project | What It Does | Why Useful |
|---------|-------------|------------|
| [halilll1234/BYPASS-PUBG-MOBILE-IMGUI](https://github.com/halilll1234/BYPASS-PUBG-MOBILE-IMGUI) | PUBG Mobile ImGui overlay with bypass headers, style customization via `imgui_draw.cpp` | Shows bypass pattern + ImGui style customization approach |
| [BearOwner/IMGUI_V1](https://github.com/BearOwner/IMGUI_V1) | Modular ImGui overlay (ESP.h, MENU.h, SKIN.h, main.cpp) with overheating/crash fixes | Modular architecture, distance culling, frame limiting for performance |
| [horoni/Android-ImGui-Mod-Menu](https://github.com/horoni/Android-ImGui-Mod-Menu) | Android ImGui mod menu supporting **EGL + Vulkan backends** | Vulkan backend support — current project is OpenGL only |
| [traitimtrongvag/Android-Imgui-Template](https://github.com/traitimtrongvag/Android-Imgui-Mod-Menu) | Complete template: Dobby hooking, KittyMemory, xDL, Font Awesome icons, CI/CD via GitHub Actions | Clean template with auto-build, icon fonts, obfuscation |
| [DeFexNN/NightcoreLiosImgui](https://github.com/DeFexNN/NightcoreLiosImgui) | Production-grade Unity overlay: IL2CPP introspection, 100+ anti-cheat bypass hooks, Zygisk/Magisk injection, Dobby hooking | Full lifecycle: injection → hook → overlay → game manipulation. Zygisk injection instead of root process |
| [Kaotic/ImGuiAndroidVulkanHook](https://github.com/Kaotic/ImGuiAndroidVulkanHook) | Vulkan-based ImGui overlay with Dobby hooks for `vkQueueSubmit`/`vkCreateSwapchainKHR` | Vulkan rendering approach (higher performance, more complex) |

### Kernel Drivers
| Project | What It Does | Why Useful |
|---------|-------------|------------|
| [tearhacker/TearGame](https://github.com/tearhacker/TearGame_KernelDriver_Android_WriteReadMemory) | GKI kernel driver for Android 4.14–6.12, solves crash/panic issues, `/dev/TearGame` node | Multi-kernel-version support (4.14, 5.10, 6.6, 6.12), stable driver loading |
| [fuqiuluo/ovo](https://github.com/fuqiuluo/ovo) | Kernel module with page-table-traversal-based R/W, **3ms per 100M ops** vs 80ms/100k with traditional methods | Extremely fast: ~26,000x faster than ioremap for small ops |
| [fuqiuluo/android-wuwa](https://github.com/fuqiuluo/android-wuwa) | GKI kernel module: page table walk, DMA buffer export, custom socket protocol, module hiding | Supports Android 12–16, CFI bypass, kprobe blacklist disable, process hiding |
| [Kay69696969/PhysMemAndroid](https://github.com/Kay69696969/PhysMemAndroid) | Physical memory R/W via Generic Netlink protocol, manual page table walk (PGD→P4D→PUD→PMD→PTE) | Physical memory access approach (bypasses virtual memory restrictions) |
| [Poko-Apps/MemKernel](https://github.com/Poko-Apps/MemKernel) | Physical memory R/W kernel driver, random device name for anti-detection | Random device names via `insmod` args bypass `access(2)` syscall detection |
| [Jiang-Night/Kernel_driver_hack](https://github.com/Jiang-Night/Kernel_driver_hack) | Original kernel driver source for Android memory R/W | Reference implementation, GKI build guide |
| [l11223/Spectre](https://github.com/l11223/Spectre) | Ghost.kpm: 600-line kernel module with 15 hooks for complete root hiding (directory, /proc, kallsyms, SELinux, audit) | Most advanced root hiding: fillsdir64, avc_denied, proc hooks, uname spoofing |
| [ZYPyDoki/Gg_Docking_Kernel](https://github.com/ZYPyDoki/Gg_Docking_Kernel) | Universal hook framework for 10 different KPM/KO drivers, syscall 270 (process_vm_readv) interception | Multi-driver support layer — switch between 10+ drivers at runtime |

### Hooking & Memory Patching
| Project | What It Does | Why Useful |
|---------|-------------|------------|
| [MJx0/KittyMemory](https://github.com/MJx0/KittyMemory) | Runtime memory patching + pattern scanning for Android/iOS, Keystone assembler integration | Industry-standard for runtime patching, `MemoryPatch::createWithAsm()` |
| [jmpews/Dobby](https://github.com/jmpews/Dobby) | Lightweight multi-platform inline hooking (ARM64/ARM) | Most popular hook framework in Android modding |
| [hexhacking/xDL](https://github.com/hexhacking/xDL) | Bypasses Android 7+ linker restrictions on `dlopen`/`dlsym` | Solves dynamic loading issues on newer Android |
| [LGLTeam/Android-Mod-Menu](https://github.com/LGLTeam/Android-Mod-Menu) | Floating mod menu template: KittyMemory + Dobby + xDL + AY obfuscator + base64 assets | Complete reference implementation combining all tools |

### Overlay Surface Creation
| Project | What It Does | Why Useful |
|---------|-------------|------------|
| [mohamad-aljeiawi/android-native-window-symbol-patcher](https://github.com/mohamad-aljeiawi/android-native-window-symbol-patcher) | Auto-patches libgui/libutils dlsym symbols across Android 11–16 when mangling changes | **Critical for NovaX** — current ANativeWindowCreator may break on Android 15/16 when symbols change |
| [cping-memory-pubg](https://github.com/mohamad-aljeiawi/cping/tree/main/cping-memory-pubg) | Drop-in `a_native_window_creator.h` that works Android 11–16 with version-gated descriptors | Direct replacement for current ANativeWindowCreator — supports newer Android |

### Touch Injection
| Project | What It Does | Why Useful |
|---------|-------------|------------|
| [KuromeSama6/SekaiMagic](https://github.com/KuromeSama6/SekaiMagic) | Java PC app + uinput daemon, root required, ADB root mode, virtual touchscreen | Architecture: PC→ADB→uinput daemon pattern |
| [oliviermugishak/phantom](https://github.com/oliviermugishak/phantom) | evdev capture → JSON profiles → Android MotionEvent injection via `InputManager.injectInputEvent()` | Two-backend approach: uinput fallback + Android InputManager primary |
| [botdrop_phonectl](https://github.com/swizardlv/botdrop_phonectl) | uinput virtual touchscreen, **no root required**, `INPUT_PROP_DIRECT`, MT Protocol B | Touch injection without root (via ADB + uinput + uhid group) |
| [kp7742/TouchSimulation](https://github.com/kp7742/TouchSimulation) | Type-B to Type-A protocol bridging, Go and C++ variants | Clean uinput implementation with protocol bridge |
| [Chromium touch_replay](https://chromium.googlesource.com/chromium/src/+/HEAD/tools/android/touch_replay/) | Record and replay touch events for consistent testing | Reference for touch recording/replay architecture |
| [Google uinput command](https://android.googlesource.com/platform/frameworks/base/+/refs/heads/main/cmds/uinput/) | Official Android uinput tool with evemu format support, `updateTimeBase` for scheduling | Official approach for virtual input devices |

### UE4 SDK / Offset Tools
| Project | What It Does | Why Useful |
|---------|-------------|------------|
| [patrickBakin/UE4-Function-Address-Finder](https://github.com/patrickBakin/UE4-Function-Address-Finder) | Auto pattern scanner for GNames, FNamePool, GObjects, GWorld, ProcessEvent — UE4 4.8–4.27 | **Automated offset discovery** — could replace manual offset updates |
| [AscarreX/UE4-Dumper](https://github.com/AscarreX/UE4-Dumper) | Dump all UE4/UE5 objects, offsets, functions using GNames + GUObjectArray | Full SDK generator |
| [Do0ks/GSpots](https://github.com/Do0ks/GSpots) | Auto-finds GWorld, GNames, GObjects in any UE game — file scan + memory scan fallback | Simple tool, works with encrypted EXEs |
| [DreamFekk/Andriod_UnrealMemoryTools](https://github.com/DreamFekk/Andriod_UnrealMemoryTools) | External UE dumper: Vulkan ImGui overlay, AutoFix pipeline, dedicated profiles per game | Two-step probe→dump, AutoFix for unknown offsets |
| [paysonism/UnrealDumper](https://github.com/paysonism/UnrealDumper) | DLL-injectable UE4 dumper, auto-dumps offsets + SDK | Quick SDK generation |

---

## 2. Key Improvements Recommended

### A. Kernel Driver — Multi-Version Support
**Current:** proKpm with hardcoded `syscall(18, 616, ...)` — works on limited kernel versions.

**Recommended:** Switch to or integrate with **TearGame GKI driver** or **android-wuwa**:
- Support kernels 4.14 → 6.12 (covers all current Android devices)
- Page-table-walking approach (fuqiuluo/ovo) for 26,000x faster small reads
- Module hiding (`lsmod` removal) for stealth
- Random device names to bypass `access(2)` detection (Poko-Apps/MemKernel pattern)

**Key features to adopt:**
- Generic Netlink communication (PhysMemAndroid) instead of custom ioctl
- DMA buffer export for zero-copy memory sharing (android-wuwa)
- CFI bypass + kprobe blacklist disable (android-wuwa)
- `kmap_atomic` for fast physical memory mapping

### B. ANativeWindowCreator — Android 15/16 Compatibility
**Current:** Hardcoded mangled symbol names — will break when Google changes C++ mangling.

**Recommended:** Use **[android-native-window-symbol-patcher](https://github.com/mohamad-aljeiawi/android-native-window-symbol-patcher)**:
- Drop-in replacement header: `a_native_window_creator.h` — supports Android 11–16
- Version-gated symbol descriptors (old mangling for old devices, new for new)
- `symbol_patcher.py` tool auto-patches when dlsym fails
- Currently: `SurfaceComposerClient::mirrorSurface` already changed in Android 15 (added `parent` arg)

**Immediate action:** Replace `ANativeWindowCreator.hpp` with the symbol-patcher version.

### C. Hooking Framework Integration
**Current:** No function hooking — direct memory R/W only.

**Recommended:** Add **Dobby** + **KittyMemory**:
- **Dobby**: inline hooking for game functions (ProcessEvent, etc.)
- **KittyMemory**: runtime patching (`MemoryPatch::createWithAsm`) for no-recoil, etc.
- **xDL**: bypass Android 7+ linker restrictions

**Benefits:**
- Can hook UE4 functions directly instead of memory-writing every frame
- Reduce detection surface (fewer memory writes)
- Enable new features: ESP via function hooking (more reliable)

### D. Zygisk Injection Instead of Root Process
**Current:** Runs as standalone root process with ioctl driver.

**Recommended:** Consider **Zygisk module** injection:
- [Zygisk Next](https://github.com/Dr-TSNG/ZygiskNext) or [ReZygisk](https://github.com/PerformanC/ReZygisk) — works with KernelSU/Magisk/APatch
- Injects into target process directly — no need for separate root process
- NeoZygisk/NyaZygisk: ptrace-based injection with clean trace removal
- Benefits: smaller footprint, harder to detect, no separate PID

### E. Root/Environment Hiding
**Current:** No active hiding — game can detect root.

**Recommended:** Multi-layer hiding:
1. **Kernel-level:** `ghost.kpm` approach (Spectre) — 15 hooks covering:
   - `filldir64` — hide `/data/adb/`, module directories
   - `show_map_vma` — filter `/proc/pid/maps` (16 keywords: root, zygisk, riru, etc.)
   - `show_mountinfo` — filter mount entries (overlay, magisk, KSU, apatch)
   - `proc_pid_status` — force `TracerPid: 0`
   - `do_faccessat` / `vfs_fstatat` / `do_sys_openat2` — hide root-related paths (22+ paths)
   - `__arm64_sys_newuname` — spoof kernel version
   - `selinux_getprocattr` — replace `u:r:magisk:s0` → `u:r:sh:s0`
2. **Userspace:** Shamiko/Zygisk DenyList — clean mount namespace per app
3. **Process:** `PR_SET_NAME` to `servicemanager` for stealth in `ps` output

### F. ImGui UI Improvements
**Current:** Fixed pixel sizes (32px font, 50x25 toggles, 65px buttons).

**Recommended improvements:**
1. **DPI-aware scaling:** Use `ScaleAllSizes(dpiScale)` like ImRAD does — `style.ScaleAllSizes(dpiScale)` where `dpiScale = getDpi() / 120.0f`
2. **Touch padding:** Increase `ImGuiStyle::TouchPadding` for better touch accuracy (current 50x25 toggles may be too small)
3. **Font Awesome icons:** Add icon font for cleaner UI (used by Android-Imgui-Template)
4. **Animated widgets:** Port SwitchButton animation improvements from `imgui_switch.cpp` — consider smoother easing
5. **Multi-touch support:** ImGui doesn't natively support multi-touch — use separate touch slots for menu vs game (phantom-style approach)
6. **Config persistence:** Consider JSON config instead of binary dump (easier to edit, less crash-prone)
7. **Window drag:** Use `ImGui::StartMouseMovingWindow()` for cleaner window movement

### G. Touch Injection Improvements
**Current:** Single uinput device, grabs all touch events.

**Recommended:**
1. **Two-backend approach** (phantom pattern): primary InputManager injection + uinput fallback
2. **Type-B to Type-A bridging** (TouchSimulation) for better compatibility
3. **Multi-finger support:** Current 50-finger array is good; add pressure sensitivity
4. **Event scheduling:** Use `updateTimeBase` (Google uinput) for precise timing
5. **Non-root touch:** botdrop_phonectl pattern — use uhid group instead of root

### H. UE4 Offset Automation
**Current:** Manual offset updates in Function.hpp.

**Recommended:**
1. **Pattern scanner** (UE4-Function-Address-Finder): auto-find GNames, GObjects, GWorld, ProcessEvent
2. **AutoFix pipeline** (Andriod_UnrealMemoryTools): brute-force locate unknown offsets
3. **SDK dumper integration:** Run SDK dump on-device, auto-update Function.hpp offsets
4. **Version detection:** Auto-detect UE4 version from binary → select correct signatures

### I. Performance Optimizations
1. **Frame timing:** Current `usleep(3000)` fixed — use adaptive frame timing based on target FPS
2. **Distance culling:** Already done (500m limit) — add LOD levels (full ESP at close, simplified at far)
3. **Entity caching:** Cache read data across frames (like BearOwner/IMGUI_V1's caching)
4. **Batch memory reads:** Read multiple values in single driver call (reduce ioctl overhead)
5. **GPU-accelerated math:** Use NEON intrinsics for matrix operations

### J. Anti-Detection Improvements
1. **Random device names:** Generate random `/dev/xxx` names at runtime (MemKernel pattern)
2. **Memory read patterns:** Avoid sequential reads — add random delays and interleaving
3. **Behavioral analysis:** Limit aimbot speed when being spectated
4. **Server-side checks:** Add heartbeat/keepalive to detect if game is checking for anomalies
5. **String obfuscation:** Use compile-time XOR (OBFUSCATE.h) for all string literals

---

## 3. Priority Action Items

| Priority | Action | Effort | Impact |
|----------|--------|--------|--------|
| 1 (Critical) | Replace ANativeWindowCreator with symbol-patcher version | Low | Prevents crash on Android 15/16 |
| 2 (Critical) | Add Zygisk/KernelSU root hiding (Shamiko or ghost.kpm approach) | Medium | Prevents root detection |
| 3 (High) | Integrate Dobby + KittyMemory for function hooking | Medium | New features + reduced detection |
| 4 (High) | Upgrade kernel driver to GKI-compatible (TearGame/android-wuwa) | High | Multi-kernel support + stability |
| 5 (Medium) | Add DPI-aware scaling + touch padding improvements | Low | Better UX across devices |
| 6 (Medium) | Implement UE4 pattern scanner for auto-offset updates | High | Eliminates manual offset maintenance |
| 7 (Medium) | Add Font Awesome icons + animated widgets | Medium | Polished UI |
| 8 (Low) | Consider Vulkan backend (horoni/Android-ImGui-Mod-Menu) | High | Better performance on supported devices |
| 9 (Low) | Add JSON config + remote config updates | Medium | Easier config management |
| 10 (Low) | Implement touch recording/replay (Chromium pattern) | Medium | Testing + automation |

---

## 4. Reference Architecture

```
Recommended NovaX v2 Architecture:

┌─────────────────────────────────────────────────┐
│  Injection Layer (Zygisk/KernelSU Module)        │
│  ├─ Zygisk Next / ReZygisk                      │
│  ├─ Inject into game process on app_start       │
│  └─ Clean trace removal                         │
├─────────────────────────────────────────────────┤
│  Hook Layer (Dobby + KittyMemory)                │
│  ├─ eglSwapBuffers hook → ImGui render           │
│  ├─ ProcessEvent hook → game function interception│
│  ├─ MemoryPatch for no-recoil, FOV, etc.         │
│  └─ xDL for linker bypass                        │
├─────────────────────────────────────────────────┤
│  Memory Layer (GKI Kernel Driver)                │
│  ├─ android-wuwa or TearGame GKI                 │
│  ├─ Page-table-walking for fast R/W              │
│  ├─ Module hiding + random device names          │
│  └─ CFI bypass + kprobe disable                  │
├─────────────────────────────────────────────────┤
│  Overlay Layer (Updated ANativeWindowCreator)     │
│  ├─ Symbol-patcher header (Android 11–16)        │
│  ├─ EGL/Vulkan dual backend                      │
│  └─ DPI-aware scaling                            │
├─────────────────────────────────────────────────┤
│  UI Layer (ImGui with enhancements)               │
│  ├─ Font Awesome icons                           │
│  ├─ DPI-aware widgets                            │
│  ├─ Multi-touch support                          │
│  └─ Animated transitions                         │
├─────────────────────────────────────────────────┤
│  Stealth Layer (ghost.kpm + Shamiko)             │
│  ├─ 15 kernel hooks (directory/proc/kallsyms)    │
│  ├─ SELinux context spoofing                     │
│  ├─ Clean mount namespace per app                │
│  └─ Process name disguise                        │
└─────────────────────────────────────────────────┘
```

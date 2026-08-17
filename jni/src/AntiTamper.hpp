// ============================================================
// NovaX — AntiTamper.hpp
// Runtime protection: anti-debug, anti-Frida, anti-hook,
// integrity checks, license file verification
// ============================================================
#ifndef ANTITAMPER_HPP
#define ANTITAMPER_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <dlfcn.h>

// ── Threat score system ───────────────────────────────────────
static int g_threatScore = 0;
static bool g_tamperDetected = false;

// Thresholds
#define THREAT_SCORE_KILL      25   // terminate binary
#define THREAT_SCORE_WARN      10   // log warning
#define THREAT_SCORE_LOW        5   // note but continue

// ── Add threat score ──────────────────────────────────────────
static void AddThreat(int score, const char* reason) {
    g_threatScore += score;
    printf("[NovaX-Anti] +%d threat: %s (total: %d)\n", score, reason, g_threatScore);
    if (g_threatScore >= THREAT_SCORE_KILL) {
        printf("[NovaX-Anti] THREAT LEVEL CRITICAL — terminating!\n");
        g_tamperDetected = true;
        // Corrupt critical data before exit
        volatile int* p = (volatile int*)0xDEADBEEF;
        *p = 0x41414141;
        _exit(1);
    }
}

// ============================================================
// 1. ANTI-DEBUGGING
// ============================================================

// Check TracerPid — non-zero means debugger attached
static bool CheckTracerPid() {
    FILE* f = fopen("/proc/self/status", "r");
    if (!f) return false;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "TracerPid:", 10) == 0) {
            int pid = atoi(line + 10);
            fclose(f);
            if (pid != 0) {
                AddThreat(20, "Debugger attached (TracerPid)");
                return true;
            }
            return false;
        }
    }
    fclose(f);
    return false;
}

// Self-ptrace — prevents others from attaching
static bool AntiPtrace() {
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) {
        AddThreat(8, "ptrace TRACEME failed (debugger may be present)");
        return false;
    }
    // Detach so we can continue running
    ptrace(PTRACE_DETACH, 0, 0, 0);
    return true;
}

// ============================================================
// 2. ANTI-FRIDA / ANTI-INSTRUMENTATION
// ============================================================

// Scan /proc/self/maps for Frida-related libraries
static bool CheckFridaMaps() {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return false;
    char line[1024];
    const char* suspicious[] = {
        "frida", "gadget", "gum-js", "linjector",
        "xposed", "lsposed", "riru", "zygisk",
        "libdobby", "libwhale", "shadowhook",
        "re.frida", "frida-agent", "frida-server",
        NULL
    };
    while (fgets(line, sizeof(line), f)) {
        for (int i = 0; suspicious[i]; i++) {
            if (strstr(line, suspicious[i])) {
                fclose(f);
                char msg[128];
                snprintf(msg, sizeof(msg), "Frida/hook library in maps: %s", suspicious[i]);
                AddThreat(20, msg);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}

// Scan /proc/self/maps for anonymous RWX regions (injected code)
static bool CheckRWXRegions() {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return false;
    char line[1024];
    int rwxCount = 0;
    while (fgets(line, sizeof(line), f)) {
        // Look for rwxp mappings that are anonymous (no file path)
        if (strstr(line, "rwxp") && !strchr(line, '/') && !strstr(line, "[stack") && !strstr(line, "[heap")) {
            rwxCount++;
        }
    }
    fclose(f);
    if (rwxCount > 2) {
        AddThreat(8, "Suspicious anonymous RWX memory regions");
        return true;
    }
    return false;
}

// Check Frida ports (27042, 27043, default frida-server)
static bool CheckFridaPorts() {
    int ports[] = {27042, 27043, 27044, 27045, 0};
    for (int i = 0; ports[i]; i++) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(ports[i]);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        struct timeval tv = {0, 500000}; // 500ms timeout
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            close(sock);
            char msg[64];
            snprintf(msg, sizeof(msg), "Frida port %d open", ports[i]);
            AddThreat(18, msg);
            return true;
        }
        close(sock);
    }
    return false;
}

// Scan /proc/self/task for Frida worker threads
static bool CheckFridaThreads() {
    char path[128];
    snprintf(path, sizeof(path), "/proc/self/task");
    DIR* d = opendir(path);
    if (!d) return false;

    struct dirent* ent;
    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] == '.') continue;
        char taskName[256];
        snprintf(taskName, sizeof(taskName), "/proc/self/task/%s/comm", ent->d_name);
        FILE* f = fopen(taskName, "r");
        if (f) {
            char name[128] = {0};
            fgets(name, sizeof(name), f);
            fclose(f);
            // Frida worker threads have specific names
            if (strstr(name, "gmain") || strstr(name, "gdbus") ||
                strstr(name, "frida") || strstr(name, "gum-js-loop")) {
                closedir(d);
                AddThreat(15, "Frida worker thread detected");
                return true;
            }
        }
    }
    closedir(d);
    return false;
}

// Check /proc/self/fd for Frida memfd
static bool CheckFridaFd() {
    char path[128];
    snprintf(path, sizeof(path), "/proc/self/fd");
    DIR* d = opendir(path);
    if (!d) return false;

    struct dirent* ent;
    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] == '.') continue;
        char fdPath[256];
        snprintf(fdPath, sizeof(fdPath), "/proc/self/fd/%s", ent->d_name);
        char link[512] = {0};
        ssize_t len = readlink(fdPath, link, sizeof(link) - 1);
        if (len > 0) {
            link[len] = '\0';
            if (strstr(link, "memfd:") && (strstr(link, "frida") || strstr(link, "gum"))) {
                closedir(d);
                AddThreat(18, "Frida memfd in file descriptors");
                return true;
            }
        }
    }
    closedir(d);
    return false;
}

// ============================================================
// 3. ANTI-ROOT (detect if device is rooted for safety)
// ============================================================

// Root detection removed — binary requires root to run (launched via su)

// ============================================================
// 4. LICENSE FILE INTEGRITY
// ============================================================

// Simple hash for license file — detect file tampering
static uint32_t SimpleHash(const char* data, size_t len) {
    uint32_t hash = 0x811c9dc5; // FNV offset basis
    for (size_t i = 0; i < len; i++) {
        hash ^= (uint8_t)data[i];
        hash *= 0x01000193; // FNV prime
    }
    return hash;
}

// Verify license key format: XXXX-XXXX-XXXX-XXXX
static bool ValidateKeyFormat(const char* key) {
    if (!key) return false;
    int len = strlen(key);
    if (len != 19) return false; // 4*4 + 3 dashes = 19

    for (int i = 0; i < 19; i++) {
        if (i == 4 || i == 9 || i == 14) {
            if (key[i] != '-') return false;
        } else {
            if (!((key[i] >= 'A' && key[i] <= 'Z') ||
                  (key[i] >= '0' && key[i] <= '9')))
                return false;
        }
    }
    return true;
}

// Verify the license file hasn't been modified
static uint32_t g_licenseFileHash = 0;
static bool g_licenseChecked = false;

static void StoreLicenseHash(const char* key) {
    g_licenseFileHash = SimpleHash(key, strlen(key));
    g_licenseChecked = true;
}

static bool CheckLicenseIntegrity(const char* currentKey) {
    if (!g_licenseChecked) return true; // not yet checked
    uint32_t currentHash = SimpleHash(currentKey, strlen(currentKey));
    if (currentHash != g_licenseFileHash) {
        AddThreat(15, "License file tampered");
        return false;
    }
    return true;
}

// ============================================================
// 5. SELF-CODE INTEGRITY CHECK
// ============================================================

// Hash the .text section of the running process to detect binary patching
static uint32_t g_textSectionHash = 0;

static bool HashTextSection() {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return false;

    char line[1024];
    // Find our own executable's .text segment
    // Look for r-xp (read-execute, private) mapping — this is the code
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "r-xp") && !strstr(line, ".so")) {
            // Parse address range
            unsigned long start, end;
            if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
                size_t size = end - start;
                if (size > 1000 && size < 50 * 1024 * 1024) { // reasonable code size
                    // Hash first 4KB of code section
                    size_t hashLen = size < 4096 ? size : 4096;
                    g_textSectionHash = SimpleHash((const char*)start, hashLen);
                    fclose(f);
                    printf("[NovaX-Anti] Code section hash: 0x%08X\n", g_textSectionHash);
                    return true;
                }
            }
        }
    }
    fclose(f);
    return false;
}

static bool VerifyTextSection() {
    if (g_textSectionHash == 0) return true; // not initialized
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return false;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "r-xp") && !strstr(line, ".so")) {
            unsigned long start, end;
            if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
                size_t size = end - start;
                if (size > 1000 && size < 50 * 1024 * 1024) {
                    size_t hashLen = size < 4096 ? size : 4096;
                    uint32_t currentHash = SimpleHash((const char*)start, hashLen);
                    fclose(f);
                    if (currentHash != g_textSectionHash) {
                        AddThreat(20, "Binary code section tampered");
                        return false;
                    }
                    return true;
                }
            }
        }
    }
    fclose(f);
    return true;
}

// ============================================================
// 6. ANTI-HOOK (PLT/GOT integrity)
// ============================================================

// Detect inline hooks by checking function prologues
static bool CheckInlineHooks() {
    // Get address of a known function (e.g., printf from our own code)
    // and check if its first bytes look like a hook trampoline
    void* funcAddr = (void*)printf;
    if (!funcAddr) return false;

    unsigned char* bytes = (unsigned char*)funcAddr;

    // ARM64 hook patterns to detect:
    // LDR X16, .+8; BR X16 (Frida-style)
    // Pattern: 50 00 00 58 00 02 1F D6
    if (bytes[0] == 0x50 && bytes[1] == 0x00 && bytes[2] == 0x00 && bytes[3] == 0x58 &&
        bytes[4] == 0x00 && bytes[5] == 0x02 && bytes[6] == 0x1F && bytes[7] == 0xD6) {
        AddThreat(20, "Inline hook detected on printf");
        return true;
    }

    // B instruction (ARM64 unconditional branch = hook jump)
    // Opcode: 0x14xxxxxx or 0x17xxxxxx (B / BL)
    if ((bytes[3] & 0xFC) == 0x14) {
        // Unconditional branch at function start — suspicious
        AddThreat(15, "Suspicious branch at function entry (possible hook)");
        return true;
    }

    return false;
}

// ============================================================
// 7. TIMING ANOMALY DETECTION
// ============================================================

static struct timespec g_lastCheckTime = {0, 0};

static bool CheckTimingAnomaly() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (g_lastCheckTime.tv_sec != 0) {
        long elapsed = (now.tv_sec - g_lastCheckTime.tv_sec) * 1000 +
                       (now.tv_nsec - g_lastCheckTime.tv_nsec) / 1000000;
        // If more than 5 seconds elapsed between checks, someone may be stepping through
        if (elapsed > 5000 && elapsed < 60000) {
            AddThreat(6, "Timing anomaly detected (possible debugging)");
            g_lastCheckTime = now;
            return true;
        }
    }
    g_lastCheckTime = now;
    return false;
}

// ============================================================
// 8. PROCESS ENVIRONMENT CHECKS
// ============================================================

// Check if LD_PRELOAD is set (common injection method)
static bool CheckLDPreload() {
    const char* ldPreload = getenv("LD_PRELOAD");
    if (ldPreload && strlen(ldPreload) > 0) {
        AddThreat(15, "LD_PRELOAD injection detected");
        return true;
    }
    const char* ldLibPath = getenv("LD_LIBRARY_PATH");
    if (ldLibPath && (strstr(ldLibPath, "frida") || strstr(ldLibPath, "xposed"))) {
        AddThreat(15, "LD_LIBRARY_PATH contains suspicious path");
        return true;
    }
    return false;
}

// Check parent process — if parent is frida or terminal emulator, suspicious
static bool CheckParentProcess() {
    char ppidPath[64];
    snprintf(ppidPath, sizeof(ppidPath), "/proc/self/stat");
    FILE* f = fopen(ppidPath, "r");
    if (!f) return false;

    char buf[512] = {0};
    fgets(buf, sizeof(buf), f);
    fclose(f);

    // Parse ppid (4th field)
    int ppid = 0;
    int field = 0;
    char* p = buf;
    while (*p && field < 4) {
        if (*p == ' ') field++;
        if (field < 4) p++;
    }
    ppid = atoi(p);

    if (ppid > 1) {
        char parentCmd[256];
        snprintf(parentCmd, sizeof(parentCmd), "/proc/%d/cmdline", ppid);
        f = fopen(parentCmd, "r");
        if (f) {
            char name[256] = {0};
            fgets(name, sizeof(name), f);
            fclose(f);
            if (strstr(name, "frida") || strstr(name, "gdb") ||
                strstr(name, "lldb") || strstr(name, "strace") ||
                strstr(name, "ltrace")) {
                AddThreat(15, "Suspicious parent process");
                return true;
            }
        }
    }
    return false;
}

// ============================================================
// 9. ANTI-DUMP (protect process memory)
// ============================================================

// Redirect stdout/stderr to /dev/null to prevent log-based analysis
static void SuppressLogs() {
    int fd = open("/dev/null", O_WRONLY);
    if (fd >= 0) {
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
}

// ============================================================
// 10. CONTINUOUS ANTI-DEBUG BACKGROUND THREAD (RDX-style)
// ============================================================

static bool g_antiDebugThreadRunning = false;

// Continuous anti-debug loop — runs in detached thread, polls every 2 seconds
// (RDX CheckDebug pattern). Kills process if debugger attaches at ANY point.
static void AntiDebugContinuousLoop() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // 1. TracerPid check
        FILE* f = fopen("/proc/self/status", "r");
        if (f) {
            char line[256];
            while (fgets(line, sizeof(line), f)) {
                if (strncmp(line, "TracerPid:", 10) == 0) {
                    int pid = atoi(line + 10);
                    if (pid > 0) {
                        printf("[NovaX-Anti] CONTINUOUS: Debugger attached (TracerPid=%d) — terminating!\n", pid);
                        _exit(1);
                    }
                    break;
                }
            }
            fclose(f);
        }

        // 2. Re-scan Frida ports
        CheckFridaPorts();

        // 3. Re-scan Frida threads
        CheckFridaThreads();
    }
}

static void StartAntiDebugThread() {
    if (!g_antiDebugThreadRunning) {
        g_antiDebugThreadRunning = true;
        std::thread(AntiDebugContinuousLoop).detach();
        printf("[NovaX-Anti] Continuous anti-debug thread started (2s interval)\n");
    }
}

// ============================================================
// MAIN: Run all anti-tamper checks
// ============================================================

static void AntiTamperInit() {
    printf("[NovaX-Anti] Running security checks...\n");

    // 1. Anti-debug (initial scan + ptrace lock)
    AntiPtrace();
    CheckTracerPid();
    // 1b. Start continuous monitoring (RDX-style background thread)
    StartAntiDebugThread();

    // 2. Anti-Frida
    CheckFridaMaps();
    CheckFridaPorts();
    CheckFridaThreads();
    CheckFridaFd();
    CheckRWXRegions();

    // 3. Environment
    CheckLDPreload();
    CheckParentProcess();

    // 5. Inline hooks
    CheckInlineHooks();

    // 6. Hash code section
    HashTextSection();

    // Report
    printf("[NovaX-Anti] Security scan complete. Threat score: %d\n", g_threatScore);
    if (g_threatScore >= THREAT_SCORE_WARN) {
        printf("[NovaX-Anti] WARNING: Elevated threat level detected!\n");
    }
}

// Periodic checks (call from main loop — extra layer on top of background thread)
static void AntiTamperPeriodic() {
    VerifyTextSection();
    CheckTimingAnomaly();
    CheckTracerPid();
    CheckFridaMaps();
    CheckFridaThreads();
}

#endif // ANTITAMPER_HPP

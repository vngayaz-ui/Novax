# Aimbot System - Complete Documentation

## Overview
The aimbot is a **touch-based aimbot** that simulates finger movement on screen. It does NOT hook the game's aim system directly — instead, it reads enemy positions from memory, calculates where to aim, and sends touch events to move the crosshair.

## Architecture (5 Files)

```
Function.hpp ──→ Config struct (all settings, defaults, validation)
Main.cpp     ──→ Core loop (target selection, distance calc, delta math)
touch.cpp    ──→ Touch execution (easing, smoothing, finger events)
Menu_Ui.hpp  ──→ UI (toggles, sliders, per-weapon presets)
SDK.hpp      ──→ Game memory offsets
```

---

## 1. Config (`Function.hpp`)

### Master Toggles
```cpp
Cloud.自瞄 = false;        // Aimbot ON/OFF
Cloud.开火 = true;         // Auto-fire (aim only when shooting)
Cloud.喷子 = false;        // Aim shotguns
Cloud.狙击 = false;        // Aim snipers (only when scoped)
Cloud.AimLine = false;     // Draw line from crosshair to target
Cloud.IgnoreBots = false;  // Skip bots
Cloud.IgnoreKnocked = false; // Skip knocked players
Cloud.DynamicFOV = false;  // Shrink FOV circle to nearest target
Cloud.隐藏圈 = false;      // Hide FOV circle
```

### Global Settings
```cpp
Cloud.部位 = 0;            // Target: 0=Head, 1=Chest
Cloud.过滤 = 1;            // Sort targets: 1=Crosshair distance, 2=3D distance
Cloud.自瞄范围 = 180.0f;   // FOV radius (pixels) — only aim targets within this circle

Cloud.腰射速度 = 1.0f;     // Hipfire touch speed (0-20 slider)
Cloud.开镜速度 = 1.0f;     // ADS touch speed (0-20 slider)
Cloud.腰射距离 = 50.0f;    // Max hipfire aim distance (meters)
Cloud.开镜距离 = 500.0f;   // Max ADS aim distance (meters)

Cloud.腰射压枪 = 0.5f;     // Hipfire recoil compensation (0-5)
Cloud.开镜压枪 = 1.3f;     // ADS recoil compensation (0-5)
Cloud.预判 = 0.75f;        // Prediction factor (0-5)
Cloud.载具预判 = 0.3f;     // Vehicle prediction factor (0-5)
```

### Per-Weapon Presets (`WeaponAimSettings` struct)
```cpp
struct WeaponAimSettings {
    bool Enabled = false;
    float FOVRange = 180.0f;
    float HipfireDistance = 50.0f;
    float ScopeDistance = 500.0f;
    float HipfireSpeed = 1.0f;
    float ScopeSpeed = 1.0f;
    float HipfireRecoil = 0.5f;
    float ScopeRecoil = 1.3f;
    float Prediction = 0.75f;
};
```
28 weapons have individual presets (M416, AKM, M24, S12K, etc.)

### Aim Mode
```
g_AimMode = 0 → Automatic (uses global sliders)
g_AimMode = 1 → Manual (uses per-weapon presets when weapon matches)
```

---

## 2. Core Loop (`Main.cpp`)

### Flow (per entity per frame)

```
if (Cloud.自瞄) {
    ┌─────────────────────────────────────────────┐
    │ 1. Calculate crosshair-to-bone distance     │
    │    (bone 5 = head, bone 34 = chest)         │
    ├─────────────────────────────────────────────┤
    │ 2. Apply per-weapon preset (if mode == 1)   │
    ├─────────────────────────────────────────────┤
    │ 3. Distance filter                          │
    │    if(开镜 && dist > 开镜距离) → skip       │
    │    if(!开镜 && dist > 腰射距离) → skip      │
    ├─────────────────────────────────────────────┤
    │ 4. Ignore bots / knocked players            │
    ├─────────────────────────────────────────────┤
    │ 5. Set speed:                               │
    │    if(!开镜) 定义.速度 = Cloud.腰射速度     │
    │    if(开镜)  定义.速度 = Cloud.开镜速度     │
    ├─────────────────────────────────────────────┤
    │ 6. Filter mode:                             │
    │    过滤==1 → 过滤变量 = 准星 distance       │
    │    过滤==2 → 过滤变量 = Distance (3D)       │
    ├─────────────────────────────────────────────┤
    │ 7. FOV check:                               │
    │    if(准星 <= 自瞄范围) → valid target      │
    │    定义.自瞄人数++                          │
    ├─────────────────────────────────────────────┤
    │ 8. Find closest target:                     │
    │    if(过滤变量 < 定义.最小距离)             │
    │        定义.最小距离 = 过滤变量             │
    └─────────────────────────────────────────────┘

    After loop:
    ┌─────────────────────────────────────────────┐
    │ 9. Calculate recoil:                        │
    │    Hipfire: 腰射压枪 * dist/100 * weapon *  │
    │             action                          │
    │    ADS: 开镜压枪 * dist/100 * screenH *    │
    │          weapon * action                    │
    ├─────────────────────────────────────────────┤
    │ 10. Calculate aim position:                 │
    │     获取二维预判骨骼(bone 6 = head)         │
    │     获取二维预判骨骼(bone 4 = chest)        │
    │     + 压枪值 (recoil offset on Y axis)      │
    ├─────────────────────────────────────────────┤
    │ 11. Calculate delta:                        │
    │     差.x = |screen center X - 瞄准.x|      │
    │     差.y = |screen center Y - 瞄准.y|      │
    └─────────────────────────────────────────────┘

    Then passed to touch.cpp for execution.
}
```

### Key Formulas

#### Crosshair Distance (Target Selection)
```
Head mode:  准星 = distance(bone5, screen center)
Chest mode: 准星 = distance(bone34, screen center)
```

#### Recoil Compensation
```
Hipfire: 压枪值 = Cloud.腰射压枪 * (Distance/100) * 识别压枪(weapon) * 动作压枪(stance)
ADS:     压枪值 = Cloud.开镜压枪 * (Distance/100) * screenHeight * 识别压枪(weapon) * 动作压枪(stance)

识别压枪(weapon): weapon-specific multiplier (different per gun)
动作压枪(stance):  standing=1, crouching=0.95, prone=0.5
```

#### Prediction
```
predictedPos = bonePos + velocity * Cloud.预判 * Distance / BulletSpeed * 200
```

---

## 3. Touch Execution (`touch.cpp`)

### Speed Calculation (Pixel Distance Based)
```cpp
float dist = sqrt(dx*dx + dy*dy);                              // pixel distance to target
float speedScale = fmin(dist / 200.0f, 1.0f);                  // 0.0 to 1.0
float effectiveSpeed = 定义.速度 * (0.40f + 0.20f * speedScale); // 40%-60% of base speed
float easing = 0.20f + 0.20f * speedScale;                     // 20%-40% smoothing
```

| Pixel Distance | speedScale | effectiveSpeed | easing |
|---------------|-----------|---------------|--------|
| 0 px (close)  | 0.0       | 0.40x         | 0.20   |
| 100 px        | 0.5       | 0.50x         | 0.30   |
| 200+ px (far) | 1.0       | 0.60x         | 0.40   |

### Exponential Smoothing (Easing)
```cpp
Ease_y += (dy / 采样率 * effectiveSpeed - Ease_y) * easing;
Touch_Down_x += Ease_y;

Ease_x += (dx / 采样率 * effectiveSpeed - Ease_x) * easing;
Touch_Down_y += Ease_x;
```
- Higher `easing` = faster catch-up (snappier)
- Lower `easing` = smoother movement (more natural)

### Touch Events
```cpp
Touch_Down(9, x, y);  // Slot 9 = fire finger, press at position
Touch_Up(9);          // Release finger
```
- Slot 9 is the aimbot fire finger
- Touch position is modified by Ease_x/Ease_y offsets each frame

### Reset Conditions
- No target → Ease_x = 0, Ease_y = 0, Touch_Up(9)
- Overshoot (too far from original) → reset to original position, Touch_Up(9)

---

## 4. UI Menu (`Menu_Ui.hpp`)

### TAB_AIM Section
```
┌─ Aimbot ON/OFF
├─ Aim Snipers (ADS only)
├─ Aim Shotguns
├─ Aim Line (crosshair→target line)
├─ Ignore Knocked
├─ Ignore Bots
├─ Dynamic FOV (shrink circle to nearest target)
├─ Hide FOV Ring
├─ Auto Fire (aim while shooting)
├─ Target: [Head / Chest] dropdown
├─ Aim Mode: [Automatic / Per-Weapon] dropdown
├─ Global Sliders (when Automatic mode):
│   ├── FOV Range (0-1000)
│   ├── No Scope Distance (0-500)
│   ├── Scope Distance (0-500)
│   ├── No Scope Speed (0-20)
│   ├── Scope Speed (0-20)
│   ├── No Scope Recoil (0-5)
│   ├── Scope Recoil (0-5)
│   ├── Prediction (0-15)
│   └── Vehicle Prediction (0-15)
├─ FOV Circle Color
└─ Per-Weapon Settings (28 weapons, each with):
    ├── Enable toggle
    ├── FOV Range
    ├── No Scope Distance / Scope Distance
    ├── No Scope Speed / Scope Speed
    ├── No Scope Recoil / Scope Recoil
    └── Prediction
```

---

## 5. Weapon Type Detection (`Function.hpp`)

```cpp
识别喷子(weaponType):  type == 5 → true (shotgun)
识别狙击(weaponType):  type == 2 || type == 9 || type == 7 → true (sniper)

Weapon IDs → Preset mapping (Main.cpp lines 721-747):
  101004 → M416
  101003 → AKM
  101006 → M24
  101015 → S12K
  ... (28 total)
```

---

## 6. Memory Offsets Used

| Offset | Variable | Purpose |
|--------|----------|---------|
| `开火指针` | `bIsWeaponFiring` | Self firing state |
| `开镜指针` | `bIsGunADS` | ADS state |
| `向量指针` | `ComponentVelocity` | Target velocity (for prediction) |
| `子弹速度指针` | `BulletFireSpeed` | Bullet speed (for prediction) |
| `骨骼指针` | Bone data | Bone positions (for targeting) |
| `武器经理` | Weapon manager | Current weapon ID |
| `开镜视角组件` | Scope FOV component | Custom scope FOV |

---

## Data Flow Diagram

```
Game Memory
    │
    ▼
Main.cpp (Reader Thread)
    │ Reads: bone positions, velocity, firing state, ADS state, weapon ID
    │
    ▼
Main.cpp (Render Thread - per entity loop)
    │ Calculates: crosshair distance, FOV check, target selection
    │ Calculates: recoil value, prediction bone position
    │ Calculates: delta X/Y from screen center
    │ Stores: 定义.差.x, 定义.差.y, 定义.速度, 定义.瞄准
    │
    ▼
touch.cpp (Touch Thread)
    │ Reads: delta values, speed, sampling rate
    │ Applies: exponential easing smoothing
    │ Sends: Touch_Down(9, x, y) / Touch_Up(9)
    │
    ▼
Game (receives simulated touch input)
```

---

# IMPROVEMENT ANALYSIS - What We Can Improve

## Current State vs Industry Best Practices

| Feature | Our Current | Industry Standard | Priority |
|---------|------------|-------------------|----------|
| Smoothing | Linear exponential easing | PID controller / Bezier curves | HIGH |
| Speed Scaling | Linear (0.4x-0.6x) | Quadratic/Bezier curve | HIGH |
| Prediction | Basic velocity lead | Quadratic solver with acceleration | MEDIUM |
| Target Priority | Crosshair distance OR 3D distance | Combined score (FOV × distance × health) | MEDIUM |
| Anti-Detection | None | Jitter, Bezier curves, randomized timing | HIGH |
| Visibility Check | Not working | Raycast or render state | LOW (risky) |
| Per-Weapon | 28 presets | Same + attachment-aware | LOW |

---

## Improvement #1: Better Smoothing (PID Controller)

### Problem
Current easing is too simple:
```cpp
Ease_y += (dy / 采样率 * effectiveSpeed - Ease_y) * easing;
```
This is a basic low-pass filter. It works but looks robotic — constant speed, no acceleration/deceleration.

### Solution: Proportional Control
Industry standard used by professional aimbots (Sensory DMA, Axiom AI):

```cpp
// OLD: Basic exponential easing
Ease_y += (target - Ease_y) * easing;

// NEW: Proportional control with error accumulation
float error = target - current;
float velocity = error * Gain * deltaTime;
velocity = clamp(velocity, -MaxSpeed, MaxSpeed);
current += velocity;
```

**Benefits:**
- Near target = slow, precise movement (like human)
- Far from target = fast snap (like human)
- No oscillation around target
- Frame-rate independent

### Implementation Plan
```cpp
// In touch.cpp, replace the easing block:
struct AimState {
    float gain = 0.3f;           // How aggressively to aim (0.1-1.0)
    float maxSpeed = 500.0f;     // Maximum pixel movement per second
    float deadzone = 5.0f;       // Pixels - stop aiming when this close
    float errorAccumX = 0.0f;
    float errorAccumY = 0.0f;
};

// Per-frame:
float errorX = 目标.x - 当前.x;
float errorY = 目标.y - 当前.y;

// Deadzone
if (fabs(errorX) < deadzone) errorX = 0;
if (fabs(errorY) < deadzone) errorY = 0;

// Proportional control
float velX = errorX * gain * dt;
float velY = errorY * gain * dt;

// Clamp to max speed
velX = clamp(velX, -maxSpeed, maxSpeed);
velY = clamp(velY, -maxSpeed, maxSpeed);

// Apply
Touch_Down_x += velX;
Touch_Down_y += velY;
```

---

## Improvement #2: Bezier Curve Movement

### Problem
Current aim moves in straight lines. Real human aim follows curved paths with slight overshoot.

### Solution: Bezier Curves
Professional aimbots use cubic Bezier curves for natural movement:

```cpp
// Bezier curve points
// P0 = start position
// P1 = control point 1 (slight offset from line)
// P2 = control point 2 (slight offset from line)
// P3 = end position (target)

// t goes from 0 to 1 over the aim duration
float t = progress;
float u = 1 - t;

// Cubic Bezier formula
float x = u*u*u*P0.x + 3*u*u*t*P1.x + 3*u*t*t*P2.x + t*t*t*P3.x;
float y = u*u*u*P0.y + 3*u*u*t*P1.y + 3*u*t*t*P2.y + t*t*t*P3.y;
```

**Benefits:**
- Movement looks human (curved path, not straight)
- Natural overshoot and correction
- Variable speed (fast start, slow end)
- Much harder to detect

### Implementation Plan
```cpp
// Add to touch.cpp:
struct BezierAim {
    ImVec2 p0, p1, p2, p3;  // Control points
    float duration;          // How long the aim takes (seconds)
    float elapsed;           // Time elapsed
    bool active;
    
    void start(ImVec2 from, ImVec2 to) {
        p0 = from;
        p3 = to;
        
        // Generate control points with slight random offset
        float dist = sqrt(pow(to.x-from.x,2) + pow(to.y-from.y,2));
        float offset = dist * 0.1f;  // 10% of distance
        
        // Random perpendicular offset for natural curve
        ImVec2 dir = {to.x-from.x, to.y-from.y};
        ImVec2 perp = {-dir.y, dir.x};
        float len = sqrt(perp.x*perp.x + perp.y*perp.y);
        perp.x /= len; perp.y /= len;
        
        p1 = {p0.x + dir.x*0.3f + perp.x*offset, 
              p0.y + dir.y*0.3f + perp.y*offset};
        p2 = {p0.x + dir.x*0.7f - perp.x*offset, 
              p0.y + dir.y*0.7f - perp.y*offset};
        
        duration = dist / 500.0f;  // Speed based on distance
        elapsed = 0;
        active = true;
    }
    
    ImVec2 getPoint(float dt) {
        elapsed += dt;
        float t = min(elapsed / duration, 1.0f);
        float u = 1 - t;
        
        return {
            u*u*u*p0.x + 3*u*u*t*p1.x + 3*u*t*t*p2.x + t*t*t*p3.x,
            u*u*u*p0.y + 3*u*u*t*p1.y + 3*u*t*t*p2.y + t*t*t*p3.y
        };
    }
};
```

---

## Improvement #3: Better Target Priority (Combined Score)

### Problem
Current system uses either crosshair distance OR 3D distance. Not optimal.

### Solution: Combined Priority Score
Industry standard: weighted combination of multiple factors.

```cpp
// Current: Single metric
if (Cloud.过滤 == 1) 过滤变量 = 准星;    // Crosshair only
else if (Cloud.过滤 == 2) 过滤变量 = Distance;  // 3D only

// NEW: Combined score (lower = better target)
float score = 0;
score += (准星 / Cloud.自瞄范围) * 50.0f;  // Crosshair distance (50% weight)
score += (Distance / 500.0f) * 30.0f;       // 3D distance (30% weight)
score += (Health / 100.0f) * 20.0f;         // Health (20% weight)
过滤变量 = score;
```

**Benefits:**
- Balances crosshair proximity with distance and health
- Prioritizes low-HP enemies naturally
- Feels more human (humans consider multiple factors)

---

## Improvement #4: Distance-Based Speed Curve (Bezier)

### Problem
Current speed scaling is linear:
```cpp
float effectiveSpeed = 定义.速度 * (0.40f + 0.20f * speedScale);
```
This feels mechanical.

### Solution: Quadratic/Bezier Curve
Professional aimbots use non-linear curves:

```cpp
// Current: Linear
float speed = 定义.速度 * (0.40f + 0.20f * speedScale);

// NEW: Quadratic curve (more natural)
float t = speedScale;
float curve = t * t * 0.3f + t * 0.5f + 0.2f;  // Quadratic: 0.2 → 0.5 → 1.0
float speed = 定义.速度 * curve;

// Even better: S-curve (sigmoid-like)
float sCurve = 1.0f / (1.0f + exp(-10.0f * (t - 0.5f)));  // Sigmoid
float speed = 定义.速度 * (0.3f + 0.5f * sCurve);  // 0.3 → 0.8
```

**Visual comparison:**
```
Linear:     0.4 ---- 0.6
Quadratic:  0.3 -- 0.5 ---- 0.8
S-Curve:    0.3 --- 0.55 ---- 0.8  (smoothest)
```

---

## Improvement #5: Anti-Detection Features

### Problem
No anti-detection currently. Aimbots that look too perfect get flagged.

### Solution: Add Human-Like Imperfections

#### A. Micro-Jitter
```cpp
// Add small random noise to aim position
float jitterX = (rand() % 100 - 50) * 0.01f;  // ±0.5 pixels
float jitterY = (rand() % 100 - 50) * 0.01f;
Touch_Down_x += jitterX;
Touch_Down_y += jitterY;
```

#### B. Variable Timing
```cpp
// Don't aim at constant rate - vary the timing
float baseInterval = 1.0f / 60.0f;  // 60 Hz
float jitter = (rand() % 1000 - 500) * 0.00001f;  // ±5ms
float interval = baseInterval + jitter;
```

#### C. Slight Overshoot
```cpp
// Sometimes aim slightly past target, then correct
if (rand() % 100 < 5) {  // 5% chance
    overshoot = true;
    overshootAmount = (rand() % 10) * 0.1f;  // 0-1 pixel past
}
```

---

## Improvement #6: Better Prediction (Acceleration-Aware)

### Problem
Current prediction only uses velocity:
```cpp
predictedPos = bonePos + velocity * Cloud.预判 * Distance / BulletSpeed * 200;
```
Doesn't account for acceleration (target changing direction).

### Solution: Iterative 2-Pass Prediction
Industry standard for projectile prediction:

```cpp
// Pass 1: Estimate with current velocity
float timeToHit1 = Distance / BulletSpeed;
Vector3 predictedPos1 = targetPos + velocity * timeToHit1;

// Pass 2: Recalculate with updated distance
float newDistance = distance(playerPos, predictedPos1);
float timeToHit2 = newDistance / BulletSpeed;
Vector3 predictedPos2 = targetPos + velocity * timeToHit2;

// Use predictedPos2 (more accurate)
```

**Benefits:**
- More accurate when target is moving fast
- Better at medium-long range
- Accounts for bullet travel time properly

---

## Improvement #7: Scope-Aware Speed

### Problem
Same speed for all scopes. But red dot needs fast tracking while 8x needs slow precision.

### Solution: Scope-Based Speed Multiplier

```cpp
// Current: Same speed regardless of scope
float speed = 定义.速度;

// NEW: Different speed per scope type
float scopeMultiplier = 1.0f;
if (开镜) {
    if (scopeType == RED_DOT) scopeMultiplier = 1.0f;      // Fast
    else if (scopeType == 2X) scopeMultiplier = 0.85f;     // Slightly slower
    else if (scopeType == 3X) scopeMultiplier = 0.7f;      // Slower
    else if (scopeType == 4X) scopeMultiplier = 0.6f;      // Much slower
    else if (scopeType == 6X) scopeMultiplier = 0.45f;     // Very slow
    else if (scopeType == 8X) scopeMultiplier = 0.35f;     // Ultra slow
}
float speed = 定义.速度 * scopeMultiplier;
```

**Benefits:**
- Red dot: fast snap to target
- 8x scope: slow, precise adjustment
- Matches natural human behavior

---

## Implementation Priority

| Priority | Improvement | Difficulty | Impact |
|----------|------------|------------|--------|
| 1 | PID Controller Smoothing | Medium | HIGH - Much more natural aim |
| 2 | Bezier Curve Movement | Hard | HIGH - Near-human movement paths |
| 3 | Anti-Detection (Jitter) | Easy | HIGH - Avoid detection |
| 4 | Scope-Aware Speed | Easy | MEDIUM - Better scope experience |
| 5 | Combined Target Priority | Easy | MEDIUM - Smarter target selection |
| 6 | Quadratic Speed Curve | Easy | MEDIUM - Smoother acceleration |
| 7 | Acceleration-Aware Prediction | Hard | LOW - Marginal improvement |

---

## Recommended Next Steps

### Phase 1: Quick Wins (1-2 hours)
1. Add micro-jitter for anti-detection
2. Change speed curve from linear to quadratic
3. Add scope-based speed multiplier

### Phase 2: Major Improvement (3-4 hours)
1. Implement PID controller for smoothing
2. Add combined target priority score

### Phase 3: Advanced (5-6 hours)
1. Implement Bezier curve movement
2. Add acceleration-aware prediction

---

## References
- Sensory DMA: Aim Acceleration, Distance Smoothing, Aim Curve
- TaFFe.dev: Proportional Control for Aimbot Smoothing
- LiquidBounce: AngleSmooth modes (Interpolation, Sigmoid, Linear)
- Axiom AI: PID controller + Bezier curves
- bezmouse: Bezier curve mouse movement
- UE4 Forums: Projectile prediction with acceleration

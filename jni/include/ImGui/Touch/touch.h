#ifndef TOUCH_H
#define TOUCH_H

#include <Member.h>
#include <Funs.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x) - 1) / BITS_PER_LONG) + 1)
#define test_bit(array, bit) \
    ((array[(bit) / BITS_PER_LONG] >> ((bit) % BITS_PER_LONG)) & 1)

void HandleTouchEvent() noexcept;
bool GrabTouchScreen();

enum FingerStatus
{
    FINGER_NO,         // No state
    FINGER_X_UPDATE,   // X position updated
    FINGER_Y_UPDATE,   // Y position updated
    FINGER_XY_UPDATE,  // X and Y updated
    FINGER_UP          // Finger released
};

struct TouchFinger
{
    int x = -1;
    int y = -1;

    // Touch position
    int tracking_id = -1;

    // Touch tracking ID
    int status = FINGER_NO;

    timeval time;
}; // Supports up to 10 fingers

void Touch_Down(int slot, float x, float y);
void Touch_Up(int slot);
void TouchScreenHandle();
int Aim_GetObj();

inline 二维 GetMoveCoords(
    float Smooth,
    float matrix[16],
    三维 Pos,
    三维 Vector,
    三维 MyVector,
    float Difference_distance,
    float BulletSpeed
);

inline void Getimscreenmove(
    float* aimtouch_x,
    float* aimtouch_y,
    二维 a,
    二维 s,
    float Ping
);

#endif
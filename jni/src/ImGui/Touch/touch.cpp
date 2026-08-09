#include <Main.h>
#include <random>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <time.h>
#include <pthread.h>
#include <linux/uinput.h>
#include <touch.h>
#include <draw.h>
#include <timer.h>
#include <dirent.h>
#include <fcntl.h>
#include <set>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <Funs.h>

using namespace std;
using std::string;

float x_proportion, y_proportion;
TouchFinger Fingers[50];

int GetEventCount()
{
    DIR* dir = opendir("/dev/input/");
    dirent* ptr = NULL;
    int count = 0;
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strstr(ptr->d_name, "event"))
            count++;
    }
    return count;
}

int GetEventId()
{
    int EventCount = GetEventCount();
    int* fdArray = (int*)malloc(EventCount * 4 + 4);
    int result;
    for (int i = 0; i < EventCount; i++)
    {
        char temp[128];
        sprintf(temp, "/dev/input/event%d", i);
        fdArray[i] = open(temp, O_RDWR | O_NONBLOCK);
    }
    int k = 0;
    input_event ev;
    while (1)
    {
        for (int i = 0; i < EventCount; i++)
        {
            memset(&ev, 0, sizeof(ev));
            read(fdArray[i], &ev, sizeof(ev));
            if (ev.type == EV_ABS)
            {
                free(fdArray);
                return i;
            }
        }
    }
}

int fb, dev_fd;
int last_slot = -1;
bool touch_status = false;
bool TouchLock = false;
int global_tracking_id = 0;
bool bLock = false;

struct input_event batchEvents[128];
int tmpBatchCnt = 0;
timeval batchTime;

void UploadSingle(int slot, struct input_event* event, int& tmpCnt) {
    if (dev_fd <= 0) return;
    if (slot < 0 || slot >= 50) return;

    auto& Finger = Fingers[slot];
    if (Finger.status == FINGER_NO) return;

    if (Finger.status != FINGER_UP) {
        if (!touch_status) {
            touch_status = true;
            event[tmpCnt] = {.type = EV_KEY, .code = BTN_TOUCH, .value = 1};
            tmpCnt++;
        }
        if (last_slot != slot) {
            event[tmpCnt] = {.type = EV_ABS, .code = ABS_MT_SLOT, .value = slot};
            tmpCnt++;
            last_slot = slot;
        }
        event[tmpCnt] = {.type = EV_ABS, .code = ABS_MT_TRACKING_ID, .value = Finger.tracking_id};
        tmpCnt++;

        bool x_update = (Finger.status == FINGER_X_UPDATE || Finger.status == FINGER_XY_UPDATE);
        bool y_update = (Finger.status == FINGER_Y_UPDATE || Finger.status == FINGER_XY_UPDATE);

        if (x_update) {
            event[tmpCnt] = {.type = EV_ABS, .code = ABS_MT_POSITION_X, .value = Finger.x};
            tmpCnt++;
        }
        if (y_update) {
            event[tmpCnt] = {.type = EV_ABS, .code = ABS_MT_POSITION_Y, .value = Finger.y};
            tmpCnt++;
        }
    } else {
        if (last_slot != slot) {
            event[tmpCnt] = {.type = EV_ABS, .code = ABS_MT_SLOT, .value = slot};
            tmpCnt++;
            last_slot = slot;
        }
        event[tmpCnt] = {.type = EV_ABS, .code = ABS_MT_TRACKING_ID, .value = -1};
        tmpCnt++;
        touch_status = false;
    }

    Finger.status = FINGER_NO;
}

void FlushBatch() {
    if (dev_fd <= 0) return;
    if (tmpBatchCnt == 0) return;

    struct input_event syn = {.type = EV_SYN, .code = SYN_REPORT, .value = 0};
    syn.time = batchTime;
    batchEvents[tmpBatchCnt++] = syn;

    write(dev_fd, batchEvents, sizeof(struct input_event) * tmpBatchCnt);
    tmpBatchCnt = 0;
}

void Upload(int slot) {
    if (dev_fd <= 0) return;
    if (slot < 0 || slot >= 50) return;
    if (Fingers[slot].status == FINGER_NO) return;

    batchTime = Fingers[slot].time;
    UploadSingle(slot, batchEvents, tmpBatchCnt);
}

void Touch_Down(int slot, float x, float y) {
    auto& Finger = Fingers[slot];
    if (Finger.x != x && Finger.y != y)
        Finger.status = FINGER_XY_UPDATE;
    else if (Finger.x != x)
        Finger.status = FINGER_X_UPDATE;
    else if (Finger.y != y)
        Finger.status = FINGER_Y_UPDATE;
    else
        Finger.status = FINGER_XY_UPDATE;

    Finger.x = x * x_proportion;
    Finger.y = y * y_proportion;
    Finger.tracking_id = slot;
    gettimeofday(&Finger.time, 0);
    Upload(slot);
}

void Touch_Up(int slot) {
    auto& Finger = Fingers[slot];
    Finger.status = FINGER_UP;
    gettimeofday(&Finger.time, 0);
    Upload(slot);
}

timer loopAutoSleep;

bool GrabTouchScreen() {
    if (ioctl(fb, EVIOCGRAB, 1)) {
        char* v1 = strerror(*__errno());
        printf("Failed to grab touchscreen: %s.\n", v1);
        return false;
    } else {
        static int uinp_fd;
        struct uinput_user_dev uinp;
        struct input_event event;
        uinp_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (uinp_fd == 0) {
            printf("Failed to open touch device\n");
            return false;
        }
        dev_fd = uinp_fd;
        memset(&uinp, 0, sizeof(uinp));
        char name[16];
        srand(time(NULL));
        for (int i = 0; i < 15; i++) {
            switch ((rand() % 3)) {
            case 1:
                name[i] = 'A' + rand() % 26;
                break;
            case 2:
                name[i] = 'a' + rand() % 26;
                break;
            default:
                name[i] = '0' + rand() % 10;
                break;
            }
        }
        name[15] = '\0';
        strncpy(uinp.name, name, UINPUT_MAX_NAME_SIZE);
        uinp.id.vendor = rand() % 3;
        uinp.id.product = rand() % 3;
        uinp.id.version = (rand() % 2 == 0 ? 0 : 0x100);
        ioctl(uinp_fd, UI_SET_PHYS, name);

        struct input_absinfo absX;
        struct input_absinfo absY;
        ioctl(fb, EVIOCGABS(ABS_MT_POSITION_X), &absX);
        ioctl(fb, EVIOCGABS(ABS_MT_POSITION_Y), &absY);

        uinp.absmin[ABS_MT_SLOT] = 0;
        uinp.absmax[ABS_MT_SLOT] = 50;
        uinp.absmin[ABS_MT_TRACKING_ID] = 0;
        uinp.absmax[ABS_MT_TRACKING_ID] = 65535;
        uinp.absmin[ABS_MT_POSITION_X] = 0;
        uinp.absmax[ABS_MT_POSITION_X] = absX.maximum;
        uinp.absmin[ABS_MT_POSITION_Y] = 0;
        uinp.absmax[ABS_MT_POSITION_Y] = absY.maximum;

        ioctl(uinp_fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);
        ioctl(uinp_fd, UI_SET_EVBIT, EV_ABS);
        ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_SLOT);
        ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_X);
        ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);
        ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID);
        ioctl(uinp_fd, UI_SET_EVBIT, EV_SYN);

        uint8_t* bits = NULL;
        ssize_t bits_size = 0;
        int res, j, k;
        while (1) {
            res = ioctl(fb, EVIOCGBIT(EV_KEY, bits_size), bits);
            if (res < bits_size)
                break;
            bits_size = res + 16;
            bits = (uint8_t*)realloc(bits, bits_size * 2);
            if (bits == NULL)
                exit(1);
        }

        for (j = 0; j < res; j++) {
            for (k = 0; k < 8; k++)
                if (bits[j] & 1 << k) {
                    if (j * 8 + k < 249 & j * 8 + k > -1) {
                        ioctl(uinp_fd, UI_SET_KEYBIT, j * 8 + k);
                    }
                }
        }

        write(uinp_fd, &uinp, sizeof(uinp));
        ioctl(uinp_fd, UI_DEV_CREATE);
    }
    return true;
}

void HandleTouchEvent() noexcept {
    int input_id = GetEventId();
    char devicepath[64];
    sprintf(devicepath, "/dev/input/event%d", input_id);
    fb = open(devicepath, O_RDWR | O_NONBLOCK);

    struct input_absinfo absX;
    struct input_absinfo absY;
    ioctl(fb, EVIOCGABS(ABS_MT_POSITION_X), &absX);
    ioctl(fb, EVIOCGABS(ABS_MT_POSITION_Y), &absY);

    float Width = absX.maximum + 1;
    float Height = absY.maximum + 1;
    int scr_x = 屏幕x;
    int scr_y = 屏幕y;

    if (scr_x > scr_y) {
        int t = scr_y;
        scr_y = scr_x;
        scr_x = t;
    }

    bool Touch_down = false;
    float Touch_Down_x = 定义.触摸x;
    float Touch_Down_y = 定义.触摸y;
    float Touch_Size = scr_x / 8 + scr_y / 8;
    float Temptsp_x = Touch_Down_x;
    float Temptsp_y = Touch_Down_y;
    float Ease_x = 0.0f;
    float Ease_y = 0.0f;

    x_proportion = Width / scr_x;
    y_proportion = Height / scr_y;

    int* Touch_屏幕y = &屏幕y;
    int* Touch_屏幕x = &屏幕x;
    struct input_event in_ev, last_in_ev;
    int slot = 0, tracking_id = 0;
    int type, code, value;
    std::set<int> UpdatePointers;
    if (!GrabTouchScreen())
        exit(0);
    while (定义.Thread) {
        loopAutoSleep.SetFps(定义.采样率);
        loopAutoSleep.AotuFPS_init();
        UpdatePointers.clear();
        while (定义.Thread) {
            ImGuiIO& io = ImGui::GetIO();
            read(fb, &in_ev, sizeof(in_ev));
            if (in_ev.code != SYN_REPORT) {
                type = in_ev.type;
                code = in_ev.code;
                value = in_ev.value;
                auto& Finger = Fingers[slot];
                Finger.time = in_ev.time;

                if (code == ABS_MT_POSITION_Y) {
                    if (slot == 0) {
                        if (方向 == 0)
                            io.MousePos.y = value / y_proportion;
                        else if (方向 == 1)
                            io.MousePos.x = value / y_proportion;
                        else if (方向 == 2)
                            io.MousePos.y = scr_y - (value / y_proportion);
                        else
                            io.MousePos.x = scr_y - value / y_proportion;
                    }
                    Finger.y = value;
                    Finger.status |= FINGER_Y_UPDATE;
                    UpdatePointers.insert(slot);
                } else if (code == ABS_MT_POSITION_X) {
                    if (slot == 0) {
                        if (方向 == 0)
                            io.MousePos.x = value / x_proportion;
                        else if (方向 == 1)
                            io.MousePos.y = scr_x - value / x_proportion;
                        else if (方向 == 2)
                            io.MousePos.x = scr_x - (value / x_proportion);
                        else
                            io.MousePos.y = value / x_proportion;
                    }
                    Finger.x = value;
                    Finger.status |= FINGER_X_UPDATE;
                    UpdatePointers.insert(slot);
                } else if (code == ABS_MT_TRACKING_ID) {
                    if (value == -1) {
                        if (slot == 0)
                            io.MouseDown[0] = false;
                        Finger.status = FINGER_UP;
                        UpdatePointers.insert(slot);
                    } else {
                        if (slot == 0)
                            io.MouseDown[0] = true;
                        Finger.tracking_id = global_tracking_id;
                        global_tracking_id++;
                    }
                } else if (code == ABS_MT_SLOT) {
                    slot = value;
                }
            } else {
                for (std::set<int>::iterator i = UpdatePointers.begin(); i != UpdatePointers.end(); i++) {
                    Upload(*i);
                }

                if (!定义.加x && !定义.减x && !定义.加y && !定义.减y) {
                    Touch_down = false;
                    Touch_Down_x = 定义.触摸x;
                    Touch_Down_y = 定义.触摸y;
                    Ease_x = 0.0f;
                    Ease_y = 0.0f;
                    Touch_Up(9);
                } else if (定义.自瞄总数 > 0 && 方向 == 1 || 方向 == 3) {
                    if (方向 == 1)
                        Touch_Down(9, Touch_Down_x, Touch_Down_y);
                    else
                        Touch_Down(9, *Touch_屏幕y - Touch_Down_x, *Touch_屏幕x - Touch_Down_y);
                    Touch_down = true;

                    if (Touch_Down_x >= 定义.触摸x + Touch_Size || Touch_Down_x <= 定义.触摸x - Touch_Size ||
                        Touch_Down_y >= 定义.触摸y + Touch_Size || Touch_Down_y <= 定义.触摸y - Touch_Size) {
                        if (Touch_down) {
                            Touch_Down_x = 定义.触摸x;
                            Touch_Down_y = 定义.触摸y;
                            Ease_x = 0.0f;
                            Ease_y = 0.0f;
                            Touch_Up(9);
                            Temptsp_x = Touch_Down_x;
                            Temptsp_y = Touch_Down_y;
                        }
                    }
                    float dx = 定义.差.x;
                    float dy = 定义.差.y;
                    float dist = sqrt(dx * dx + dy * dy);

                    // PID Controller Parameters
                    float speedScale = fmin(dist / 200.0f, 1.0f);
                    float gain = 定义.速度 * 0.15f * (1.0f - 0.7f * pow(speedScale, 0.6f));
                    float maxSpeed = 2.75f + (定义.速度 * 0.025f);
                    float deadzone = 定义.速度 * 0.5f;
                    float dt = 1.0f / 定义.采样率;

                    if (定义.加y) {
                        float error = dy;
                        if (fabs(error) < deadzone) {
                            Ease_y = 0;
                        } else {
                            Ease_y = error * gain * dt;
                            Ease_y = fmin(fmax(Ease_y, -maxSpeed), maxSpeed);
                        }
                        Touch_Down_x += Ease_y;
                    } else if (定义.减y) {
                        float error = dy;
                        if (fabs(error) < deadzone) {
                            Ease_y = 0;
                        } else {
                            Ease_y = error * gain * dt;
                            Ease_y = fmin(fmax(Ease_y, -maxSpeed), maxSpeed);
                        }
                        Touch_Down_x -= Ease_y;
                    }
                    if (定义.加x) {
                        float error = dx;
                        if (fabs(error) < deadzone) {
                            Ease_x = 0;
                        } else {
                            Ease_x = error * gain * dt;
                            Ease_x = fmin(fmax(Ease_x, -maxSpeed), maxSpeed);
                        }
                        Touch_Down_y += Ease_x;
                    } else if (定义.减x) {
                        float error = dx;
                        if (fabs(error) < deadzone) {
                            Ease_x = 0;
                        } else {
                            Ease_x = error * gain * dt;
                            Ease_x = fmin(fmax(Ease_x, -maxSpeed), maxSpeed);
                        }
                        Touch_Down_y -= Ease_x;
                    }
                } else {
                    Touch_down = false;
                    Touch_Down_x = 定义.触摸x;
                    Touch_Down_y = 定义.触摸y;
                    Touch_Up(9);
                }
                FlushBatch();
                break;
            }
        }
        loopAutoSleep.AotuFPS();
    }
    x_proportion = 1.0f;
    y_proportion = 1.0f;
    TouchLock = false;
    bLock = false;
    UpdatePointers.clear();
    memset(Fingers, 0, sizeof(Fingers));
    方向 = 0;
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(0.0f, 0.0f);
    io.MouseDown[0] = false;
    touch_status = false;
    last_slot = -1;
    global_tracking_id = 0;
    if (fb > 0) {
        ioctl(fb, EVIOCGRAB, 0);
        close(fb);
        fb = -1;
    }
    if (dev_fd > 0) {
        struct input_event ev[3];
        for (int slot = 0; slot < 50; slot++) {
            memset(ev, 0, sizeof(ev));
            ev[0] = {.type = EV_ABS, .code = ABS_MT_SLOT, .value = slot};
            ev[1] = {.type = EV_ABS, .code = ABS_MT_TRACKING_ID, .value = -1};
            ev[2] = {.type = EV_SYN, .code = SYN_REPORT, .value = 0};
            write(dev_fd, ev, sizeof(ev));
        }
        ioctl(dev_fd, UI_DEV_DESTROY);
        close(dev_fd);
        dev_fd = -1;
    }
    return;
}
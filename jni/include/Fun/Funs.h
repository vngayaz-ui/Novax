#ifndef FUNS_H
#define FUNS_H

#include <Member.h>
#include <string.h>
#include <pthread.h>
#include <vector>
using namespace std;

struct 定义结构 {
    pid_t pid = -1;
    int 数组数量 = 0;
    long int UE4, 矩阵, 数组, 数组指针, 自身, 对象, 根, Mesh, Human, Bone, 运动组件, 车辆常见, 三称广角, 一称广角, 开镜广角, 相机广角, 聚点, 无后;
    float 触摸x, 触摸y, 采样率;
    int 自瞄总数, 自瞄人数;
    bool 开火, 开镜, b日韩, 人机判断, 战斗状态, 死亡, 上车;
    int 真人数量, 人机数量,骨骼数量;
    float 矩阵数组[16] = {
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0
    };
    三维 自身坐标, 坐标, 向量, 载具向量;
    bool 加x, 加y, 减x, 减y;
    float 速度, Distance, Health, 倒地血量, 子弹速度, 过滤物资, 载具油, 载具血, 旋转, 敌人视角, 自身判断, 最小距离;
    float 有效腰射距离, 有效开镜距离, 有效自瞄范围, 有效腰射速度, 有效开镜速度, 有效腰射压枪, 有效开镜压枪, 有效预判;
    int TeamID, 自身队伍, Action, 状态, 全局人数, 全局真人, 全局队伍, 过滤载具, 投掷物代码, 物资代码, 自身子弹数量, 手持武器, 敌人武器, 子弹数量, 子弹最大, 载具类型, 自身状态, 自身动作, 隐藏;
    unsigned char 武器类型;
    UTF16 名字[16] = {0};
    UTF16 UID[16]  = {0};
    二维 差, 瞄准;
    float 准星;
    char* 驱动路径 = "!";
    bool initialize = false;
    bool Thread = true;
    float* threadtime;
    char init[32] = "initialize";
};
struct 骨骼索引结构{
int boneaddr[200];
骨骼索引结构(){
boneaddr[0] = 288;
boneaddr[1] = 240;
boneaddr[2] = 48;
boneaddr[3] = 576;
boneaddr[4] = 1584;
boneaddr[5] = 624;
boneaddr[6] = 1632;
boneaddr[7] = 3072;
boneaddr[8] = 3024;
boneaddr[9] = 2544;
boneaddr[10] = 2736;
boneaddr[11] = 2592;
boneaddr[12] = 2784;
boneaddr[13] = 2688;
boneaddr[14] = 2880;
boneaddr[15] = 192;
};
BoneStruct bone[200];
BoneStruct bone预判[200];
};

struct 绘制结构 {
    四维 方框;
    float 方框水平中心, 方框垂直中心;
    float 长, 宽, 上高, 下高;
    骨骼索引结构 dbptr;
};

extern struct 定义结构 定义;
extern struct 骨骼索引结构 骨骼;
extern struct 绘制结构 绘制信息;

#endif
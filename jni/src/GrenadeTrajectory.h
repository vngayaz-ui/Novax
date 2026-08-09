// في GrenadeTrajectory.h
#ifndef GRENADE_TRAJECTORY_H
#define GRENADE_TRAJECTORY_H

#include <vector>
#include <unordered_map>
#include <chrono>
#include <imgui.h>
#include "Member.h"

extern int 屏幕x, 屏幕y;
extern float 矩阵数组[16];
extern float 自身坐标[3];

struct GrenadePathPoint {
    三维 position;
    float timeStamp;
};

class GrenadeTrajectory {
public:
    GrenadeTrajectory(float lifespan = 30.0f, float maxLife = 7.0f);
    void Update(三维 pos);
    void Draw(ImDrawList* 绘制);
    bool IsExpired() const;
    float GetRemainingTime() const;
    size_t GetPathSize() const { return path.size(); }
    void SetIsMolotov(bool isMolo) { isMolotov = isMolo; }
    bool IsMolotov() const { return isMolotov; }
    float CalculateDistance(三维 pos);
    
private:
    float lifespan;
    float maxLifespan;
    float startTime;
    bool hasStarted;
    bool isMolotov;
    std::vector<GrenadePathPoint> path;
    float GetCurrentTime() const;
    void CleanUpOldPaths(float currentTime);
    二维 WorldToScreen(三维 pos);
};

static std::unordered_map<long long, GrenadeTrajectory> grenadeTrajectories;

inline void UpdateAndDrawGrenade(long long objAddr, 三维 pos, ImDrawList* 绘制, bool isMolotov) {
    auto it = grenadeTrajectories.find(objAddr);
    if (it == grenadeTrajectories.end()) {
        GrenadeTrajectory traj;
        traj.SetIsMolotov(isMolotov);
        grenadeTrajectories[objAddr] = traj;
        it = grenadeTrajectories.find(objAddr);
    }
    
    auto& traj = it->second;
    
    if (traj.IsExpired()) {
        grenadeTrajectories.erase(it);
        return;
    }
    
    traj.Update(pos);
    traj.Draw(绘制);
}

#endif
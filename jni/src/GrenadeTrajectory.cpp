// في GrenadeTrajectory.cpp
#include "GrenadeTrajectory.h"
#include <cmath>
#include <cstdio>

GrenadeTrajectory::GrenadeTrajectory(float lifespan, float maxLife) 
    : lifespan(lifespan), maxLifespan(maxLife), startTime(0), hasStarted(false), isMolotov(false) {}

float GrenadeTrajectory::GetCurrentTime() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count() / 1000.0f;
}

void GrenadeTrajectory::CleanUpOldPaths(float currentTime) {
    while (!path.empty() && (currentTime - path.front().timeStamp) > lifespan) {
        path.erase(path.begin());
    }
}

void GrenadeTrajectory::Update(三维 pos) {
    float currentTime = GetCurrentTime();
    
    if (IsExpired()) return;
    
    if (!hasStarted) {
        startTime = currentTime;
        hasStarted = true;
    }
    
    path.push_back({pos, currentTime});
}

bool GrenadeTrajectory::IsExpired() const {
    if (!hasStarted) return false;
    return GetRemainingTime() <= 0.0f;
}

float GrenadeTrajectory::GetRemainingTime() const {
    if (!hasStarted) return maxLifespan;
    float elapsed = GetCurrentTime() - startTime;
    float remaining = maxLifespan - elapsed;
    return remaining > 0 ? remaining : 0;
}

二维 GrenadeTrajectory::WorldToScreen(三维 pos) {
    二维 result = {-1, -1};
    float camera = 矩阵数组[3] * pos.x + 矩阵数组[7] * pos.y + 矩阵数组[11] * pos.z + 矩阵数组[15];
    if (camera <= 0.01f) return result;
    result.x = (屏幕x / 2) + (矩阵数组[0] * pos.x + 矩阵数组[4] * pos.y + 矩阵数组[8] * pos.z + 矩阵数组[12]) / camera * (屏幕x / 2);
    result.y = (屏幕y / 2) - (矩阵数组[1] * pos.x + 矩阵数组[5] * pos.y + 矩阵数组[9] * pos.z + 矩阵数组[13]) / camera * (屏幕y / 2);
    return result;
}

float GrenadeTrajectory::CalculateDistance(三维 pos) {
    float dx = pos.x - 自身坐标[0];
    float dy = pos.y - 自身坐标[1];
    float dz = pos.z - 自身坐标[2];
    return sqrt(dx*dx + dy*dy + dz*dz) / 100.0f;
}

void GrenadeTrajectory::Draw(ImDrawList* 绘制) {
    if (path.empty()) return;
    
    for (size_t i = 1; i < path.size(); i++) {
        二维 prev = WorldToScreen(path[i-1].position);
        二维 curr = WorldToScreen(path[i].position);
        
        绘制->AddLine(ImVec2(prev.x, prev.y), ImVec2(curr.x, curr.y), IM_COL32(255, 100, 0, 255), 4.0f);
    }
    
    二维 lastPos = WorldToScreen(path.back().position);
    if (lastPos.x <= 0 || lastPos.y <= 0) return;
    
    float remainingTime = GetRemainingTime();
    if (remainingTime <= 0) return;
    
    float radius = 35.0f;
    float centerX = lastPos.x;
    float centerY = lastPos.y;
    
    绘制->AddCircleFilled(ImVec2(centerX, centerY), radius, IM_COL32(0, 0, 0, 180));
    
    float percent = remainingTime / maxLifespan;
    ImU32 arcColor;
    if (percent > 0.6f) {
        arcColor = IM_COL32(0, 255, 0, 255);
    } else if (percent > 0.3f) {
        arcColor = IM_COL32(255, 255, 0, 255);
    } else {
        arcColor = IM_COL32(255, 0, 0, 255);
    }
    
    float startAngle = -90.0f * 3.14159f / 180.0f;
    float endAngle = startAngle + (percent * 360.0f * 3.14159f / 180.0f);
    绘制->PathArcTo(ImVec2(centerX, centerY), radius - 4, startAngle, endAngle, 40);
    绘制->PathStroke(arcColor, 0, 4.0f);
    
    char timeText[16];
    int timeInt = (int)(remainingTime + 0.5f);
    snprintf(timeText, sizeof(timeText), "%d", timeInt);
    float fontSize = 28.0f;
    ImVec2 textSize = ImGui::CalcTextSize(timeText, NULL, false, fontSize);
    绘制->AddText(NULL, fontSize, 
        ImVec2(centerX - textSize.x / 2, centerY - textSize.y / 2),
        IM_COL32(255, 255, 255, 255), timeText);
    
    float distance = CalculateDistance(path.back().position);
    char distText[32];
    snprintf(distText, sizeof(distText), "%.0fm", distance);
    float distFontSize = 16.0f;
    ImVec2 distSize = ImGui::CalcTextSize(distText, NULL, false, distFontSize);
    
    float textX = centerX + radius + 10;
    float textY = centerY - distSize.y / 2;

    绘制->AddText(NULL, distFontSize, ImVec2(textX, textY), IM_COL32(220, 220, 220, 255), distText);
}
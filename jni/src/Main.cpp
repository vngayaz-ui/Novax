#include <Menu_Ui.hpp>
#include <cstdlib>
#include <string>
#include <cstring>
#include <thread>
#include <cstdlib>
#include <unordered_set>
#include <cmath>
#include <random>
#include "GrenadeTrajectory.h"
float 矩阵数组[16];
float 自身坐标[3];

timer fps_controller;
timer tempfps;
timer *rThreadFPS;
MyImGui Initptr;

struct Vector_3D{
float x=0,y=0,z=0;
};
uintptr_t K2_GetActorLocation_call_sub(uintptr_t Ptr);
uintptr_t K2_GetActorLocation_call_sub1(uintptr_t Ptr);
uintptr_t K2_GetActorLocation_call_sub2(uintptr_t Ptr);
uintptr_t K2_GetBoneLocation_call_sub(uintptr_t Actor,uintptr_t Ptr);
uintptr_t K2_RefreshCollision_call_sub(uintptr_t Actor,uintptr_t Ptr);
Vector_3D Decrypt_Location(Vector_3D EncryptedLocation,uintptr_t Actor){
uintptr_t sub_address=K2_GetActorLocation_call_sub(Actor);
if(sub_address>=0xA0BDC00)EncryptedLocation.x+=7000;
else EncryptedLocation.x-=1200;
uintptr_t sub_address1=K2_RefreshCollision_call_sub(Actor,K2_GetActorLocation_call_sub1(Driver.Read<uintptr_t>(sub_address)));
if(sub_address1>=0xC06DC00)EncryptedLocation.x+=7200;
else EncryptedLocation.x-=1300;
uintptr_t sub_address2=K2_GetActorLocation_call_sub2(Driver.Read<uintptr_t>(sub_address1));
if(sub_address2>=0xD0B2C00)EncryptedLocation.x+=7300;
else EncryptedLocation.x-=1100;
Vector_3D DecryptedLocation=Driver.Read<Vector_3D>(sub_address2);
DecryptedLocation.z=EncryptedLocation.z;
return DecryptedLocation;
}

int main() {
    Initialization();
    float fpsnum;
    定义.threadtime = &fpsnum;
    rThreadFPS = &tempfps;
    
    while (屏幕x == 0 || 屏幕y == 0 || 方向 == -1) {
        usleep(200);
    }
    
    init_屏幕x = (屏幕x > 屏幕y) ? 屏幕x : 屏幕y;
    init_屏幕y = (屏幕x > 屏幕y) ? 屏幕y : 屏幕x;

    if (!Initptr.init_egl(init_屏幕x, init_屏幕y, false, Cloud.防录屏))
        return 1;
    
    pthread_t touch_tid;
    pthread_create(&touch_tid, NULL, (void*(*)(void*))HandleTouchEvent, NULL);
    usleep(5000);
    rThreadFPS->AotuFPS_init();
    rThreadFPS->setAffinity();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    bindToCores(getBigCores());
    
    while (定义.Thread) {
        usleep(3000);
        rThreadFPS->SetFps(Cloud.目标帧率);
        ImGui_ImplAndroid_NewFrame(init_屏幕x, init_屏幕y);
        ImGui::NewFrame();
        窗口函数();
        绘制函数(ImGui::GetBackgroundDrawList());
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        eglSwapBuffers(display, surface);
        fpsnum = rThreadFPS->AotuFPS();
    }
    
    Window = false;
    pthread_join(touch_tid, NULL);
    usleep(5000);
    Cleanup();
    return 0;
}

void 绘制函数(ImDrawList *绘制) {
    if (isBGMI) {
        自身头 = UlocalPlayerBGMI;
        矩阵头 = MatrixBGMI;
    } else if (isVNG) {
        自身头 = UlocalPlayerVN;
        矩阵头 = MatrixVN;
    } else {
        自身头 = UlocalPlayerGL;
        矩阵头 = MatrixGL;
    }
定义.开镜广角=Driver.Read<uintptr_t>(定义.自身+开镜视角组件)+广角指针;
定义.一称广角=Driver.Read<uintptr_t>(定义.自身+一称视角组件)+广角指针;
定义.三称广角=Driver.Read<uintptr_t>(定义.自身+三称视角组件)+广角指针;
定义.相机广角=Driver.Read<uintptr_t>(定义.自身+相机视角组件)+广角指针;
定义.全局人数=Driver.Read<int>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.UE4+自身头)+Uworld)+网络连接对象)+网络Driver模块)+世界)+游戏状态)+玩家数量指针);
定义.全局真人=Driver.Read<int>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.UE4+自身头)+Uworld)+网络连接对象)+网络Driver模块)+世界)+游戏状态)+真人数量指针);
定义.全局队伍=Driver.Read<int>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.UE4+自身头)+Uworld)+网络连接对象)+网络Driver模块)+世界)+游戏状态)+队伍数量指针);
定义.矩阵=Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.UE4+矩阵头)+0x20)+0x270;
定义.数组=数组解密(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.UE4+自身头)+Uworld)+Ulevel),0xA0,0x448);
定义.数组数量=Driver.Read<int>(定义.数组+0x8);
定义.数组指针=Driver.Read<uintptr_t>(定义.数组);
定义.自身=Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.UE4+自身头)+Uworld)+自身指针);
定义.自身状态=Driver.Read<int>(定义.自身+状态指针);
定义.自身判断=Driver.Read<float>(定义.自身+过滤物资指针);
定义.战斗状态=Driver.Read<bool>(定义.自身+战斗状态指针);
定义.自身动作=Driver.Read<int>(定义.自身+动作指针);
Driver.Read(Driver.Read<uintptr_t>(定义.自身+Mesh指针)+坐标指针,&定义.自身坐标,12);
Driver.Read(定义.矩阵,&定义.矩阵数组,16*4);
定义.自身子弹数量=Driver.Read<int>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.自身+武器经理)+当前武器)+子弹数量指针);
定义.手持武器=Driver.Read<int>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.自身+武器经理)+当前武器)+武器实体)+武器id指针);
定义.武器类型=Driver.Read<unsigned char>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.自身+武器经理)+当前武器)+武器实体)+武器类型指针);
定义.子弹速度=Driver.Read<float>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.自身+武器经理)+当前武器)+武器实体)+子弹速度指针);
定义.聚点=Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.自身+武器经理)+当前武器)+武器实体)+聚点指针;
定义.无后=Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.自身+武器经理)+当前武器)+武器实体)+无后指针;
if(Driver.Read<int>(定义.自身+队伍指针)>0&&定义.自身+队伍指针!=队伍指针)
定义.自身队伍=Driver.Read<int>(定义.自身+队伍指针);
定义.开火=Driver.Read<bool>(定义.自身+开火指针);
定义.开镜=Driver.Read<bool>(定义.自身+开镜指针);
if(定义.自身判断==479.5){
if(Cloud.广角){
Driver.Write<float>(定义.开镜广角,Cloud.开镜广角大小);
Driver.Write<float>(定义.一称广角,Cloud.一称广角大小);
Driver.Write<float>(定义.三称广角,Cloud.三称广角大小);
Driver.Write<float>(定义.相机广角,Cloud.相机广角大小);}
if(定义.武器类型!=0&&定义.手持武器!=0&&!识别投掷(定义.手持武器).b投掷){
if(Cloud.聚点)
Driver.Write<float>(定义.聚点,0);
if(Cloud.无后)
Driver.Write<float>(定义.无后,0.15);}
}
定义.真人数量=0;
定义.人机数量=0;
定义.自瞄人数=0;
定义.最小距离=5000;
过滤变量=0;
std::unordered_set<long long> 存储对象;
存储对象.clear();
if(定义.数组数量>=10000)定义.数组数量=0;
for (int i=0;i<=定义.数组数量;i++) {
定义.对象=Driver.Read<uintptr_t>(定义.数组指针+0x8*i);
定义.过滤载具=Driver.Read<int>(定义.对象+过滤载具指针);
定义.投掷物代码=Driver.Read<int>(定义.对象+投掷物代码指针);
定义.物资代码=Driver.Read<int>(定义.对象+物资代码指针);
定义.过滤物资=Driver.Read<float>(定义.对象+过滤物资指针);  
if(定义.过滤载具==20000||定义.过滤物资==479.5||识别投掷(定义.投掷物代码).b投掷||ITEMS(定义.物资代码).bItem){
if(存储对象.count(定义.对象)) continue;
存储对象.insert(定义.对象);
定义.载具油=Driver.Read<float>(Driver.Read<uintptr_t>(定义.对象+车辆常见指针)+车油指针)/Driver.Read<float>(Driver.Read<uintptr_t>(定义.对象+车辆常见指针)+车油最大指针);
定义.载具血=Driver.Read<float>(Driver.Read<uintptr_t>(定义.对象+车辆常见指针)+车血指针)/Driver.Read<float>(Driver.Read<uintptr_t>(定义.对象+车辆常见指针)+车血最大指针);
定义.载具类型=Driver.Read<short int>(定义.对象+载具类型指针);
定义.敌人武器=Driver.Read<int>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.对象+武器经理)+当前武器)+武器实体)+武器id指针);
定义.子弹数量=Driver.Read<int>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.对象+武器经理)+当前武器)+子弹数量指针);
定义.子弹最大=Driver.Read<int>(Driver.Read<uintptr_t>(Driver.Read<uintptr_t>(定义.对象+武器经理)+当前武器)+子弹最大指针);
定义.运动组件=Driver.Read<uintptr_t>(定义.对象+运动组件指针);
定义.根=Driver.Read<uintptr_t>(定义.对象+根指针);
定义.TeamID=Driver.Read<int>(定义.对象+队伍指针);
定义.Action=Driver.Read<int>(定义.对象+动作指针);
定义.状态=Driver.Read<int>(定义.对象+状态指针);
定义.死亡=Driver.Read<bool>(定义.对象+死亡指针);
定义.上车=Driver.Read<bool>(定义.对象+上车指针);
定义.隐藏=Driver.Read<int>(定义.对象+隐藏指针);
Driver.Read(Driver.Read<uintptr_t>(定义.对象+名字指针), 定义.名字, 28);
Driver.Read(Driver.Read<uintptr_t>(定义.对象+UID指针), 定义.UID, 28);
定义.人机判断=Driver.Read<bool>(定义.对象+人机指针)||Driver.Read<bool>(定义.对象+高级人机指针)||Driver.Read<bool>(定义.对象+高级人机指针2)||Driver.Read<bool>(定义.对象+高级人机指针3);
定义.Health=Driver.Read<float>(定义.对象+血量指针)/Driver.Read<float>(定义.对象+最大血量指针)*100;
定义.倒地血量=Driver.Read<float>(定义.对象+倒地血量指针)/Driver.Read<float>(Driver.Read<uintptr_t>(定义.对象+倒地组件)+倒地最大血量指针)*100;
定义.Mesh=Driver.Read<uintptr_t>(定义.对象+Mesh指针);
定义.Human=定义.Mesh+Human指针;
定义.Bone=K2_GetBoneLocation_call_sub(定义.对象,Driver.Read<uintptr_t>(定义.Mesh+Bone指针));
定义.骨骼数量=Driver.Read<int>(定义.Mesh+Bone指针+0x8);
定义.敌人视角=Driver.Read<float>(定义.根+旋转指针);
Driver.Read(定义.根+向量指针,&定义.向量,12);
Vector_3D Decrypted=Decrypt_Location({Driver.Read<Vector_3D>(定义.Mesh+坐标指针).x,Driver.Read<Vector_3D>(定义.Mesh+坐标指针).y,Driver.Read<Vector_3D>(定义.Mesh+坐标指针).z},定义.对象);
if(Cloud.Decryption&&定义.过滤物资==479.5)
定义.坐标={Decrypted.x,Decrypted.y,Decrypted.z};
else if(定义.过滤物资==479.5)
Driver.Read(定义.Mesh+坐标指针,&定义.坐标,12);
else Driver.Read(定义.根+坐标指针,&定义.坐标,12);
Driver.Read(Driver.Read<uintptr_t>(定义.对象+当前载具指针)+载具向量指针,&定义.载具向量,12);

          定义.Distance=sqrt(pow(定义.坐标.x-定义.自身坐标.x,2)+pow(定义.坐标.y-定义.自身坐标.y,2)+pow(定义.坐标.z-定义.自身坐标.z,2))/100;

            if (ITEMS(定义.物资代码).bItem) {
                if (矩阵转换(定义.坐标, 定义.矩阵数组).高度 < 0 || 矩阵转换(定义.坐标, 定义.矩阵数组).高度 > 屏幕x || 定义.Distance >= 200)
                    continue;

                std::string ItemName;
                ItemName += ITEMS(定义.物资代码).ItemName;
                ItemName += " ";
                ItemName += to_string((int)定义.Distance);
                ItemName += "M";
                auto Size = ImGui::CountTextSize(NULL, ItemName.c_str(), 30);
                绘制->AddText(NULL, 30, ImVec2(矩阵转换(定义.坐标, 定义.矩阵数组).二维x + (矩阵转换(定义.坐标, 定义.矩阵数组).高度 * 0.5) - Size.x / 2, 矩阵转换(定义.坐标, 定义.矩阵数组).二维y - Size.y), IM_COL32(0, 255, 0, 255), ItemName.c_str());
            } else if (识别投掷(定义.投掷物代码).b投掷 && Cloud.Grenade) {
                if (矩阵转换(定义.坐标, 定义.矩阵数组).高度 < 0 || 矩阵转换(定义.坐标, 定义.矩阵数组).高度 > 屏幕x || 定义.Distance >= 200 || 定义.隐藏 != 335544832)
                    continue;

                int weaponID = 定义.投掷物代码;
                bool isMolotov = (weaponID == 602003);

                ::屏幕x = 屏幕x;
                ::屏幕y = 屏幕y;
                for (int i = 0; i < 16; i++) ::矩阵数组[i] = 定义.矩阵数组[i];
                ::自身坐标[0] = 定义.自身坐标.x;
                ::自身坐标[1] = 定义.自身坐标.y;
                ::自身坐标[2] = 定义.自身坐标.z;

                UpdateAndDrawGrenade(定义.对象, 定义.坐标, 绘制, isMolotov);

                auto& traj = grenadeTrajectories[定义.对象];

                GLuint 投掷图片 = 获取投掷物纹理(weaponID);
                if (投掷图片 != 0) {
                    float 图片宽 = 45;
                    float 图片高 = 45;
                    float centerX = 矩阵转换(定义.坐标, 定义.矩阵数组).二维x + (矩阵转换(定义.坐标, 定义.矩阵数组).高度 * 0.5);
                    float topY = 矩阵转换(定义.坐标, 定义.矩阵数组).二维y - 图片高 - 10;

                    ImVec2 图片位置 = ImVec2(centerX - 图片宽 / 2, topY);
                    绘制->AddImage((ImTextureID)投掷图片, 图片位置, ImVec2(图片位置.x + 图片宽, 图片位置.y + 图片高));

                    float remainingTime = traj.GetRemainingTime();

                    char timeText[16];
                    snprintf(timeText, sizeof(timeText), "%.1f", remainingTime > 0 ? remainingTime : 0);
                    float fontSize = 18.0f;
                    ImVec2 textSize = ImGui::CalcTextSize(timeText, NULL, false, fontSize);

                    绘制->AddText(NULL, fontSize, ImVec2(centerX - textSize.x / 2, 图片位置.y - textSize.y - 5), IM_COL32(255, 255, 255, 255), timeText);

                    float distance = traj.CalculateDistance(定义.坐标);
                    char distText[32];
                    snprintf(distText, sizeof(distText), "%.0fm", distance);
                    float distFontSize = 18.0f;
                    ImVec2 distSize = ImGui::CalcTextSize(distText, NULL, false, distFontSize);

                    绘制->AddText(NULL, distFontSize, ImVec2(图片位置.x + 图片宽 + 8, 图片位置.y + (图片高 / 2) - (distSize.y / 2)), IM_COL32(255, 255, 255, 255), distText);
                }
            } else if (定义.过滤载具 == 20000) {
            auto Vehicle = GetVehicleItem(定义.载具类型);
            if (!Vehicle.bVehicle) continue;
            if (矩阵转换(定义.坐标, 定义.矩阵数组).高度 < 0 || 矩阵转换(定义.坐标, 定义.矩阵数组).高度 > 屏幕x || 定义.Distance <= 5 || 定义.Distance >= 500 || 定义.载具血 <= 0 || 定义.载具油 < 0)
                    continue;
            std::string 载具字符;
            载具字符 += Vehicle.Name;
            载具字符 += " ";
            载具字符 += to_string((int)定义.Distance);
            载具字符 += "M";
            auto Size = ImGui::CountTextSize(NULL, 载具字符.c_str(), 30);
            绘制->AddText(NULL, 30, ImVec2(矩阵转换(定义.坐标, 定义.矩阵数组).二维x + (矩阵转换(定义.坐标, 定义.矩阵数组).高度 * 0.5) - Size.x / 2, 矩阵转换(定义.坐标, 定义.矩阵数组).二维y - 矩阵转换(定义.坐标, 定义.矩阵数组).高度 * 1.1), Cloud.载具颜色, 载具字符.c_str());
            float barWidth = 60.0f, barHeight = 6.0f;
            ImVec2 healthPos(矩阵转换(定义.坐标, 定义.矩阵数组).二维x + 矩阵转换(定义.坐标, 定义.矩阵数组).高度 * 0.5 - barWidth / 2, 矩阵转换(定义.坐标, 定义.矩阵数组).二维y - 矩阵转换(定义.坐标, 定义.矩阵数组).高度 * 1.1 - 20);
            ImVec2 healthEnd(healthPos.x + 定义.载具血 * barWidth, healthPos.y + barHeight);
            绘制->AddRectFilled(healthPos, healthEnd, IM_COL32(0, 255, 0, 255), 0.0f);
            绘制->AddRect(healthPos, ImVec2(healthPos.x + barWidth, healthPos.y + barHeight), IM_COL32(255, 255, 255, 200), 2.5f);
            ImVec2 fuelPos(矩阵转换(定义.坐标, 定义.矩阵数组).二维x + 矩阵转换(定义.坐标, 定义.矩阵数组).高度 * 0.5 - barWidth / 2, 矩阵转换(定义.坐标, 定义.矩阵数组).二维y - 矩阵转换(定义.坐标, 定义.矩阵数组).高度 * 1.1 - 10);
            ImVec2 fuelEnd(fuelPos.x + 定义.载具油 * barWidth, fuelPos.y + barHeight);
            绘制->AddRectFilled(fuelPos, fuelEnd, IM_COL32(255, 255, 0, 255), 0.0f);
            绘制->AddRect(fuelPos, ImVec2(fuelPos.x + barWidth, fuelPos.y + barHeight), IM_COL32(255, 255, 255, 200), 2.5f);
            } else if(定义.过滤物资==479.5){
            if(定义.Distance>=500||定义.对象==定义.自身||定义.TeamID==定义.自身队伍||定义.TeamID<0||定义.Action==0||定义.Action==33554440||定义.Action==8650752||定义.Action>=262130&&定义.Action<=262170||定义.Mesh<=0x0||定义.Bone<=0x0||定义.倒地血量<0||定义.Health<0||定义.死亡)
                        continue;

                if (Cloud.HideBots) {
                    if (定义.人机判断)
                        continue;
                }
                if(!定义.人机判断)
                定义.真人数量++;
                else
                定义.人机数量++;

                if (Cloud.Alert) {
                    ImVec2 极坐标;
                    float 矩阵最大差长度 = sqrt(定义.矩阵数组[3] * 定义.矩阵数组[3] + 定义.矩阵数组[7] * 定义.矩阵数组[7]);
                    float 视角正弦 = 定义.矩阵数组[7] / 矩阵最大差长度;
                    float 视角余弦 = 定义.矩阵数组[3] / 矩阵最大差长度;
                    float 坐标正弦 = -(定义.坐标.x - 定义.自身坐标.x) / 定义.Distance;
                    float 坐标余弦 = (定义.坐标.y - 定义.自身坐标.y) / 定义.Distance;
                    float finalSin = 坐标正弦 * 视角余弦 - 视角正弦 * 坐标余弦;
                    float finalCos = 坐标余弦 * 视角余弦 + 坐标正弦 * 视角正弦;
                    float lenFinal = sqrt(finalCos * finalCos + finalSin * finalSin);
                    极坐标.x = finalCos * 150 / lenFinal;
                    极坐标.y = finalSin * 150 / lenFinal;

                    ImVec2 center(屏幕x / 2, 屏幕y / 2);
                    ImVec2 circleCenter(center.x + 极坐标.x, center.y + 极坐标.y);

                    ImColor alertColor;
                    if (定义.人机判断)
                        alertColor = White;
                    else
                        alertColor = Red;

                    ImVec2 dir = ImVec2(极坐标.x, 极坐标.y);
                    float len = sqrt(dir.x * dir.x + dir.y * dir.y);
                    if (len != 0) {
                        dir.x /= len;
                        dir.y /= len;
                    }

                    float size = 18.0f;

                    ImVec2 p1 = circleCenter;
                    ImVec2 p2 = ImVec2(circleCenter.x - dir.x * size + dir.y * size * 0.6f, circleCenter.y - dir.y * size - dir.x * size * 0.6f);
                    ImVec2 p3 = ImVec2(circleCenter.x - dir.x * size - dir.y * size * 0.6f, circleCenter.y - dir.y * size + dir.x * size * 0.6f);

                    绘制->AddTriangleFilled(p1, p2, p3, alertColor);

                    std::string arrowTip;
                    arrowTip += to_string((int)定义.Distance);
                    arrowTip += "M";
                    auto arrowLeft = ImGui::CountTextSize(NULL, arrowTip.c_str(), 20);
                    绘制->AddText(NULL, 20, ImVec2(circleCenter.x - arrowLeft.x / 2, circleCenter.y - arrowLeft.y / 2), Black, arrowTip.c_str());
                }
if (Cloud.EnemyLanding) {
    if (定义.Action == 8388616 || 定义.Action == 8388617) {
        if (定义.Distance <= 50.0f && 定义.Distance > 0) {
            std::string alertText = "ENEMY LANDING! " + std::to_string((int)定义.Distance) + "m";
            auto textSize = ImGui::CalcTextSize(alertText.c_str());
            
            float centerX = 屏幕x / 2;
            float centerY = 156;
            
            ImVec2 bgPos(centerX - textSize.x/2 - 30, centerY - 35);
            ImVec2 bgEnd(centerX + textSize.x/2 + 30, centerY + 45);
            绘制->AddRectFilled(bgPos, bgEnd, IM_COL32(200, 0, 0, 200), 12.0f);
            绘制->AddRect(bgPos, bgEnd, IM_COL32(255, 0, 0, 255), 12.0f, 0, 3.0f);
            绘制->AddText(NULL, 40.0f, ImVec2(centerX - textSize.x/2, centerY - 20), IM_COL32(255, 50, 50, 255), alertText.c_str());
            
            float pulse = (sin(ImGui::GetTime() * 5.0f) + 1.0f) * 0.5f;
            float radius = 100.0f + pulse * 50.0f;
            ImU32 circleColor = IM_COL32(255, 0, 0, (int)(150 + pulse * 80));
            绘制->AddCircle(ImVec2(centerX, centerY + 40), radius, circleColor, 0, 4.0f);
            绘制->AddCircle(ImVec2(centerX, centerY + 40), radius * 0.6f, circleColor, 0, 3.0f);
            
            ImVec2 dirPos(centerX - 60, centerY - 80);
            ImVec2 dirEnd(centerX + 60, centerY - 80);
            绘制->AddLine(dirPos, dirEnd, IM_COL32(255, 0, 0, 255), 2.0f);
            绘制->AddLine(ImVec2(centerX - 40, centerY - 60), ImVec2(centerX, centerY - 100), IM_COL32(255, 0, 0, 255), 2.0f);
            绘制->AddLine(ImVec2(centerX + 40, centerY - 60), ImVec2(centerX, centerY - 100), IM_COL32(255, 0, 0, 255), 2.0f);
        }
    }
}
if(矩阵转换(定义.坐标,定义.矩阵数组).高度<0||矩阵转换(定义.坐标,定义.矩阵数组).高度>屏幕x||矩阵转换({定义.坐标.x+(-绘制信息.宽*cos(定义.敌人视角/180*M_PI)-(-绘制信息.长)*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(-绘制信息.宽*sin(定义.敌人视角/180*M_PI)+(-绘制信息.长)*cos(定义.敌人视角/180*M_PI)),定义.坐标.z-绘制信息.下高},定义.矩阵数组).高度<0||矩阵转换({定义.坐标.x+(-绘制信息.宽*cos(定义.敌人视角/180*M_PI)-(-绘制信息.长)*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(-绘制信息.宽*sin(定义.敌人视角/180*M_PI)+(-绘制信息.长)*cos(定义.敌人视角/180*M_PI)),定义.坐标.z-绘制信息.下高},定义.矩阵数组).高度>屏幕x||矩阵转换({定义.坐标.x+(绘制信息.宽*cos(定义.敌人视角/180*M_PI)-(-绘制信息.长)*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(绘制信息.宽*sin(定义.敌人视角/180*M_PI)+(-绘制信息.长)*cos(定义.敌人视角/180*M_PI)),定义.坐标.z-绘制信息.下高},定义.矩阵数组).高度<0||矩阵转换({定义.坐标.x+(绘制信息.宽*cos(定义.敌人视角/180*M_PI)-(-绘制信息.长)*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(绘制信息.宽*sin(定义.敌人视角/180*M_PI)+(-绘制信息.长)*cos(定义.敌人视角/180*M_PI)),定义.坐标.z-绘制信息.下高},定义.矩阵数组).高度>屏幕x||矩阵转换({定义.坐标.x+(绘制信息.宽*cos(定义.敌人视角/180*M_PI)-绘制信息.长*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(绘制信息.宽*sin(定义.敌人视角/180*M_PI)+绘制信息.长*cos(定义.敌人视角/180*M_PI)),定义.坐标.z-绘制信息.下高},定义.矩阵数组).高度<0||矩阵转换({定义.坐标.x+(绘制信息.宽*cos(定义.敌人视角/180*M_PI)-绘制信息.长*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(绘制信息.宽*sin(定义.敌人视角/180*M_PI)+绘制信息.长*cos(定义.敌人视角/180*M_PI)),定义.坐标.z-绘制信息.下高},定义.矩阵数组).高度>屏幕x||矩阵转换({定义.坐标.x+(-绘制信息.宽*cos(定义.敌人视角/180*M_PI)-绘制信息.长*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(-绘制信息.宽*sin(定义.敌人视角/180*M_PI)+绘制信息.长*cos(定义.敌人视角/180*M_PI)),定义.坐标.z-绘制信息.下高},定义.矩阵数组).高度<0||矩阵转换({定义.坐标.x+(-绘制信息.宽*cos(定义.敌人视角/180*M_PI)-绘制信息.长*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(-绘制信息.宽*sin(定义.敌人视角/180*M_PI)+绘制信息.长*cos(定义.敌人视角/180*M_PI)),定义.坐标.z-绘制信息.下高},定义.矩阵数组).高度>屏幕x||矩阵转换({定义.坐标.x+(-绘制信息.宽*cos(定义.敌人视角/180*M_PI)-(-绘制信息.长)*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(-绘制信息.宽*sin(定义.敌人视角/180*M_PI)+(-绘制信息.长)*cos(定义.敌人视角/180*M_PI)),定义.坐标.z+绘制信息.上高},定义.矩阵数组).高度<0||矩阵转换({定义.坐标.x+(-绘制信息.宽*cos(定义.敌人视角/180*M_PI)-(-绘制信息.长)*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(-绘制信息.宽*sin(定义.敌人视角/180*M_PI)+(-绘制信息.长)*cos(定义.敌人视角/180*M_PI)),定义.坐标.z+绘制信息.上高},定义.矩阵数组).高度>屏幕x||矩阵转换({定义.坐标.x+(绘制信息.宽*cos(定义.敌人视角/180*M_PI)-(-绘制信息.长)*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(绘制信息.宽*sin(定义.敌人视角/180*M_PI)+(-绘制信息.长)*cos(定义.敌人视角/180*M_PI)),定义.坐标.z+绘制信息.上高},定义.矩阵数组).高度<0||矩阵转换({定义.坐标.x+(绘制信息.宽*cos(定义.敌人视角/180*M_PI)-(-绘制信息.长)*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(绘制信息.宽*sin(定义.敌人视角/180*M_PI)+(-绘制信息.长)*cos(定义.敌人视角/180*M_PI)),定义.坐标.z+绘制信息.上高},定义.矩阵数组).高度>屏幕x||矩阵转换({定义.坐标.x+(绘制信息.宽*cos(定义.敌人视角/180*M_PI)-绘制信息.长*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(绘制信息.宽*sin(定义.敌人视角/180*M_PI)+绘制信息.长*cos(定义.敌人视角/180*M_PI)),定义.坐标.z+绘制信息.上高},定义.矩阵数组).高度<0||矩阵转换({定义.坐标.x+(绘制信息.宽*cos(定义.敌人视角/180*M_PI)-绘制信息.长*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(绘制信息.宽*sin(定义.敌人视角/180*M_PI)+绘制信息.长*cos(定义.敌人视角/180*M_PI)),定义.坐标.z+绘制信息.上高},定义.矩阵数组).高度>屏幕x||矩阵转换({定义.坐标.x+(-绘制信息.宽*cos(定义.敌人视角/180*M_PI)-绘制信息.长*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(-绘制信息.宽*sin(定义.敌人视角/180*M_PI)+绘制信息.长*cos(定义.敌人视角/180*M_PI)),定义.坐标.z+绘制信息.上高},定义.矩阵数组).高度<0||矩阵转换({定义.坐标.x+(-绘制信息.宽*cos(定义.敌人视角/180*M_PI)-绘制信息.长*sin(定义.敌人视角/180*M_PI)),定义.坐标.y+(-绘制信息.宽*sin(定义.敌人视角/180*M_PI)+绘制信息.长*cos(定义.敌人视角/180*M_PI)),定义.坐标.z+绘制信息.上高},定义.矩阵数组).高度>屏幕x)continue;
if(定义.状态==9){
绘制信息.长=50;绘制信息.宽=50;绘制信息.上高=100;绘制信息.下高=0;
}else if(定义.状态==1||定义.状态==4||定义.状态==10){
绘制信息.长=50;绘制信息.宽=50;绘制信息.上高=20;绘制信息.下高=100;
}else if(定义.上车){
绘制信息.长=50;绘制信息.宽=50;绘制信息.上高=60;绘制信息.下高=120;
}else if(定义.Action==8388616||定义.Action==8388617){
绘制信息.长=50;绘制信息.宽=50;绘制信息.上高=60;绘制信息.下高=140;
}else if(定义.状态==8||定义.状态==2||定义.状态==5||定义.Action==131072||定义.Action==131073||定义.Action==1179648||定义.Action==163840){
绘制信息.长=50;绘制信息.宽=50;绘制信息.上高=0;绘制信息.下高=100;
}else{
绘制信息.长=50;绘制信息.宽=50;绘制信息.上高=100;绘制信息.下高=100;
}
绘制信息.方框.t=矩阵转换(定义.坐标,定义.矩阵数组).二维y-矩阵转换(定义.坐标,定义.矩阵数组).高度*2.18;
绘制信息.方框.b=矩阵转换(定义.坐标,定义.矩阵数组).二维y+矩阵转换(定义.坐标,定义.矩阵数组).高度*0.2;
绘制信息.方框.l=矩阵转换(定义.坐标,定义.矩阵数组).二维x-矩阵转换(定义.坐标,定义.矩阵数组).高度*0.09;
绘制信息.方框.r=矩阵转换(定义.坐标,定义.矩阵数组).二维x+矩阵转换(定义.坐标,定义.矩阵数组).高度*1.09;
绘制信息.方框水平中心=矩阵转换(定义.坐标,定义.矩阵数组).二维x+(矩阵转换(定义.坐标,定义.矩阵数组).高度*0.5);
绘制信息.方框垂直中心=矩阵转换(定义.坐标,定义.矩阵数组).二维y-(矩阵转换(定义.坐标,定义.矩阵数组).高度*1);
                if (Cloud.Box) {
                    ImColor 方框颜色;
                    ImColor 内部颜色;

                    if (定义.人机判断) {
                        方框颜色 = Cloud.BotBoxColor;
                        内部颜色 = ImColor((int)(Cloud.BotBoxColor.Value.x * 255), (int)(Cloud.BotBoxColor.Value.y * 255), (int)(Cloud.BotBoxColor.Value.z * 255), (int)(Cloud.BoxAlpha * 255));
                    } else {
                        方框颜色 = Cloud.PlayerBoxColor;
                        内部颜色 = ImColor((int)(Cloud.PlayerBoxColor.Value.x * 255), (int)(Cloud.PlayerBoxColor.Value.y * 255), (int)(Cloud.PlayerBoxColor.Value.z * 255), (int)(Cloud.BoxAlpha * 255));
                    }

                    绘制->AddRect(ImVec2(绘制信息.方框.l, 绘制信息.方框.t), ImVec2(绘制信息.方框.r, 绘制信息.方框.b), 方框颜色, 0, 0, Cloud.方框粗细);
                }
                if (Cloud.Health) {
                    ImColor 血量颜色;

                    if (定义.Health == 0 && !定义.死亡)
                        血量颜色 = ImColor(255, 0, 0, 255);
                    else
                        血量颜色 = 定义.人机判断 ? Cloud.BotHealthColor : Cloud.PlayerHealthColor;

                    float 方框宽 = 120, 方框高 = 15, 血量长度;
                    if (定义.Health == 0 && !定义.死亡)
                        血量长度 = 定义.倒地血量;
                    else
                        血量长度 = 定义.Health;

                    auto 屏幕位置 = 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角);

                    float 中心X = (屏幕位置.屏幕顶点[4].x + 屏幕位置.屏幕顶点[5].x + 屏幕位置.屏幕顶点[6].x + 屏幕位置.屏幕顶点[7].x) / 4;
                    float 顶部Y = std::min({屏幕位置.屏幕顶点[4].y, 屏幕位置.屏幕顶点[5].y, 屏幕位置.屏幕顶点[6].y, 屏幕位置.屏幕顶点[7].y}) - 30;

                    ImVec2 开始位置 = ImVec2(中心X - 方框宽 / 2, 顶部Y);
                    ImVec2 结束位置 = ImVec2(开始位置.x + 血量长度 * 方框宽 / 100, 开始位置.y + 方框高);

                    绘制->AddRectFilled(开始位置, 结束位置, 血量颜色, 5, ImDrawFlags_RoundCornersAll);
                    绘制->AddRect(ImVec2(开始位置.x, 开始位置.y), ImVec2(开始位置.x + 方框宽, 结束位置.y), ImColor(50, 50, 50, 255), 5, ImDrawFlags_RoundCornersAll, 1);
                }
                if (Cloud.Line) {
                    ImColor 颜色;
                    if (定义.人机判断)
                        颜色 = Cloud.BotLineColor;
                    else
                        颜色 = Cloud.PlayerLineColor;
                    auto box = 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角);
                    float 中心X = (box.屏幕顶点[4].x + box.屏幕顶点[5].x + box.屏幕顶点[6].x + box.屏幕顶点[7].x) / 4;
                    float 底部Y = std::min({box.屏幕顶点[4].y, box.屏幕顶点[5].y, box.屏幕顶点[6].y, box.屏幕顶点[7].y}) - (屏幕y * 0.05);
                    绘制->AddLine(ImVec2(屏幕x / 2, 0), ImVec2(中心X, 底部Y), 颜色, 3);
                }
                if (Cloud.Weapon) {
    GLuint 枪械图片 = 识别枪械(定义.敌人武器);
    if (枪械图片 != 0) {
        float 图片宽 = 120.0f;
        float 图片高 = 45.0f;
        auto box = 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角);
        float centerX = (box.屏幕顶点[4].x + box.屏幕顶点[5].x + box.屏幕顶点[6].x + box.屏幕顶点[7].x) / 4;
        float topY = std::min({box.屏幕顶点[4].y, box.屏幕顶点[5].y, box.屏幕顶点[6].y, box.屏幕顶点[7].y}) - 90;
        ImVec2 图片位置 = ImVec2(centerX - 图片宽 / 2, topY - 图片高);
        绘制->AddImage((ImTextureID)枪械图片, 图片位置, ImVec2(图片位置.x + 图片宽, 图片位置.y + 图片高));
        
        if (Cloud.TimerRevive) {
            std::string key = std::to_string(定义.TeamID) + "_" + std::to_string(i);
            if (定义.Action == 32784) {
                if (rescueTimers.find(key) == rescueTimers.end()) rescueTimers[key] = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - rescueTimers[key]).count();
                int remaining = std::max(0, 10 - (int)elapsed);
                drawCountdownCircle(centerX, topY - 图片高 - 15, remaining, 10, ImColor(0, 255, 0, 255));
            } else if (定义.Action == 163840) {
                if (自救Timers.find(key) == 自救Timers.end()) 自救Timers[key] = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - 自救Timers[key]).count();
                int remaining = std::max(0, 10 - (int)elapsed);
                drawCountdownCircle(centerX, topY - 图片高 - 15, remaining, 10, ImColor(255, 255, 0, 255));
            }
        }
    }
}
                if (Cloud.Name || Cloud.TeamID) {
                    std::string 字符团队;
                    std::string 字符名字;

                    if (Cloud.TeamID) {
                        字符团队 = to_string((int)定义.TeamID) + " ";
                    }

                    if (Cloud.Name) {
                        if (定义.人机判断)
                            字符名字 = "AI";
                        else
                            字符名字 = getUTF(定义.名字);
                    }

                    auto Size团队 = ImGui::CountTextSize(NULL, 字符团队.c_str(), 30);
                    auto Size名字 = ImGui::CountTextSize(NULL, 字符名字.c_str(), 30);

                    auto box = 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角);

                    ImVec2 pos = ImVec2(
                        (box.屏幕顶点[4].x + box.屏幕顶点[5].x + box.屏幕顶点[6].x + box.屏幕顶点[7].x) / 4 - (Size团队.x + Size名字.x) / 2,
                        std::min({box.屏幕顶点[4].y, box.屏幕顶点[5].y, box.屏幕顶点[6].y, box.屏幕顶点[7].y}) - 35 - std::max({Size团队.y, Size名字.y})
                    );

                    if (!字符团队.empty())
                        绘制->AddText(NULL, 30, pos, Cloud.TeamIDColor, 字符团队.c_str());

                    if (!字符名字.empty()) {
                        int teamIndex = 定义.TeamID % 20;
                        ImColor playerNameColor = ImColor(Cloud.teamColors[teamIndex]);
                        绘制->AddText(NULL, 30, ImVec2(pos.x + Size团队.x, pos.y), playerNameColor, 字符名字.c_str());
                    }
                }
                if (Cloud.Distance) {
                    std::string 距离字符;
                    距离字符 += to_string((int)定义.Distance);
                    距离字符 += "M";
                    auto Size = ImGui::CountTextSize(NULL, 距离字符.c_str(), 30);
                    绘制->AddText(NULL, 30, ImVec2((三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角).屏幕顶点[0].x + 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角).屏幕顶点[1].x + 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角).屏幕顶点[2].x + 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角).屏幕顶点[3].x) / 4 - Size.x / 2, std::max({三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角).屏幕顶点[0].y, 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角).屏幕顶点[1].y, 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角).屏幕顶点[2].y, 三维方框函数(定义.坐标, 绘制信息.长, 绘制信息.宽, 绘制信息.上高, 绘制信息.下高, 定义.敌人视角).屏幕顶点[3].y}) + 55 - Size.y), ImColor(255, 255, 0, 255), 距离字符.c_str());
                }
if(Cloud.Skeleton){
绘制->AddCircle(ImVec2(获取二维骨骼(定义.坐标,定义.Human,定义.Bone,6).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,6).y-(获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y-获取二维骨骼(定义.坐标,定义.Human,定义.Bone,6).y)),(获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y-获取二维骨骼(定义.坐标,定义.Human,定义.Bone,6).y)*2,IM_COL32(255,255,255,200),200,2.5f);
if(!定义.人机判断){
if(定义.骨骼数量==65){
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y+7},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,37).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,37).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,58).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,58).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,58).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,58).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).y},IM_COL32(255,255,255,200),{2.5f});
}else if(定义.骨骼数量==67){
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y+7},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,37).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,37).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,56).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,56).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,56).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,56).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,60).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,60).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,61).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,61).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,60).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,60).y},IM_COL32(255,255,255,200),{2.5f});
}
}else if(定义.人机判断&&定义.骨骼数量!=29&&定义.骨骼数量!=61&&定义.Action!=1073741832){
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y+7},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,37).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,37).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,58).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,58).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,58).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,58).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).y},IM_COL32(255,255,255,200),{2.5f});
}else if(定义.人机判断&&定义.Action==1073741832){
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y+7},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,13).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,35).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,37).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,37).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,56).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,56).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,57).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,56).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,56).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,59).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,60).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,60).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,61).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,61).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,60).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,60).y},IM_COL32(255,255,255,200),{2.5f});
}else if(定义.人机判断&&定义.骨骼数量==29){
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y+7},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,8).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,8).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,14).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,15).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,15).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,15).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,15).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,16).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,8).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,8).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,9).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,9).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,9).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,9).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,10).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,10).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,22).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,22).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,19).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,19).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,22).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,22).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,23).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,23).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,23).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,23).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,24).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,24).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,19).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,19).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,20).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,20).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,21).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,21).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,20).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,20).y},IM_COL32(255,255,255,200),{2.5f});
}else if(定义.人机判断&&定义.骨骼数量==61){
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y+7},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,1).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,29).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,29).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,8).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,8).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,29).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,29).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,30).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,30).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,30).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,30).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,31).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,31).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,8).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,8).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,9).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,9).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,9).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,9).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,10).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,10).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,49).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,49).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,49).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,49).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,50).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,50).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,51).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,51).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,50).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,50).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,53).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).y},IM_COL32(255,255,255,200),{2.5f});
绘制->AddLine({获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,54).y},{获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).x,获取二维骨骼(定义.坐标,定义.Human,定义.Bone,55).y},IM_COL32(255,255,255,200),{2.5f});
}
}

if(Cloud.自瞄){
    // FIX: use Cloud.AimMode (saved in struct), write presets to g_ActiveAim not Cloud
    if(Cloud.部位 == 0)
        定义.准星=sqrt(pow(获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x-屏幕x/2,2)+pow(获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).y-屏幕y/2,2));
    else
        定义.准星=sqrt(pow(获取二维骨骼(定义.坐标,定义.Human,定义.Bone,5).x-屏幕x/2,2)+pow(获取二维骨骼(定义.坐标,定义.Human,定义.Bone,34).y-屏幕y/2,2));
    
    if (Cloud.AimMode == 1) {
        int WeaponID = 定义.手持武器;
        WeaponAimSettings* Preset = nullptr;
        
        if (WeaponID == 101004 || WeaponID == 1010041) Preset = &Cloud.M416_Settings;
        else if (WeaponID == 101005 || WeaponID == 1010051) Preset = &Cloud.AKM_Settings;
        else if (WeaponID == 101006 || WeaponID == 1010061) Preset = &Cloud.M762_Settings;
        else if (WeaponID == 101007 || WeaponID == 1010071) Preset = &Cloud.SCAR_L_Settings;
        else if (WeaponID == 101008 || WeaponID == 1010081) Preset = &Cloud.AUG_Settings;
        else if (WeaponID == 101009 || WeaponID == 1010091) Preset = &Cloud.Groza_Settings;
        else if (WeaponID == 103001 || WeaponID == 1030011) Preset = &Cloud.M24_Settings;
        else if (WeaponID == 103003 || WeaponID == 1030031) Preset = &Cloud.AWM_Settings;
        else if (WeaponID == 103002 || WeaponID == 1030021) Preset = &Cloud.Kar98k_Settings;
        else if (WeaponID == 103004 || WeaponID == 1030041) Preset = &Cloud.Mosin_Settings;
        else if (WeaponID == 103005 || WeaponID == 1030051) Preset = &Cloud.MK14_Settings;
        else if (WeaponID == 103006 || WeaponID == 1030061) Preset = &Cloud.Mini14_Settings;
        else if (WeaponID == 103007 || WeaponID == 1030071) Preset = &Cloud.SLR_Settings;
        else if (WeaponID == 103008 || WeaponID == 1030081) Preset = &Cloud.SKS_Settings;
        else if (WeaponID == 105001 || WeaponID == 1050011) Preset = &Cloud.DP28_Settings;
        else if (WeaponID == 105002 || WeaponID == 1050021) Preset = &Cloud.M249_Settings;
        else if (WeaponID == 102007 || WeaponID == 1020071) Preset = &Cloud.UMP45_Settings;
        else if (WeaponID == 102008 || WeaponID == 1020081) Preset = &Cloud.Bison_Settings;
        else if (WeaponID == 102003 || WeaponID == 1020031) Preset = &Cloud.Vector_Settings;
        else if (WeaponID == 102004 || WeaponID == 1020041) Preset = &Cloud.TommyGun_Settings;
        else if (WeaponID == 102005 || WeaponID == 1020051) Preset = &Cloud.MP5K_Settings;
        else if (WeaponID == 102009 || WeaponID == 1020091) Preset = &Cloud.MP9_Settings;
        else if (WeaponID == 102010 || WeaponID == 1020101) Preset = &Cloud.P90_Settings;
        else if (WeaponID == 104003 || WeaponID == 1040031) Preset = &Cloud.S12K_Settings;
        else if (WeaponID == 104004 || WeaponID == 1040041) Preset = &Cloud.DBS_Settings;
        else if (WeaponID == 104001 || WeaponID == 1040011) Preset = &Cloud.S686_Settings;
        else if (WeaponID == 104002 || WeaponID == 1040021) Preset = &Cloud.S1897_Settings;
        
        if (Preset && Preset->Enabled) {
            // FIX: write to g_ActiveAim — NEVER overwrite Cloud saved globals
            g_ActiveAim.腰射距离 = Preset->HipfireDistance;
            g_ActiveAim.开镜距离 = Preset->ScopeDistance;
            g_ActiveAim.自瞄范围 = Preset->FOVRange;
            g_ActiveAim.腰射速度 = Preset->HipfireSpeed;
            g_ActiveAim.开镜速度 = Preset->ScopeSpeed;
            g_ActiveAim.腰射压枪 = Preset->HipfireRecoil;
            g_ActiveAim.开镜压枪 = Preset->ScopeRecoil;
            g_ActiveAim.预判     = Preset->Prediction;
        } else {
            // No preset for this weapon — use saved global values
            g_ActiveAim.腰射距离 = Cloud.腰射距离;
            g_ActiveAim.开镜距离 = Cloud.开镜距离;
            g_ActiveAim.自瞄范围 = Cloud.自瞄范围;
            g_ActiveAim.腰射速度 = Cloud.腰射速度;
            g_ActiveAim.开镜速度 = Cloud.开镜速度;
            g_ActiveAim.腰射压枪 = Cloud.腰射压枪;
            g_ActiveAim.开镜压枪 = Cloud.开镜压枪;
            g_ActiveAim.预判     = Cloud.预判;
        }
    } else {
        // Automatic mode — always use saved global values
        g_ActiveAim.腰射距离 = Cloud.腰射距离;
        g_ActiveAim.开镜距离 = Cloud.开镜距离;
        g_ActiveAim.自瞄范围 = Cloud.自瞄范围;
        g_ActiveAim.腰射速度 = Cloud.腰射速度;
        g_ActiveAim.开镜速度 = Cloud.开镜速度;
        g_ActiveAim.腰射压枪 = Cloud.腰射压枪;
        g_ActiveAim.开镜压枪 = Cloud.开镜压枪;
        g_ActiveAim.预判     = Cloud.预判;
    }
    
    if(定义.开镜 && 定义.Distance > g_ActiveAim.开镜距离 || !定义.开镜 && 定义.Distance > g_ActiveAim.腰射距离)
        continue;
    
    if (Cloud.IgnoreBots) {
        if (定义.人机判断)
            continue;
    }

    if (Cloud.IgnoreKnocked) {
        if (定义.Health == 0)
            continue;
    }
    if(!定义.开镜) 定义.速度 = g_ActiveAim.腰射速度;
    else 定义.速度 = g_ActiveAim.开镜速度;
    
    if(Cloud.过滤==1) 过滤变量 = 定义.准星;
    else if(Cloud.过滤==2) 过滤变量 = 定义.Distance;
    
if (Cloud.AimLine && 定义.过滤物资 == 479.5 && 定义.对象 != 定义.自身 && 定义.TeamID != 定义.自身队伍 && 定义.TeamID > 0 && 定义.Mesh > 0x0 && 定义.Bone > 0x0 && !定义.死亡 && 定义.Distance > 0 && 定义.Distance < 500) {
    ImVec2 startPoint = ImVec2(屏幕x / 2, 屏幕y / 2);
    ImVec2 endPoint;
    if (Cloud.部位 == 1) {
        endPoint = ImVec2(获取二维骨骼(定义.坐标, 定义.Human, 定义.Bone, 6).x, 获取二维骨骼(定义.坐标, 定义.Human, 定义.Bone, 6).y);
    } else {
        endPoint = ImVec2(获取二维骨骼(定义.坐标, 定义.Human, 定义.Bone, 5).x, 获取二维骨骼(定义.坐标, 定义.Human, 定义.Bone, 5).y);
    }
    if (endPoint.x > 0 && endPoint.x < 屏幕x && endPoint.y > 0 && endPoint.y < 屏幕y) {
        float dx = endPoint.x - startPoint.x;
        float dy = endPoint.y - startPoint.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance <= Cloud.自瞄范围) {
            ImGui::GetForegroundDrawList()->AddLine(ImVec2(屏幕x / 2, 屏幕y / 2),ImVec2(endPoint.x, endPoint.y),ImColor(255, 255, 255, 255),2.1f);
        }
    }
}

    if(定义.准星 <= Cloud.自瞄范围) {
        if(定义.开火 || 定义.战斗状态 && Cloud.喷子 && 识别喷子(定义.武器类型) || Cloud.狙击 && 识别狙击(定义.武器类型) && 定义.开镜)
            定义.自瞄人数++;
        
        if(过滤变量 < 定义.最小距离) {
            定义.最小距离 = 过滤变量;

            if(定义.开火 && !识别喷子(定义.武器类型) && !识别狙击(定义.武器类型)) {
                if(!定义.开镜) 压枪值 = Cloud.腰射压枪 * 定义.Distance / 100 * 识别压枪(定义.手持武器) * 动作压枪(定义.自身状态);
                else 压枪值 = Cloud.开镜压枪 * 定义.Distance / 100 * 矩阵转换(定义.坐标, 定义.矩阵数组).高度 * 识别压枪(定义.手持武器) * 动作压枪(定义.自身状态);
            } else {
                压枪值 = 0;
            }
            if(Cloud.部位 == 0) {
                定义.瞄准.x = 获取二维预判骨骼(定义.坐标, 定义.Human, 定义.Bone, 定义.向量, 定义.载具向量, Cloud.预判, Cloud.载具预判, 定义.Distance, 定义.子弹速度, 定义.上车, 6).x;
                定义.瞄准.y = 获取二维预判骨骼(定义.坐标, 定义.Human, 定义.Bone, 定义.向量, 定义.载具向量, Cloud.预判, Cloud.载具预判, 定义.Distance, 定义.子弹速度, 定义.上车, 6).y + 压枪值;
            } else {
                定义.瞄准.x = 获取二维预判骨骼(定义.坐标, 定义.Human, 定义.Bone, 定义.向量, 定义.载具向量, Cloud.预判, Cloud.载具预判, 定义.Distance, 定义.子弹速度, 定义.上车, 4).x;
                定义.瞄准.y = 获取二维预判骨骼(定义.坐标, 定义.Human, 定义.Bone, 定义.向量, 定义.载具向量, Cloud.预判, Cloud.载具预判, 定义.Distance, 定义.子弹速度, 定义.上车, 4).y + 压枪值;
            }
        }
    }
}}}}
if((定义.开火||Cloud.喷子&&识别喷子(定义.武器类型)||Cloud.狙击&&识别狙击(定义.武器类型)&&定义.开镜)&&定义.自瞄人数>0){
if(屏幕x/2>定义.瞄准.x)
{
定义.差.x=(屏幕x/2)-定义.瞄准.x;
定义.减x=true;
定义.加x=false;
}else if(屏幕x/2< 定义.瞄准.x)
{
定义.差.x=定义.瞄准.x-(屏幕x/2);
定义.加x=true;
定义.减x=false;
}
if(屏幕y/2> 定义.瞄准.y)
{
定义.差.y=(屏幕y/2)-定义.瞄准.y;
定义.加y=true;
定义.减y=false;
}else if(屏幕y/2< 定义.瞄准.y)
{
定义.差.y=定义.瞄准.y-(屏幕y/2);
定义.减y=true;
定义.加y=false;
}
}else{定义.加y=false;定义.减y=false;定义.加x=false;定义.减x=false;}
if(Cloud.自瞄&&!Cloud.隐藏圈){
float 范围;
if(!Cloud.DynamicFOV)范围=Cloud.自瞄范围; else if((定义.开火||Cloud.喷子&&识别喷子(定义.武器类型)||Cloud.狙击&&识别狙击(定义.武器类型)&&定义.开镜)&&定义.自瞄人数>0)范围=定义.最小距离;else 范围=Cloud.自瞄范围;
绘制->AddCircle(ImVec2(屏幕x/2,屏幕y/2),范围,Cloud.自瞄圈颜色,200,2);}

定义.自瞄总数=定义.自瞄人数;

    if (Cloud.Players) {
        auto 真人Size = ImGui::CountTextSize(NULL, to_string((int)定义.真人数量).c_str(), 55);
        auto 人机Size = ImGui::CountTextSize(NULL, to_string((int)定义.人机数量).c_str(), 55);
        绘制->AddText(NULL, 55, ImVec2(屏幕x / 2 - (真人Size.x / 2) - 60, 屏幕y * 0.05), Red, to_string((int)定义.真人数量).c_str());
        绘制->AddText(NULL, 55, ImVec2(屏幕x / 2 - (人机Size.x / 2) + 60, 屏幕y * 0.05), White, to_string((int)定义.人机数量).c_str());
    }

    if (MainWindow.x < 100 && MainWindow.y < 100 && !SmallMenuToggle) {
        static float 颜色[5] = {255, 255, 0, 0, 255};
        static bool 加[5] = {false, false, false, false, false};

        if (颜色[1] >= 169)
            加[1] = false;
        else if (颜色[1] < 0)
            加[1] = true;

        if (颜色[2] >= 120)
            加[2] = false;
        else if (颜色[2] < 0)
            加[2] = true;

        if (颜色[3] >= 223)
            加[3] = false;
        else if (颜色[3] < 0)
            加[3] = true;

        if (颜色[4] >= 255)
            加[4] = false;
        else if (颜色[4] < 200)
            加[4] = true;

        if (加[1])
            颜色[1] += 50 / ImGui::GetIO().Framerate;
        else
            颜色[1] -= 50 / ImGui::GetIO().Framerate;

        if (加[2])
            颜色[2] += 50 / ImGui::GetIO().Framerate;
        else
            颜色[2] -= 50 / ImGui::GetIO().Framerate;

        if (加[3])
            颜色[3] += 50 / ImGui::GetIO().Framerate;
        else
            颜色[3] -= 50 / ImGui::GetIO().Framerate;

        if (加[4])
            颜色[4] += 50 / ImGui::GetIO().Framerate;
        else
            颜色[4] -= 50 / ImGui::GetIO().Framerate;

        auto Size = ImGui::CountTextSize(NULL, "NovaX", 40);
        绘制->AddText(NULL, 40, ImVec2(Cloud.PillPositionX + 100 - Size.x / 2, Cloud.PillPositionY + 37.5 - Size.y / 2), ImColor(139, 0, 0, 255), "NovaX");
    }
return;
}
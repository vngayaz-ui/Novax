#include <Funs.h>
#include <draw.h>
#include <Kernel.hpp>
#include <Main.h>
#include <regex>
#include <imgui.h>
#include "Definitions.h"
#include "SDK.hpp"
#include <sched.h>
#include <unistd.h>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <map>
#include <chrono>

uintptr_t 矩阵头,自身头;

#define package com_tencent_ig

#define UlocalPlayerBGMI 0xe96e270
#define MatrixBGMI 0xe966610

#define UlocalPlayerGL 0xF1D5F70
#define MatrixGL 0xF1CE310

#define UlocalPlayerVN 0xF1D5F70
#define MatrixVN 0xF1CE310

#define 坐标指针 package::SceneComponent::ComponentVelocity-0xA0
#define 坐标解密指针 package::STExtraBaseCharacter::MeshContainer
#define Human指针 坐标指针-0x10
#define 坐标解密地址 0x1c8
#define 开发者偏移量 0x670

#define 投掷物代码指针 package::STExtraGrenadeBase::ItemDefineID+package::ItemDefineID::TypeSpecificID
#define 物资代码指针 package::PickUpWrapperActor::DefineID+package::ItemDefineID::TypeSpecificID
#define 载具向量指针 package::Actor::ReplicatedMovement+package::RepMovement::LinearVelocity
#define Uworld package::Player::PlayerController
#define Ulevel package::LevelCollection::PersistentLevel
#define 死亡指针 package::STExtraCharacter::bDead
#define 上车指针 package::STExtraBaseCharacter::bIsAttachedToVehicle
#define 战斗状态指针 package::STExtraBaseCharacter::bInBattleState
#define 当前载具指针 package::STExtraCharacter::CurrentVehicle
#define 过滤载具指针 package::STExtraVehicleBase::iRegionVehicle
#define 车辆常见指针 package::STExtraVehicleBase::VehicleCommon
#define 载具类型指针 package::STExtraVehicleBase::VehicleShapeType
#define 车油指针 package::VehicleCommonComponent::Fuel
#define 车油最大指针 package::VehicleCommonComponent::FuelMax
#define 车血指针 package::VehicleCommonComponent::HP
#define 车血最大指针 package::VehicleCommonComponent::HPMax
#define 自身指针 package::STExtraPlayerController::STExtraBaseCharacter
#define 队伍指针 package::UAECharacter::TeamID
#define 开火指针 package::STExtraBaseCharacter::bIsWeaponFiring
#define 开镜指针 package::STExtraCharacter::bIsGunADS
#define 运动组件指针 package::Character::CharacterMovement
#define 根指针 package::Actor::RootComponent
#define 动作指针 package::STExtraCharacter::CurrentStates
#define 状态指针 package::STExtraBaseCharacter::PoseState
#define 名字指针 package::UAECharacter::PlayerName
#define UID指针 package::UAECharacter::PlayerUID
#define Nation package::UAECharacter::Nation
#define 人机指针 package::UAECharacter::bEnsure
#define 隐藏指针 package::Actor::bHidden
#define 高级人机指针 package::UAECharacter::bMEnsure
#define 高级人机指针2 package::UAECharacter::bIsAI
#define 高级人机指针3 package::UAECharacter::bIsMLAI
#define 血量指针 package::STExtraCharacter::Health
#define 最大血量指针 package::STExtraCharacter::HealthMax
#define 倒地血量指针 package::STExtraBaseCharacter::NearDeathBreath
#define 倒地组件 package::STExtraBaseCharacter::NearDeatchComponent
#define 倒地最大血量指针 package::STCharacterNearDeathComp::BreathMax
#define Mesh指针 package::Character::Mesh
#define Bone指针 package::StaticMeshComponent::StaticMesh
#define 向量指针 package::SceneComponent::ComponentVelocity
#define 旋转指针 package::SceneComponent::RelativeRotation+package::Rotator::Yaw
#define 过滤物资指针 package::STExtraBaseCharacter::HighWalkSpeed
#define 武器经理 package::STExtraBaseCharacter::WeaponManagerComponent
#define 当前武器 package::WeaponManagerComponent::CurrentWeaponReplicated
#define 武器实体 package::STExtraWeapon::WeaponEntityComp
#define 武器类型指针 package::WeaponEntity::WeaponTypeNew
#define 武器id指针 package::WeaponEntity::WeaponId
#define 子弹数量指针 package::STExtraShootWeapon::CurBulletInClip
#define 子弹最大指针 package::STExtraShootWeapon::CurMaxBulletNumInOneClip
#define 子弹速度指针 package::ShootWeaponEntity::BulletFireSpeed
#define 聚点指针 package::ShootWeaponEntity::GameDeviationFactor
#define 无后指针 package::ShootWeaponEntity::AccessoriesVRecoilFactor
#define 网络连接对象 package::PlayerController::NetConnection
#define 网络Driver模块 package::NetConnection::Driver
#define 世界 package::NetDriver::World
#define 游戏状态 package::World::GameState
#define 真人数量指针 package::STExtraGameStateBase::PlayerNum
#define 玩家数量指针 package::STExtraGameStateBase::AlivePlayerNum
#define 队伍数量指针 package::STExtraGameStateBase::AliveTeamNum
#define 三称视角组件 package::STExtraBaseCharacter::ThirdPersonCameraComponent
#define 一称视角组件 package::STExtraBaseCharacter::FPPCameraComp
#define 开镜视角组件 package::STExtraBaseCharacter::ScopeSpringArmComp
#define 相机视角组件 package::STExtraBaseCharacter::SpringArmComp
#define 广角指针 package::CameraComponent::FieldOfView

pthread_t touch_tid;
bool Start=false;
bool Window=false,isVNG=false,isBGMI=false,更新=false,开发者忽略无效=false,MainMenuSwitch=false,SmallMenuToggle=false;
int 读写方式=0;
std::string input,更新内容字符,新版本字符,继续字符,退出字符,选择模式字符,插件字符,语言字符;
int 页面=2,androidVersion;
ImVec2 主菜单位置={200,200},SmallMenuPosition={200,200},人数位置,MainWindow={0,0};
float 压枪值,kernelVersion,过滤变量;
// AimMode: 0=Automatic (global sliders), 1=Manual (per-weapon presets)
// Shared between Menu_Ui.hpp (Combo) and Main.cpp (apply presets) — BUG-24 fix
int g_AimMode = 0;

std::map<std::string, std::chrono::steady_clock::time_point> rescueTimers;
std::map<std::string, std::chrono::steady_clock::time_point> 自救Timers;

ImColor 队伍变色[7]={ImColor(0,255,255,200),ImColor(255,200,250,200),ImColor(0,50,255,200),ImColor(255,255,255,200),ImColor(255,255,50,200),ImColor(169,120,223,200),ImColor(0,255,50,200)},半蓝色=ImColor(0,0,255,150),半品红=ImColor(255,0,255,150),白色=ImColor(255,255,255),半白色=ImColor(200,200,200),Red=ImColor(150,0,0),半绿色=ImColor(0,255,0,50),半浅蓝=ImColor(0,0,255,50),浅粉=ImColor(255,200,250,242), Black=ImColor(0,0,0,255),半黑=ImColor(0,0,0,46),血色=ImColor(0,249,0,89),黄色=ImColor(255,255,0,255),半黄色=ImColor(255,255,0,150),半橘黄=ImColor(255,153,0,150),粉红=ImColor(255,192,203,255),紫色=ImColor(169,120,223,242),半青绿=ImColor(0,255,192,150),蓝色=ImColor(0,0,255),品红=ImColor(255,0,255), White=ImColor(255,255,255), 浅蓝=ImColor(0,255,255),橘黄=ImColor(255,153,0),青绿=ImColor(0,255,192),深灰=ImColor(100,100,100),半深灰=ImColor(100,100,100,150),浅灰=ImColor(230,230,230),半浅灰=ImColor(230,230,230,150),深红=ImColor(178,34,34),半深红=ImColor(178,34,34,150),深绿=ImColor(0,100,0),半深绿=ImColor(0,100,0,150),深蓝=ImColor(0,0,139),半深蓝=ImColor(0,0,139,150),深紫=ImColor(128,0,128),半深紫=ImColor(128,0,128,150),深黄=ImColor(204,204,0),半深黄=ImColor(204,204,0,150),橙色=ImColor(255,165,0),半橙色=ImColor(255,165,0,150),玫瑰红=ImColor(255,0,127),半玫瑰红=ImColor(255,0,127,150),天蓝=ImColor(135,206,235),半天蓝=ImColor(135,206,235,150),薄荷绿=ImColor(152,251,152),半薄荷绿=ImColor(152,251,152,150),紫罗兰=ImColor(138,43,226),半紫罗兰=ImColor(138,43,226,150),琥珀色=ImColor(255,191,0),半琥珀色=ImColor(255,191,0,150),珊瑚色=ImColor(255,127,80),半珊瑚色=ImColor(255,127,80,150),翡翠绿=ImColor(0,255,127),半翡翠绿=ImColor(0,255,127,150),靛蓝色=ImColor(75,0,130),半靛蓝色=ImColor(75,0,130,150);
ImColor HellRed = ImColor(220, 20, 60, 255);
ImColor HellOrange = ImColor(255, 69, 0, 255);
ImColor HellDarkRed = ImColor(139, 0, 0, 255);
ImColor HellGlow = ImColor(255, 50, 50, 200);
ImColor HellBorder = ImColor(255, 30, 30, 255);
ImColor HellText = ImColor(255, 100, 50, 255);
ImColor HellBg = ImColor(20, 0, 0, 220);
ImColor HellButton = ImColor(80, 0, 0, 255);
ImColor HellButtonActive = ImColor(180, 30, 30, 255);

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

struct 配置 {
    bool Players = true, Line = false, Box = false, Skeleton = false, Health = false, EnemyLanding = false;
    bool TeamID = false, Distance = false, Name = false, Grenade = false;
    bool Weapon = false, Items = false, TouchPosition = false, 自瞄 = false, AimLine = false,IgnoreBots = false, IgnoreSmoke = false;
    bool IgnoreKnocked = false, 开火 = true, 喷子 = false, 狙击 = false, HideBots = false;
    bool Alert = false, Vehicle = false, DynamicFOV = false, 防录屏 = false, 广角 = false;
    bool 聚点 = false, 无后 = false, Background = false, TimerRevive = false, Decryption = false;
    bool 隐藏圈 = false, HandcamMode = false;

    bool AutoClicker = false;
    bool AutoClicker2 = false;
    float ClickPositionX = -1.0f;
    float ClickPositionY = -1.0f;
    float ClickRadius = 200.0f;
    float ClickSpeed = 0.3f;
    
    float TouchRate = 1000.0f, 腰射压枪 = 0.5f, 开镜压枪 = 1.3f, 自瞄范围 = 180.0f;
    float 开镜速度 = 1.0f, 腰射速度 = 1.0f, 开镜距离 = 500.0f, 腰射距离 = 50.0f;
    float 预判 = 0.75f, 载具预判 = 0.3f, 目标帧率 = 120.0f;
    float 三称广角大小 = 90.0f, 一称广角大小 = 103.0f, 开镜广角大小 = 5.0f;
    float 相机广角大小 = 220.0f, 方框粗细 = 2.0f, BoxAlpha = 0.3f;

    ImVec2 Touch = {150, 150};
    ImColor PlayerLineColor = ImColor(150, 0, 0);
    ImColor BotLineColor = ImColor(255, 255, 255, 255);
    ImColor PlayerBoxColor = ImColor(255, 0, 0, 255);
    ImColor BotBoxColor = ImColor(255, 255, 255, 255);
    ImColor PlayerHealthColor = ImColor(255, 0, 0, 255);
    ImColor BotHealthColor = ImColor(0, 255, 0, 255);
    ImColor TeamIDColor = ImColor(255, 255, 255, 255);
    ImColor 载具颜色 = ImColor(255, 255, 255, 255);
    ImColor 自瞄圈颜色 = White;

    // Attachment
    bool ScopeX3 = false, ScopeX4 = false, ScopeX6 = false, ScopeX8 = false, ExtendedMagAR = false, QuickDrawMagAR = false, ExtendedQuickDrawMagAR = false;
    // Weapons
    bool M416 = false, M16A4 = false, SCARL = false, AKM = false, AUG = false, QBZ = false, G36C = false, M762 = false, MK47 = false, ACE32 = false, UMP = false, Vector = false, UZI = false, TommyGun = false, Bison = false, MP5K = false, P90 = false, JS9 = false, S12K = false, S1897 = false, S686 = false, DBS = false, NS2000 = false, Kar98K = false, M24 = false, Win94 = false, Mosin = false, Mini14 = false, SKS = false, SLR = false, QBU = false, Mk12 = false, VSS = false, DP28 = false, M249 = false, Pan = false, Flare = false;    
    // Vehicles
    bool Buggy = false, Dacia = false, CoupeRB = false, UAZOpen = false, UAZClosed = false, Mirado = false, PickupOpen = false, PickupClosed = false, Van = false, Rony = false, Tukshai = false, Scooter = false, MonsterTruck = false, MotorGlider = false, Snowmobile = false, Zima = false, PG117 = false, JetSki = false, BRDM2 = false, UTV = false, Roadster = false;
    // Consumables
    bool AdrenalineSyringe = false, Bandage = false, EnergyDrink = false, FirstAidKit = false, MedKit = false, Painkillers = false, SelfAED = false;
    // Throwables
    bool FragGrenade = false, SmokeGrenade = false, MolotovCocktail = false, StickyBomb = false, StunGrenade = false;
    // Equipments
    bool PoliceVest1 = false, PoliceVest2 = false, MilitaryVest3 = false, MotorcycleHelmet1 = false, MilitaryHelmet2 = false, SpetsnazHelmet3 = false, Backpack1 = false, Backpack2 = false, Backpack3 = false;
    // Ammo
    bool Gauge12 = false, Magnum300 = false, ACP45 = false, MM556 = false, MM762 = false, MM9 = false, Bolt = false;
     
    // NOTE: const removed — binary fread/fwrite requires non-const members
    float tm = 0.5f;
    ImVec4 teamColors[20] = {
        {144/255.f, 238/255.f, 144/255.f, 0.5f}, {135/255.f, 206/255.f, 255/255.f, 0.5f},
        {255/255.f, 0/255.f,   0/255.f,   0.5f}, {0/255.f,   255/255.f, 0/255.f,   0.5f},
        {0/255.f,   255/255.f, 150/255.f, 0.5f}, {255/255.f, 182/255.f, 193/255.f, 0.5f},
        {218/255.f, 112/255.f, 214/255.f, 0.5f}, {248/255.f, 248/255.f, 255/255.f, 0.5f},
        {0/255.f,   255/255.f, 255/255.f, 0.5f}, {255/255.f, 165/255.f, 0/255.f,   0.5f},
        {153/255.f, 204/255.f, 255/255.f, 0.5f}, {204/255.f, 255/255.f, 153/255.f, 0.5f},
        {255/255.f, 255/255.f, 153/255.f, 0.5f}, {255/255.f, 153/255.f, 153/255.f, 0.5f},
        {153/255.f, 153/255.f, 204/255.f, 0.5f}, {204/255.f, 204/255.f, 204/255.f, 0.5f},
        {102/255.f, 204/255.f, 153/255.f, 0.5f}, {255/255.f, 102/255.f, 0/255.f,   0.5f},
        {102/255.f, 204/255.f, 204/255.f, 0.5f}, {153/255.f, 204/255.f, 255/255.f, 0.5f}
    };

    // 0 = Head, 1 = Chest (was wrongly 1/2 before)
    int 部位 = 0, 过滤 = 1;

    WeaponAimSettings M416_Settings, AKM_Settings, M24_Settings, S12K_Settings, UMP45_Settings, Bison_Settings;
    WeaponAimSettings M762_Settings, SCAR_L_Settings, AUG_Settings, DP28_Settings, M249_Settings, Groza_Settings;
    WeaponAimSettings AWM_Settings, Kar98k_Settings, Mosin_Settings, MK14_Settings, Mini14_Settings, SLR_Settings, SKS_Settings;
    WeaponAimSettings Vector_Settings, TommyGun_Settings, MP5K_Settings, MP9_Settings, P90_Settings;
    WeaponAimSettings DBS_Settings, S686_Settings, S1897_Settings;
};

// numSave removed — each save/load opens and closes its own handle (see SaveConfig/ReadConfig)
配置 Cloud;

struct ActorsEncryption {
    uint32_t Enc_1,Enc_2;
    uint32_t Enc_3,Enc_4;
};

struct Encryption_Chunk {
    uint32_t val_1,val_2;
    uint32_t val_3,val_4;
};

struct Actors {
    uint64_t Enc_1,Enc_2;
    uint64_t Enc_3,Enc_4;
};

struct Chunk {
    uint32_t val_1,val_2,val_3,val_4;
    uint32_t val_5,val_6,val_7,val_8;
};

uint64_t 数组解密(uint64_t uLevel,int Actors_Offset,int EncryptedActors_Offset) {
    if(uLevel<0x10000000)
        return 0;
    if(Driver.Read<uint64_t>(uLevel+Actors_Offset)>0)
        return uLevel+Actors_Offset;
    if(Driver.Read<uint64_t>(uLevel+EncryptedActors_Offset)>0)
        return uLevel+EncryptedActors_Offset;
    auto AActors=Driver.Read<Actors>(uLevel+EncryptedActors_Offset+0x10);
    if(AActors.Enc_1>0) {
        auto Enc=Driver.Read<Chunk>(AActors.Enc_1+0x80);
        return (((Driver.Read<uint8_t>(AActors.Enc_1+Enc.val_1)
            | (Driver.Read<uint8_t>(AActors.Enc_1+Enc.val_2) << 8))
            | (Driver.Read<uint8_t>(AActors.Enc_1+Enc.val_3) << 0x10)) & 0xFFFFFF
            | ((uint64_t)Driver.Read<uint8_t>(AActors.Enc_1+Enc.val_4) << 0x18)
            | ((uint64_t)Driver.Read<uint8_t>(AActors.Enc_1+Enc.val_5) << 0x20)) & 0xFFFF00FFFFFFFFFF
            | ((uint64_t)Driver.Read<uint8_t>(AActors.Enc_1+Enc.val_6) << 0x28)
            | ((uint64_t)Driver.Read<uint8_t>(AActors.Enc_1+Enc.val_7) << 0x30)
            | ((uint64_t)Driver.Read<uint8_t>(AActors.Enc_1+Enc.val_8) << 0x38);
    }
    else if(AActors.Enc_2>0) {
        auto Lost_Actors=Driver.Read<uint64_t>(AActors.Enc_2);
        if(Lost_Actors>0) {
            return (uint16_t)(Lost_Actors-0x400) & 0xFF00
                | (uint8_t)(Lost_Actors-0x04)
                | (Lost_Actors+0xFC0000) & 0xFF0000
                | (Lost_Actors-0x4000000) & 0xFF000000
                | (Lost_Actors+0xFC00000000) & 0xFF00000000
                | (Lost_Actors+0xFC0000000000) & 0xFF0000000000
                | (Lost_Actors+0xFC000000000000) & 0xFF000000000000
                | (Lost_Actors-0x400000000000000) & 0xFF00000000000000;
        }
    }
    else if(AActors.Enc_3>0) {
        auto Lost_Actors=Driver.Read<uint64_t>(AActors.Enc_3);
        if(Lost_Actors>0) {
            return (Lost_Actors >> 0x38) | (Lost_Actors << (64-0x38));
        }
    }
    else if(AActors.Enc_4>0) {
        auto Lost_Actors=Driver.Read<uint64_t>(AActors.Enc_4);
        if(Lost_Actors>0) {
            return Lost_Actors ^ 0xCDCD00;
        }
    }
    return 0;
}

void 更新救人倒计时() {
    auto it = rescueTimers.begin();
    while (it != rescueTimers.end()) {
        auto elapsed = std::chrono::duration_cast < std::chrono::seconds > (
        std::chrono::steady_clock::now() - it-> second
        ).count();
        if (elapsed >= 10) it = rescueTimers.erase(it);
        else ++ it;
    }
}

void 更新自救倒计时() {
    auto it = 自救Timers.begin();
    while (it != 自救Timers.end()) {
        auto elapsed = std::chrono::duration_cast < std::chrono::seconds > (
        std::chrono::steady_clock::now() - it-> second
        ).count();
        if (elapsed >= 10) it = 自救Timers.erase(it);
        else ++ it;
    }
}

void drawCountdownCircle(float MIDDLE, float top, int remainingTime, int totalTime, ImColor progressColor) {
    ImGui::GetForegroundDrawList()->AddCircleArc(ImVec2(MIDDLE, top - 74), 20, {0, 360},ImColor(255, 255, 255, 100), 0, 3);
    float percentage = (float)remainingTime / totalTime;
    float angle = percentage * 360.0f;
    ImGui::GetForegroundDrawList()->AddCircleArc(ImVec2(MIDDLE, top - 74), 20, {0, angle},progressColor, 0, 3);
    std::string timeText = std::to_string(remainingTime);
    auto textSize = ImGui::CalcTextSize(timeText.c_str(), 0, 20);
    ImGui::GetForegroundDrawList()->AddText(NULL, 50,ImVec2(MIDDLE - (textSize.x / 2), top - 74 - 10),ImColor(255, 0, 0, 255), timeText.c_str());
}

android::ANativeWindowCreator::DisplayInfo displayInfo;

void* 分辨率线程(void *) {
    while(定义.Thread){
        ::displayInfo=android::ANativeWindowCreator::GetDisplayInfo();
        android::ANativeWindowCreator::ProcessMirrorDisplay();
        方向=displayInfo.方向;
        屏幕x=displayInfo.width;
        屏幕y=displayInfo.height;
        sleep(1);
    }
    return NULL;
}

void CreateResolutionThread() {
    pthread_t tid;
    if(pthread_create(&tid,NULL,分辨率线程,NULL) != 0) {
        exit(1);
    }
    pthread_detach(tid);
    return;
}

int GetAndroidVersion(){
    char prop_value[PROP_VALUE_MAX];
    __system_property_get("ro.build.version.release",prop_value);
    return atoi(prop_value);
}

float GetKernelVersion() {
    const char* command="uname -r | sed 's/\\.[^.]*$//g'";
    FILE* file=popen(command,"r");
    if(file==NULL) {
        return NULL;
    }
    static char result[512];
    if(fgets(result,sizeof(result),file)==NULL) {
        return NULL;
    }
    pclose(file);
    result[strlen(result)-1]='\0';
    return atof(result);
}

int GetModuleBase(){
   定义.initialize=true;
   if(Driver.getPID("com.tencent.ig")!=-1)
       定义.pid=Driver.getPID("com.tencent.ig");
   else if(Driver.getPID("com.pubg.krmobile")!=-1)
       定义.pid=Driver.getPID("com.pubg.krmobile");
   else if(Driver.getPID("com.rekoo.pubgm")!=-1)
       定义.pid=Driver.getPID("com.rekoo.pubgm");
   else if(Driver.getPID("com.vng.pubgmobile")!=-1)
       定义.pid=Driver.getPID("com.vng.pubgmobile");
   else if(Driver.getPID("com.pubg.imobile")!=-1)
       定义.pid=Driver.getPID("com.pubg.imobile");
   if(定义.pid==Driver.getPID("com.pubg.imobile"))
       isBGMI=true;
   else
       isBGMI=false;
   if(定义.pid==Driver.getPID("com.vng.pubgmobile"))
       isVNG=true;
   else
       isVNG=false;
   Driver.initialize(定义.pid);
   if(GetKernelVersion()<6.0)
   定义.UE4=Driver.获取模块头(定义.pid,"libUE4.so");
   else
   定义.UE4=Driver.获取模块头2(定义.pid,"libUE4.so");
   return 1;
}

std::string getUTF(UTF16 *pTempUTF16) {
    char utf8Buffer[32];
    UTF8 *pTempUTF8=utf8Buffer;
    UTF8 *pUTF8End=pTempUTF8+32;
    while (pTempUTF16<pTempUTF16+28) {
        if(*pTempUTF16<=0x007F&&pTempUTF8+1<pUTF8End) {
            *pTempUTF8++=(UTF8)*pTempUTF16;
        }else if(*pTempUTF16>=0x0080&&*pTempUTF16<=0x07FF&&pTempUTF8+2<pUTF8End) {
            *pTempUTF8++=(*pTempUTF16 >> 6) | 0xC0;
            *pTempUTF8++=(*pTempUTF16 & 0x3F) | 0x80;
        }else if(*pTempUTF16>=0x0800&&*pTempUTF16<=0xFFFF&&pTempUTF8+3<pUTF8End) {
            *pTempUTF8++=(*pTempUTF16 >> 12) | 0xE0;
            *pTempUTF8++=((*pTempUTF16 >> 6) & 0x3F) | 0x80;
            *pTempUTF8++=(*pTempUTF16 & 0x3F) | 0x80;
        }else {
            break;
        }
        pTempUTF16++;
    }
    return utf8Buffer;
}

// Config paths — primary on /data/local/tmp (survives reboots, no sdcard permission needed),
// fallback to /sdcard. Both are tried on read so old configs still load.
static const char* CONFIG_PATH_PRIMARY  = "/data/local/tmp/NovaXConfig.bin";
static const char* CONFIG_PATH_FALLBACK = "/sdcard/NovaXConfig.bin";
static const char* CONFIG_PATH_TEMP     = "/data/local/tmp/NovaXConfig.tmp";

static constexpr uint32_t CONFIG_MAGIC   = 0x4E584346;  // "NXCF"
static constexpr uint32_t CONFIG_VERSION = 3;

struct ConfigHeader {
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint32_t checksum;
};

static uint32_t CalcChecksum(const void* data, size_t len) {
    const uint8_t* p = (const uint8_t*)data;
    uint32_t sum = 0x12345678;
    for (size_t i = 0; i < len; i++) {
        sum ^= p[i];
        sum = (sum << 7) | (sum >> 25);
        sum += 0x9E3779B9;
    }
    return sum;
}

void SaveConfig(const char* /*name_unused*/) {
    // Write to temp file first, then rename — atomic on most filesystems
    FILE* f = fopen(CONFIG_PATH_TEMP, "wb");
    if (!f) f = fopen(CONFIG_PATH_PRIMARY, "wb");
    if (!f) return;

    ConfigHeader hdr{};
    hdr.magic = CONFIG_MAGIC;
    hdr.version = CONFIG_VERSION;
    hdr.size = sizeof(Cloud);
    hdr.checksum = CalcChecksum(&Cloud, sizeof(Cloud));

    fwrite(&hdr, sizeof(hdr), 1, f);
    fwrite(&Cloud, sizeof(Cloud), 1, f);
    fflush(f);
    fsync(fileno(f));
    fclose(f);

    // Atomic rename (same filesystem)
    rename(CONFIG_PATH_TEMP, CONFIG_PATH_PRIMARY);
}

void ReadConfig(const char* /*name_unused*/) {
    const char* paths[] = { CONFIG_PATH_PRIMARY, CONFIG_PATH_FALLBACK };
    for (const char* path : paths) {
        FILE* f = fopen(path, "rb");
        if (!f) continue;

        ConfigHeader hdr{};
        size_t readCount = fread(&hdr, sizeof(hdr), 1, f);
        if (readCount != 1 || hdr.magic != CONFIG_MAGIC || hdr.version != CONFIG_VERSION || hdr.size != sizeof(Cloud)) {
            fclose(f);
            continue;  // wrong version or corrupt — try next path
        }

        配置 tmp{};
        readCount = fread(&tmp, sizeof(tmp), 1, f);
        fclose(f);
        if (readCount != 1) continue;

        uint32_t sum = CalcChecksum(&tmp, sizeof(tmp));
        if (sum != hdr.checksum) continue;  // checksum mismatch — try next path

        Cloud = tmp;
        return;  // success
    }
    // No valid config found — Cloud keeps its compile-time defaults
}

float 识别压枪(int 枪械id) {
    if(枪械id==101008||枪械id==1010081||枪械id==1010082||枪械id==1010083||枪械id==1010084||枪械id==1010085||枪械id==1010086||枪械id==1010087||枪械id==101102||枪械id==1011021||枪械id==1011022||枪械id==1011023||枪械id==1011024||枪械id==1011025||枪械id==1011026||枪械id==1011027||枪械id==101001||枪械id==1010011||枪械id==1010012||枪械id==1010013||枪械id==1010014||枪械id==1010015||枪械id==1010016||枪械id==1010017||枪械id==101005||枪械id==1010051||枪械id==1010052||枪械id==1010053||枪械id==1010054||枪械id==1010055||枪械id==1010056||枪械id==1010057) return 1.4;
    else if(枪械id==102105||枪械id==1021051||枪械id==1021052||枪械id==1021053||枪械id==1021054||枪械id==1021055||枪械id==1021056||枪械id==1021057) return 0.1;
    else if(枪械id==105010||枪械id==1050103||枪械id==1050104||枪械id==1050105||枪械id==1050106||枪械id==1050107||枪械id==1050109||枪械id==102001||枪械id==102002||枪械id==1020011||枪械id==1020012||枪械id==1020013||枪械id==1020014||枪械id==1020015||枪械id==1020021||枪械id==1020022||枪械id==1020023||枪械id==1020024||枪械id==1020025||枪械id==1020029||枪械id==102008||枪械id==1020081||枪械id==1020082||枪械id==1020083||枪械id==1020084||枪械id==1020085||枪械id==1020086||枪械id==1020087||枪械id==102901||枪械id==102007||枪械id==102003||枪械id==102004||枪械id==102005||枪械id==102007||枪械id==102901||枪械id==102903||枪械id==1020032||枪械id==1020033||枪械id==1020034||枪械id==1020035||枪械id==1020036||枪械id==1020037||枪械id==1020042||枪械id==1020043||枪械id==1020044||枪械id==1020052||枪械id==1020053||枪械id==1020054||枪械id==1020059||枪械id==1020072||枪械id==1020073||枪械id==1020074||枪械id==1020075||枪械id==1020076||枪械id==1020077) return 0.7;
    else if(枪械id==101101||枪械id==1011011||枪械id==1011012||枪械id==1011013||枪械id==1011014||枪械id==1011015||枪械id==1011016||枪械id==1011017||枪械id==105002||枪械id==1050021||枪械id==1050022||枪械id==1050023||枪械id==1050024||枪械id==1050025||枪械id==1050026||枪械id==1050027||枪械id==103005||枪械id==1030052||枪械id==1030053||枪械id==1030054||枪械id==1030055||枪械id==1030059) return 0.4;
    else if(枪械id==1010042||枪械id==1010043||枪械id==1010044||枪械id==1010045||枪械id==1010046||枪械id==1010047||枪械id==1010049||枪械id==101004) return 1.1;
    else return 1;
}

bool 识别喷子(unsigned char 枪械类型) {
    if(枪械类型==5)
        return true;
    else return false;
}

bool 识别狙击(unsigned char 枪械类型) {
    if(枪械类型==2||枪械类型==9||枪械类型==7)
        return true;
    else return false;
}

GLuint 识别枪械(int 枪械id) {
    if(枪械id==101001||枪械id==1010011||枪械id==1010012||枪械id==1010013||枪械id==1010014||枪械id==1010015||枪械id==1010016||枪械id==1010017||枪械id==1010019||枪械id==101901)
        return LoadWeaponTexture(枪械id, picture_101001_png, sizeof(picture_101001_png));
    else if(枪械id==101002||枪械id==1010021||枪械id==1010022||枪械id==1010023||枪械id==1010024||枪械id==1010025)
        return LoadWeaponTexture(枪械id, picture_101002_png, sizeof(picture_101002_png));
    else if(枪械id==101003||枪械id==1010031||枪械id==1010032||枪械id==1010033||枪械id==1010034||枪械id==1010035||枪械id==1010036||枪械id==1010037||枪械id==1010039||枪械id==101903)
        return LoadWeaponTexture(枪械id, picture_101003_png, sizeof(picture_101003_png));
    else if(枪械id==101004||枪械id==1010041||枪械id==1010042||枪械id==1010043||枪械id==1010044||枪械id==1010045||枪械id==1010046||枪械id==1010047||枪械id==1010049)
        return LoadWeaponTexture(枪械id, picture_101004_png, sizeof(picture_101004_png));
    else if(枪械id==101005||枪械id==1010051||枪械id==1010052||枪械id==1010053||枪械id==1010054||枪械id==1010055||枪械id==1010056||枪械id==1010057||枪械id==1010059)
        return LoadWeaponTexture(枪械id, picture_101005_png, sizeof(picture_101005_png));
    else if(枪械id==101006||枪械id==1010061||枪械id==1010062||枪械id==1010063||枪械id==1010064||枪械id==1010065||枪械id==1010066||枪械id==1010067)
        return LoadWeaponTexture(枪械id, picture_101006_png, sizeof(picture_101006_png));
    else if(枪械id==101007||枪械id==1010071||枪械id==1010072||枪械id==1010073||枪械id==1010074||枪械id==1010075||枪械id==1010076||枪械id==1010077)
        return LoadWeaponTexture(枪械id, picture_101007_png, sizeof(picture_101007_png));
    else if(枪械id==101008||枪械id==1010081||枪械id==1010082||枪械id==1010083||枪械id==1010084||枪械id==1010085||枪械id==1010086||枪械id==1010087||枪械id==1010089||枪械id==101908)
        return LoadWeaponTexture(枪械id, picture_101008_png, sizeof(picture_101008_png));
    else if(枪械id==101009||枪械id==1010091||枪械id==1010092||枪械id==1010093||枪械id==1010094||枪械id==1010095)
        return LoadWeaponTexture(枪械id, picture_101009_png, sizeof(picture_101009_png));
    else if(枪械id==101010||枪械id==1010101||枪械id==1010102||枪械id==1010103||枪械id==1010104||枪械id==1010105||枪械id==1010106||枪械id==1010107)
        return LoadWeaponTexture(枪械id, picture_101010_png, sizeof(picture_101010_png));
    else if(枪械id==101011)
        return LoadWeaponTexture(枪械id, picture_101011_png, sizeof(picture_101011_png));
    else if(枪械id==1010121||枪械id==1010122||枪械id==1010123||枪械id==1010124||枪械id==1010125||枪械id==1010126||枪械id==1010127)
        return LoadWeaponTexture(枪械id, picture_101012_png, sizeof(picture_101012_png));
    else if(枪械id==101100||枪械id==1011001||枪械id==1011002||枪械id==1011003||枪械id==1011004||枪械id==1011005||枪械id==1011006||枪械id==1011007)
        return LoadWeaponTexture(枪械id, picture_101101_png, sizeof(picture_101101_png));
    else if(枪械id==101102||枪械id==1011021||枪械id==1011022||枪械id==1011023||枪械id==1011024||枪械id==1011025||枪械id==1011026||枪械id==1011027)
        return LoadWeaponTexture(枪械id, picture_101102_png, sizeof(picture_101102_png));
    else if(枪械id==102001||枪械id==1020011||枪械id==1020012||枪械id==1020013||枪械id==1020014||枪械id==1020015||枪械id==102901)
        return LoadWeaponTexture(枪械id, picture_102001_png, sizeof(picture_102001_png));
    else if(枪械id==102002||枪械id==1020021||枪械id==1020022||枪械id==1020023||枪械id==1020024||枪械id==1020025||枪械id==1020029)
        return LoadWeaponTexture(枪械id, picture_102002_png, sizeof(picture_102002_png));
    else if(枪械id==102003||枪械id==1020031||枪械id==1020032||枪械id==1020033||枪械id==1020034||枪械id==1020035||枪械id==1020036||枪械id==1020037||枪械id==102903)
        return LoadWeaponTexture(枪械id, picture_102003_png, sizeof(picture_102003_png));
    else if(枪械id==102004||枪械id==1020041||枪械id==1020042||枪械id==1020043||枪械id==1020044||枪械id==1020045)
        return LoadWeaponTexture(枪械id, picture_102004_png, sizeof(picture_102004_png));
    else if(枪械id==102005||枪械id==1020051||枪械id==1020052||枪械id==1020053||枪械id==1020054||枪械id==1020055||枪械id==1020059)
        return LoadWeaponTexture(枪械id, picture_102005_png, sizeof(picture_102005_png));
    else if(枪械id==102007||枪械id==1020071||枪械id==1020072||枪械id==1020073||枪械id==1020074||枪械id==1020075||枪械id==1020076||枪械id==1020077)
        return LoadWeaponTexture(枪械id, picture_102007_png, sizeof(picture_102007_png));
    else if(枪械id==102008||枪械id==1020081||枪械id==1020082||枪械id==1020083||枪械id==1020084||枪械id==1020085||枪械id==1020086||枪械id==1020087)
        return LoadWeaponTexture(枪械id, picture_102008_png, sizeof(picture_102008_png));
    else if(枪械id==102105||枪械id==1021051||枪械id==1021052||枪械id==1021053||枪械id==1021054||枪械id==1021055||枪械id==1021056||枪械id==1021057)
        return LoadWeaponTexture(枪械id, picture_102105_png, sizeof(picture_102105_png));
    else if(枪械id==103001||枪械id==1030011||枪械id==1030012||枪械id==1030013||枪械id==1030014||枪械id==1030015||枪械id==1030019||枪械id==103901)
        return LoadWeaponTexture(枪械id, picture_103001_png, sizeof(picture_103001_png));
    else if(枪械id==103002||枪械id==1030021||枪械id==1030022||枪械id==1030023||枪械id==1030024||枪械id==1030025||枪械id==1030026||枪械id==1030027||枪械id==103902||枪械id==1039021)
        return LoadWeaponTexture(枪械id, picture_103002_png, sizeof(picture_103002_png));
    else if(枪械id==103003||枪械id==1030031||枪械id==1030032||枪械id==1030033||枪械id==1030034||枪械id==1030035||枪械id==1030036||枪械id==1030037||枪械id==1030039||枪械id==103903)
        return LoadWeaponTexture(枪械id, picture_103003_png, sizeof(picture_103003_png));
    else if(枪械id==103004||枪械id==1030041||枪械id==1030042||枪械id==1030043||枪械id==1030044||枪械id==1030045||枪械id==1030046||枪械id==1030047)
        return LoadWeaponTexture(枪械id, picture_103004_png, sizeof(picture_103004_png));
    else if(枪械id==103005||枪械id==1030051||枪械id==1030052||枪械id==1030053||枪械id==1030054||枪械id==1030055||枪械id==1030059)
        return LoadWeaponTexture(枪械id, picture_103005_png, sizeof(picture_103005_png));
    else if(枪械id==103006||枪械id==1030061||枪械id==1030062||枪械id==1030063||枪械id==1030064||枪械id==1030065||枪械id==1030069)
        return LoadWeaponTexture(枪械id, picture_103006_png, sizeof(picture_103006_png));
    else if(枪械id==103007||枪械id==1030071||枪械id==1030072||枪械id==1030073||枪械id==1030074||枪械id==1030075||枪械id==1030076||枪械id==1030077)
        return LoadWeaponTexture(枪械id, picture_103007_png, sizeof(picture_103007_png));
    else if(枪械id==103008||枪械id==1030081||枪械id==1030082||枪械id==1030083||枪械id==1030084||枪械id==1030085)
        return LoadWeaponTexture(枪械id, picture_103008_png, sizeof(picture_103008_png));
    else if(枪械id==103009||枪械id==1030091||枪械id==1030092||枪械id==1030093||枪械id==1030094||枪械id==1030095||枪械id==1030096||枪械id==1030097||枪械id==1030099)
        return LoadWeaponTexture(枪械id, picture_103009_png, sizeof(picture_103009_png));
    else if(枪械id==103010||枪械id==1030101||枪械id==1030102||枪械id==1030103||枪械id==1030104||枪械id==1030105)
        return LoadWeaponTexture(枪械id, picture_103010_png, sizeof(picture_103010_png));
    else if(枪械id==103011)
        return LoadWeaponTexture(枪械id, picture_103011_png, sizeof(picture_103011_png));
    else if(枪械id==103012||枪械id==1030121||枪械id==1030122||枪械id==1030123||枪械id==1030124||枪械id==1030125||枪械id==1030126||枪械id==1030127)
        return LoadWeaponTexture(枪械id, picture_103012_png, sizeof(picture_103012_png));
    else if(枪械id==103100||枪械id==1031001||枪械id==1031002||枪械id==1031003||枪械id==1031004||枪械id==1031005||枪械id==1031006||枪械id==1031007)
        return LoadWeaponTexture(枪械id, picture_103100_png, sizeof(picture_103100_png));
    else if(枪械id==103901)
        return LoadWeaponTexture(枪械id, picture_103901_png, sizeof(picture_103901_png));
    else if(枪械id==103902||枪械id==1039021)
        return LoadWeaponTexture(枪械id, picture_103902_png, sizeof(picture_103902_png));
    else if(枪械id==103903)
        return LoadWeaponTexture(枪械id, picture_103903_png, sizeof(picture_103903_png));
    else if(枪械id==104001||枪械id==1040011||枪械id==1040012||枪械id==1040013||枪械id==1040014||枪械id==1040015)
        return LoadWeaponTexture(枪械id, picture_104001_png, sizeof(picture_104001_png));
    else if(枪械id==104002||枪械id==1040021||枪械id==1040022||枪械id==1040023||枪械id==1040024||枪械id==1040025)
        return LoadWeaponTexture(枪械id, picture_104002_png, sizeof(picture_104002_png));
    else if(枪械id==104003||枪械id==1040031||枪械id==1040032||枪械id==1040033||枪械id==1040034||枪械id==1040035||枪械id==1040036||枪械id==1040037||枪械id==1040039)
        return LoadWeaponTexture(枪械id, picture_104003_png, sizeof(picture_104003_png));
    else if(枪械id==104004||枪械id==1040041||枪械id==1040042||枪械id==1040043||枪械id==1040044||枪械id==1040045||枪械id==1040046||枪械id==1040047||枪械id==1040049)
        return LoadWeaponTexture(枪械id, picture_104004_png, sizeof(picture_104004_png));
    else if(枪械id==104100)
        return LoadWeaponTexture(枪械id, picture_104100_png, sizeof(picture_104100_png));
    else if(枪械id==105001||枪械id==1050011||枪械id==1050012||枪械id==1050013||枪械id==1050014||枪械id==1050015||枪械id==1050016||枪械id==1050017)
        return LoadWeaponTexture(枪械id, picture_105001_png, sizeof(picture_105001_png));
    else if(枪械id==105002||枪械id==1050021||枪械id==1050022||枪械id==1050023||枪械id==1050024||枪械id==1050025||枪械id==1050029)
        return LoadWeaponTexture(枪械id, picture_105002_png, sizeof(picture_105002_png));
    else if(枪械id==105010||枪械id==1050101||枪械id==1050102||枪械id==1050103||枪械id==1050104||枪械id==1050105||枪械id==1050106||枪械id==1050107||枪械id==1050109)
        return LoadWeaponTexture(枪械id, picture_105010_png, sizeof(picture_105010_png));
    else if(枪械id==106001||枪械id==1060011||枪械id==1060012||枪械id==1060013||枪械id==1060014||枪械id==1060015||枪械id==1060019)
        return LoadWeaponTexture(枪械id, picture_106001_png, sizeof(picture_106001_png));
    else if(枪械id==106002||枪械id==1060021||枪械id==1060022||枪械id==1060023||枪械id==1060024||枪械id==1060025)
        return LoadWeaponTexture(枪械id, picture_106002_png, sizeof(picture_106002_png));
    else if(枪械id==106003||枪械id==1060031||枪械id==1060032||枪械id==1060033||枪械id==1060034||枪械id==1060035)
        return LoadWeaponTexture(枪械id, picture_106003_png, sizeof(picture_106003_png));
    else if(枪械id==106004||枪械id==1060041||枪械id==1060042||枪械id==1060043||枪械id==1060044||枪械id==1060045||枪械id==1060049)
        return LoadWeaponTexture(枪械id, picture_106004_png, sizeof(picture_106004_png));
    else if(枪械id==106005||枪械id==1060051||枪械id==1060052||枪械id==1060053||枪械id==1060054||枪械id==1060055)
        return LoadWeaponTexture(枪械id, picture_106005_png, sizeof(picture_106005_png));
    else if(枪械id==106006||枪械id==1060061||枪械id==1060062||枪械id==1060063||枪械id==1060064||枪械id==1060065||枪械id==1060069)
        return LoadWeaponTexture(枪械id, picture_106006_png, sizeof(picture_106006_png));
    else if(枪械id==106007||枪械id==1060071)
        return LoadWeaponTexture(枪械id, picture_106007_png, sizeof(picture_106007_png));
    else if(枪械id==106008||枪械id==1060081||枪械id==1060082||枪械id==1060083||枪械id==1060084||枪械id==1060085||枪械id==1060089)
        return LoadWeaponTexture(枪械id, picture_106008_png, sizeof(picture_106008_png));
    else if(枪械id==106010||枪械id==1060101||枪械id==1060102||枪械id==1060103||枪械id==1060104||枪械id==1060105||枪械id==1060109)
        return LoadWeaponTexture(枪械id, picture_106010_png, sizeof(picture_106010_png));
    else if(枪械id==106011||枪械id==1060111||枪械id==1060112||枪械id==1060113||枪械id==1060114||枪械id==1060115)
        return LoadWeaponTexture(枪械id, picture_106011_png, sizeof(picture_106011_png));
    else if(枪械id==107001||枪械id==1070011||枪械id==1070012||枪械id==1070013||枪械id==1070014||枪械id==1070015)
        return LoadWeaponTexture(枪械id, picture_107001_png, sizeof(picture_107001_png));
    else if(枪械id==107006)
        return LoadWeaponTexture(枪械id, picture_107006_png, sizeof(picture_107006_png));
    else if(枪械id==107007)
        return LoadWeaponTexture(枪械id, picture_107007_png, sizeof(picture_107007_png));
    else if(枪械id==107008)
        return LoadWeaponTexture(枪械id, picture_107008_png, sizeof(picture_107008_png));
    else if(枪械id==108001||枪械id==1080011)
        return LoadWeaponTexture(枪械id, picture_108001_png, sizeof(picture_108001_png));
    else if(枪械id==108002)
        return LoadWeaponTexture(枪械id, picture_108002_png, sizeof(picture_108002_png));
    else if(枪械id==108003)
        return LoadWeaponTexture(枪械id, picture_108003_png, sizeof(picture_108003_png));
    else if(枪械id==108004||枪械id==1080041)
        return LoadWeaponTexture(枪械id, picture_108004_png, sizeof(picture_108004_png));
    else if(枪械id==602001)
        return LoadWeaponTexture(枪械id, picture_602001_png, sizeof(picture_602001_png));
    else if(枪械id==602002)
        return LoadWeaponTexture(枪械id, picture_602002_png, sizeof(picture_602002_png));
    else if(枪械id==602003)
        return LoadWeaponTexture(枪械id, picture_602003_png, sizeof(picture_602003_png));
    else if(枪械id==602004)
        return LoadWeaponTexture(枪械id, picture_602004_png, sizeof(picture_602004_png));
    else
        return LoadWeaponTexture(枪械id, picture_0_png, sizeof(picture_0_png));
}

struct 投掷物结构{
    bool b投掷;
    std::string 名字;
};

投掷物结构 识别投掷(int 枪械id) {
    投掷物结构 投掷物返回;
    if(枪械id==602045||枪械id==602123||枪械id==602001||枪械id==602002||
       枪械id==602003||枪械id==602004||枪械id==307006)
        投掷物返回.b投掷=true;
    else
        投掷物返回.b投掷=false;
    if(枪械id==602045||枪械id==602123)
        投掷物返回.名字="Sticky Bomb";
    else if(枪械id==602001)
        投掷物返回.名字="Stun Grenade";
    else if(枪械id==602002)
        投掷物返回.名字="Smoke Grenade";
    else if(枪械id==602003)
        投掷物返回.名字="Molotov Cocktail";
    else if(枪械id==602004)
        投掷物返回.名字="Frag Grenade";
    return 投掷物返回;
}

GLuint 获取投掷物纹理(int 投掷物ID) {
    switch(投掷物ID) {
        case 602004:
            return LoadWeaponTexture(投掷物ID, missile1_data, sizeof(missile1_data));
        case 602003:
            return LoadWeaponTexture(投掷物ID, missile2_data, sizeof(missile2_data));
        case 602002:
            return LoadWeaponTexture(投掷物ID, missile3_data, sizeof(missile3_data));
        case 602001:
            return LoadWeaponTexture(投掷物ID, missile4_data, sizeof(missile4_data));
        case 602045:
        case 602123:
            return LoadWeaponTexture(投掷物ID, missile5_data, sizeof(missile5_data));
        default:
            return 0;
    }
}

struct 六维{
    float 相机深度,初始x,二维y,初始y,二维x,高度;
};

六维 矩阵转换(三维 坐标,float 矩阵数组[16])
{
    六维 返回六维;
    float 相机深度=矩阵数组[3]*坐标.x+矩阵数组[7]*坐标.y+矩阵数组[11]*坐标.z+矩阵数组[15];
    float 初始x=(屏幕x/2)+(矩阵数组[0]*坐标.x+矩阵数组[4]*坐标.y+矩阵数组[8]*坐标.z+矩阵数组[12])/相机深度*(屏幕x/2);
    float 二维y=(屏幕y/2)-(矩阵数组[1]*坐标.x+矩阵数组[5]*坐标.y+矩阵数组[9]*坐标.z+矩阵数组[13])/相机深度*(屏幕y/2);
    float 初始y=(屏幕y/2)-(矩阵数组[1]*坐标.x+矩阵数组[5]*坐标.y+矩阵数组[9]*(坐标.z+165)+矩阵数组[13])/相机深度*(屏幕y/2);
    float 二维x=初始x-(二维y-初始y)/4;
    float 高度=(二维y-初始y)/2;
    返回六维.相机深度=相机深度;
    返回六维.二维y=二维y;
    返回六维.二维x=二维x;
    返回六维.初始x=初始x;
    返回六维.初始y=初始y;
    返回六维.高度=高度;
    return 返回六维;
}

struct 骨骼结构{
    ImVec2 二维骨骼[15];
};

inline 二维 获取二维骨骼(三维 测试坐标,long int Human,long int Bone,int idx)
{
    FTransform meshtrans;
    Driver.Read(Human,reinterpret_cast<void*>(&meshtrans),48);
    meshtrans.Translation=测试坐标;
    FMatrix c2wMatrix=TransformToMatrix(meshtrans);
    二维 返回骨骼位置;
    FTransform temp_trans;
    FMatrix boneMatrix;
    Driver.Read(Bone+idx*48,reinterpret_cast<void*>(&temp_trans),48);
    boneMatrix=TransformToMatrix(temp_trans);
    Get_Bone(MarixToVector(MatrixMulti(boneMatrix,c2wMatrix)),定义.矩阵数组,{屏幕x,屏幕y},&返回骨骼位置);
    return 返回骨骼位置;
}

inline 二维 获取二维预判骨骼(三维 测试坐标,long int Human,long int Bone,三维 向量,三维 载具向量,float 预判,float 载具预判,float 距离,float 子弹速度,bool 上车,int idx)
{
    FTransform meshtrans;
    Driver.Read(Human,reinterpret_cast<void*>(&meshtrans),48);
    meshtrans.Translation=测试坐标;
    if(!上车){
        meshtrans.Translation.x+=向量.x*Cloud.预判*距离/子弹速度*200;
        meshtrans.Translation.y+=向量.y*Cloud.预判*距离/子弹速度*200;
        meshtrans.Translation.z+=向量.z*Cloud.预判*距离/子弹速度*200;
    }
    else{
        meshtrans.Translation.x+=载具向量.x*Cloud.载具预判*距离/子弹速度*200;
        meshtrans.Translation.y+=载具向量.y*Cloud.载具预判*距离/子弹速度*200;
        meshtrans.Translation.z+=载具向量.z*Cloud.载具预判*距离/子弹速度*200;
    }
    FMatrix c2wMatrix=TransformToMatrix(meshtrans);
    二维 返回骨骼位置;
    FTransform temp_trans;
    FMatrix boneMatrix;
    Driver.Read(Bone+idx*48,reinterpret_cast<void*>(&temp_trans),48);
    boneMatrix=TransformToMatrix(temp_trans);
    Get_Bone(MarixToVector(MatrixMulti(boneMatrix,c2wMatrix)),定义.矩阵数组,{屏幕x,屏幕y},&返回骨骼位置);
    return 返回骨骼位置;
}

inline 骨骼结构 获取二维骨骼(long int Human,long int Bone)
{
    FTransform meshtrans;
    Driver.Read(Human, reinterpret_cast<void*>(&meshtrans), 4 * 11);
    FMatrix c2wMatrix = TransformToMatrix(meshtrans);
    骨骼结构 返回骨骼位置;
    FTransform temp_trans;
    FMatrix boneMatrix;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[0], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[0].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    绘制信息.dbptr.bone[0].Pos.z += 7;
    Get_Bone(绘制信息.dbptr.bone[0].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[0].ScreenPos);
    返回骨骼位置.二维骨骼[0].x = 绘制信息.dbptr.bone[0].ScreenPos.x;
    返回骨骼位置.二维骨骼[0].y = 绘制信息.dbptr.bone[0].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[1], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[1].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[1].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[1].ScreenPos);
    返回骨骼位置.二维骨骼[1].x = 绘制信息.dbptr.bone[1].ScreenPos.x;
    返回骨骼位置.二维骨骼[1].y = 绘制信息.dbptr.bone[1].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[2], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[2].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[2].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[2].ScreenPos);
    返回骨骼位置.二维骨骼[2].x = 绘制信息.dbptr.bone[2].ScreenPos.x;
    返回骨骼位置.二维骨骼[2].y = 绘制信息.dbptr.bone[2].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[3], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[3].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[3].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[3].ScreenPos);
    返回骨骼位置.二维骨骼[3].x = 绘制信息.dbptr.bone[3].ScreenPos.x;
    返回骨骼位置.二维骨骼[3].y = 绘制信息.dbptr.bone[3].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[4], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[4].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[4].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[4].ScreenPos);
    返回骨骼位置.二维骨骼[4].x = 绘制信息.dbptr.bone[4].ScreenPos.x;
    返回骨骼位置.二维骨骼[4].y = 绘制信息.dbptr.bone[4].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[5], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[5].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[5].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[5].ScreenPos);
    返回骨骼位置.二维骨骼[5].x = 绘制信息.dbptr.bone[5].ScreenPos.x;
    返回骨骼位置.二维骨骼[5].y = 绘制信息.dbptr.bone[5].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[6], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[6].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[6].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[6].ScreenPos);
    返回骨骼位置.二维骨骼[6].x = 绘制信息.dbptr.bone[6].ScreenPos.x;
    返回骨骼位置.二维骨骼[6].y = 绘制信息.dbptr.bone[6].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[7], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[7].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[7].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[7].ScreenPos);
    返回骨骼位置.二维骨骼[7].x = 绘制信息.dbptr.bone[7].ScreenPos.x;
    返回骨骼位置.二维骨骼[7].y = 绘制信息.dbptr.bone[7].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[8], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[8].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[8].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[8].ScreenPos);
    返回骨骼位置.二维骨骼[8].x = 绘制信息.dbptr.bone[8].ScreenPos.x;
    返回骨骼位置.二维骨骼[8].y = 绘制信息.dbptr.bone[8].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[9], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[9].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[9].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[9].ScreenPos);
    返回骨骼位置.二维骨骼[9].x = 绘制信息.dbptr.bone[9].ScreenPos.x;
    返回骨骼位置.二维骨骼[9].y = 绘制信息.dbptr.bone[9].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[10], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[10].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[10].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[10].ScreenPos);
    返回骨骼位置.二维骨骼[10].x = 绘制信息.dbptr.bone[10].ScreenPos.x;
    返回骨骼位置.二维骨骼[10].y = 绘制信息.dbptr.bone[10].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[11], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[11].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[11].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[11].ScreenPos);
    返回骨骼位置.二维骨骼[11].x = 绘制信息.dbptr.bone[11].ScreenPos.x;
    返回骨骼位置.二维骨骼[11].y = 绘制信息.dbptr.bone[11].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[12], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[12].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[12].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[12].ScreenPos);
    返回骨骼位置.二维骨骼[12].x = 绘制信息.dbptr.bone[12].ScreenPos.x;
    返回骨骼位置.二维骨骼[12].y = 绘制信息.dbptr.bone[12].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[13], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[13].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[13].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[13].ScreenPos);
    返回骨骼位置.二维骨骼[13].x = 绘制信息.dbptr.bone[13].ScreenPos.x;
    返回骨骼位置.二维骨骼[13].y = 绘制信息.dbptr.bone[13].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[14], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[14].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[14].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[14].ScreenPos);
    返回骨骼位置.二维骨骼[14].x = 绘制信息.dbptr.bone[14].ScreenPos.x;
    返回骨骼位置.二维骨骼[14].y = 绘制信息.dbptr.bone[14].ScreenPos.y;

    Driver.Read(Bone + 绘制信息.dbptr.boneaddr[15], reinterpret_cast<void*>(&temp_trans), 4 * 11);
    boneMatrix = TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[15].Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    Get_Bone(绘制信息.dbptr.bone[15].Pos, 定义.矩阵数组, {屏幕x, 屏幕y}, &绘制信息.dbptr.bone[15].ScreenPos);
    返回骨骼位置.二维骨骼[15].x = 绘制信息.dbptr.bone[15].ScreenPos.x;
    返回骨骼位置.二维骨骼[15].y = 绘制信息.dbptr.bone[15].ScreenPos.y;

    return 返回骨骼位置;
}

inline 骨骼结构 获取二维预判骨骼(long int Human,long int Bone,三维 向量,三维 载具向量,float 预判,float 载具预判,float 距离,float 子弹速度,bool 上车)
{
    FTransform meshtrans;
    Driver.Read(Human,reinterpret_cast<void*>(&meshtrans),4*11);
    FMatrix c2wMatrix=TransformToMatrix(meshtrans);
    骨骼结构 返回骨骼预判位置;
    FTransform temp_trans;
    FMatrix boneMatrix;

    Driver.Read(Bone+绘制信息.dbptr.boneaddr[0],reinterpret_cast<void*>(&temp_trans),4*11);
    boneMatrix=TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[0].Pos=MarixToVector(MatrixMulti(boneMatrix,c2wMatrix));
    if(!上车){
        绘制信息.dbptr.bone[0].Pos.x+=向量.x*Cloud.预判*距离/子弹速度*200;
        绘制信息.dbptr.bone[0].Pos.y+=向量.y*Cloud.预判*距离/子弹速度*200;
        绘制信息.dbptr.bone[0].Pos.z+=向量.z*Cloud.预判*距离/子弹速度*200;
    }
    else{
        绘制信息.dbptr.bone[0].Pos.x+=载具向量.x*Cloud.载具预判*距离/子弹速度*200;
        绘制信息.dbptr.bone[0].Pos.y+=载具向量.y*Cloud.载具预判*距离/子弹速度*200;
        绘制信息.dbptr.bone[0].Pos.z+=载具向量.z*Cloud.载具预判*距离/子弹速度*200;
    }
    绘制信息.dbptr.bone[0].Pos.z+=7;
    Get_Bone(绘制信息.dbptr.bone[0].Pos,定义.矩阵数组,{屏幕x,屏幕y},&绘制信息.dbptr.bone[0].ScreenPos);
    返回骨骼预判位置.二维骨骼[0].x=绘制信息.dbptr.bone[0].ScreenPos.x;
    返回骨骼预判位置.二维骨骼[0].y=绘制信息.dbptr.bone[0].ScreenPos.y;

    Driver.Read(Bone+绘制信息.dbptr.boneaddr[15],reinterpret_cast<void*>(&temp_trans),4*11);
    boneMatrix=TransformToMatrix(temp_trans);
    绘制信息.dbptr.bone[15].Pos=MarixToVector(MatrixMulti(boneMatrix,c2wMatrix));
    if(!上车){
        绘制信息.dbptr.bone[15].Pos.x+=向量.x*Cloud.预判*距离/子弹速度*200;
        绘制信息.dbptr.bone[15].Pos.y+=向量.y*Cloud.预判*距离/子弹速度*200;
        绘制信息.dbptr.bone[15].Pos.z+=向量.z*Cloud.预判*距离/子弹速度*200;
    }
    else{
        绘制信息.dbptr.bone[15].Pos.x+=载具向量.x*Cloud.载具预判*距离/子弹速度*200;
        绘制信息.dbptr.bone[15].Pos.y+=载具向量.y*Cloud.载具预判*距离/子弹速度*200;
        绘制信息.dbptr.bone[15].Pos.z+=载具向量.z*Cloud.载具预判*距离/子弹速度*200;
    }
    绘制信息.dbptr.bone[15].Pos.z+=7;
    Get_Bone(绘制信息.dbptr.bone[15].Pos,定义.矩阵数组,{屏幕x,屏幕y},&绘制信息.dbptr.bone[15].ScreenPos);
    返回骨骼预判位置.二维骨骼[15].x=绘制信息.dbptr.bone[15].ScreenPos.x;
    返回骨骼预判位置.二维骨骼[15].y=绘制信息.dbptr.bone[15].ScreenPos.y;

    return 返回骨骼预判位置;
}

struct 三维方框结构{
    ImVec2 屏幕顶点[8];
    三维 顶点[8];
};

inline 三维方框结构 三维方框函数(三维 坐标,float 长,float 宽,float 上高,float 下高,float 敌人视角){
    三维方框结构 三维方框返回;
    三维方框返回.顶点[0]={坐标.x+(-宽*cos(敌人视角/180*M_PI)-(-长)*sin(敌人视角/180*M_PI)),坐标.y+(-宽*sin(敌人视角/180*M_PI)+(-长)*cos(敌人视角/180*M_PI)),坐标.z-下高};
    三维方框返回.顶点[1]={坐标.x+(宽*cos(敌人视角/180*M_PI)-(-长)*sin(敌人视角/180*M_PI)),坐标.y+(宽*sin(敌人视角/180*M_PI)+(-长)*cos(敌人视角/180*M_PI)),坐标.z-下高};
    三维方框返回.顶点[2]={坐标.x+(宽*cos(敌人视角/180*M_PI)-长*sin(敌人视角/180*M_PI)),坐标.y+(宽*sin(敌人视角/180*M_PI)+长*cos(敌人视角/180*M_PI)),坐标.z-下高};
    三维方框返回.顶点[3]={坐标.x+(-宽*cos(敌人视角/180*M_PI)-长*sin(敌人视角/180*M_PI)),坐标.y+(-宽*sin(敌人视角/180*M_PI)+长*cos(敌人视角/180*M_PI)),坐标.z-下高};
    三维方框返回.顶点[4]={坐标.x+(-宽*cos(敌人视角/180*M_PI)-(-长)*sin(敌人视角/180*M_PI)),坐标.y+(-宽*sin(敌人视角/180*M_PI)+(-长)*cos(敌人视角/180*M_PI)),坐标.z+上高};
    三维方框返回.顶点[5]={坐标.x+(宽*cos(敌人视角/180*M_PI)-(-长)*sin(敌人视角/180*M_PI)),坐标.y+(宽*sin(敌人视角/180*M_PI)+(-长)*cos(敌人视角/180*M_PI)),坐标.z+上高};
    三维方框返回.顶点[6]={坐标.x+(宽*cos(敌人视角/180*M_PI)-长*sin(敌人视角/180*M_PI)),坐标.y+(宽*sin(敌人视角/180*M_PI)+长*cos(敌人视角/180*M_PI)),坐标.z+上高};
    三维方框返回.顶点[7]={坐标.x+(-宽*cos(敌人视角/180*M_PI)-长*sin(敌人视角/180*M_PI)),坐标.y+(-宽*sin(敌人视角/180*M_PI)+长*cos(敌人视角/180*M_PI)),坐标.z+上高};

    三维方框返回.屏幕顶点[0].x=矩阵转换(三维方框返回.顶点[0],定义.矩阵数组).二维x+矩阵转换(三维方框返回.顶点[0],定义.矩阵数组).高度*0.5;
    三维方框返回.屏幕顶点[0].y=矩阵转换(三维方框返回.顶点[0],定义.矩阵数组).二维y-矩阵转换(三维方框返回.顶点[0],定义.矩阵数组).高度*1;
    三维方框返回.屏幕顶点[1].x=矩阵转换(三维方框返回.顶点[1],定义.矩阵数组).二维x+矩阵转换(三维方框返回.顶点[1],定义.矩阵数组).高度*0.5;
    三维方框返回.屏幕顶点[1].y=矩阵转换(三维方框返回.顶点[1],定义.矩阵数组).二维y-矩阵转换(三维方框返回.顶点[1],定义.矩阵数组).高度*1;
    三维方框返回.屏幕顶点[2].x=矩阵转换(三维方框返回.顶点[2],定义.矩阵数组).二维x+矩阵转换(三维方框返回.顶点[2],定义.矩阵数组).高度*0.5;
    三维方框返回.屏幕顶点[2].y=矩阵转换(三维方框返回.顶点[2],定义.矩阵数组).二维y-矩阵转换(三维方框返回.顶点[2],定义.矩阵数组).高度*1;
    三维方框返回.屏幕顶点[3].x=矩阵转换(三维方框返回.顶点[3],定义.矩阵数组).二维x+矩阵转换(三维方框返回.顶点[3],定义.矩阵数组).高度*0.5;
    三维方框返回.屏幕顶点[3].y=矩阵转换(三维方框返回.顶点[3],定义.矩阵数组).二维y-矩阵转换(三维方框返回.顶点[3],定义.矩阵数组).高度*1;
    三维方框返回.屏幕顶点[4].x=矩阵转换(三维方框返回.顶点[4],定义.矩阵数组).二维x+矩阵转换(三维方框返回.顶点[4],定义.矩阵数组).高度*0.5;
    三维方框返回.屏幕顶点[4].y=矩阵转换(三维方框返回.顶点[4],定义.矩阵数组).二维y-矩阵转换(三维方框返回.顶点[4],定义.矩阵数组).高度*1;
    三维方框返回.屏幕顶点[5].x=矩阵转换(三维方框返回.顶点[5],定义.矩阵数组).二维x+矩阵转换(三维方框返回.顶点[5],定义.矩阵数组).高度*0.5;
    三维方框返回.屏幕顶点[5].y=矩阵转换(三维方框返回.顶点[5],定义.矩阵数组).二维y-矩阵转换(三维方框返回.顶点[5],定义.矩阵数组).高度*1;
    三维方框返回.屏幕顶点[6].x=矩阵转换(三维方框返回.顶点[6],定义.矩阵数组).二维x+矩阵转换(三维方框返回.顶点[6],定义.矩阵数组).高度*0.5;
    三维方框返回.屏幕顶点[6].y=矩阵转换(三维方框返回.顶点[6],定义.矩阵数组).二维y-矩阵转换(三维方框返回.顶点[6],定义.矩阵数组).高度*1;
    三维方框返回.屏幕顶点[7].x=矩阵转换(三维方框返回.顶点[7],定义.矩阵数组).二维x+矩阵转换(三维方框返回.顶点[7],定义.矩阵数组).高度*0.5;
    三维方框返回.屏幕顶点[7].y=矩阵转换(三维方框返回.顶点[7],定义.矩阵数组).二维y-矩阵转换(三维方框返回.顶点[7],定义.矩阵数组).高度*1;

    return 三维方框返回;
}

std::string GetAction(int ActionID)
{
    std::string ActionName;

    if(ActionID==163840) {
        ActionName="Self-Rescue";
    } else if(ActionID>=260&&ActionID<=290||ActionID>=328&&ActionID<=329) {
        ActionName="Reload";
    } else if(ActionID>=120&&ActionID<=170||ActionID>=630&&ActionID<=680||ActionID>=200&&ActionID<=203) {
        ActionName="Shoot";
    } else if(ActionID==32784) {
        ActionName="Rescue";
    } else if(ActionID==8388616||ActionID==8388617) {
        ActionName="Parachute";
    } else if(ActionID<=16777226&&ActionID>=16777224) {
        ActionName="Climb";
    } else if(ActionID>=500&&ActionID<=550||ActionID>=1500&&ActionID<=1550) {
        ActionName="ADS";
    } else if(ActionID>=1150&&ActionID<=1170||ActionID>=1670&&ActionID<=1700) {
        ActionName="Shoot";
    } else if(ActionID>=1020&&ActionID<=1060||ActionID>=1530&&ActionID<=1560) {
        ActionName="Peek";
    } else if(ActionID>=2120&&ActionID<=2123||ActionID>=2040&&ActionID<=2090||ActionID>=3080&&ActionID<=3090) {
        ActionName="Switch Weapon";
    } else if(ActionID>=8200&&ActionID<=8270) {
        ActionName="Punch";
    } else if(ActionID>=65540&&ActionID<=65580||ActionID==1114120||ActionID==589832) {
        ActionName="Use Heal Item";
    } else if(ActionID>=16390&&ActionID<=16420||ActionID==16456||ActionID==16457||ActionID>=17410&&ActionID<=17430||ActionID==3162120) {
        ActionName="Throw Item";
    } else if(ActionID>=3140000&&ActionID<=3170000) {
        ActionName="Peek";
    } else if(ActionID>=3146240&&ActionID<=3146250) {
        ActionName="ADS";
    } else if(ActionID>=4194300&&ActionID<=4504310) {
        ActionName="Swim";
    } else if(ActionID==524288) {
        ActionName="Drive Boat";
    } else if(ActionID==524296) {
        ActionName="Drive Vehicle";
    } else if(ActionID==589832||ActionID==1114120||ActionID==3211272) {
        ActionName="Refuel Vehicle";
    } else if(ActionID==1048584||ActionID==1048840||ActionID>=3145000&&ActionID<=3150000) {
        ActionName="Ride Vehicle";
    } else if(ActionID==1050632) {
        ActionName="Holster Weapon";
    } else if(ActionID==1) {
        ActionName="Move";
    } else if(ActionID==8||ActionID==1073741832) {
        ActionName="Stand";
    } else if(ActionID==9) {
        ActionName="Walk";
    } else if(ActionID==11||ActionID==10) {
        ActionName="Run";
    } else if(ActionID==16||ActionID==1073741824) {
        ActionName="Crouch";
    } else if(ActionID>=17&&ActionID<=19) {
        ActionName="Crouch Walk";
    } else if(ActionID>=33&&ActionID<=35) {
        ActionName="Prone Crawl";
    } else if(ActionID==32) {
        ActionName="Prone";
    } else if(ActionID>=60&&ActionID<=100) {
        ActionName="Jump";
    } else if(ActionID==131072||ActionID==1179648||ActionID==131073) {
        ActionName="Knocked Down";
    } else if(ActionID==262144) {
        ActionName="Eliminated";
    } else if(ActionID==1048576||ActionID==1050624) {
        ActionName="Ride Boat";
    } else if(ActionID>=16000000&&ActionID<=17000000) {
        ActionName="Climb";
    } else if(ActionID==268435464) {
        ActionName="Emote";
    } else if(ActionID>=-2150000000&&ActionID<=-2147000000||ActionID==-1073741816) {
        ActionName="Take Damage";
    } else {
        ActionName="Unrecorded";
    }
    return ActionName;
}
    
struct ItemStruct{
    bool bItem;
    std::string ItemName;
    ImColor ItemColor;
};

ItemStruct ITEMS(int ItemID) {
    ItemStruct ItemReturn;
    bool anyItemEnabled = Cloud.M416 || Cloud.S12K || Cloud.M24 || Cloud.AKM || Cloud.Bison || Cloud.UMP || Cloud.Flare || Cloud.SelfAED ||  Cloud.ScopeX3 || Cloud.ScopeX4 || Cloud.ScopeX6 || Cloud.ScopeX8 ||  Cloud.PoliceVest1 || Cloud.PoliceVest2 || Cloud.MilitaryVest3 || Cloud.MotorcycleHelmet1 || Cloud.MilitaryHelmet2 || Cloud.SpetsnazHelmet3 ||  Cloud.Backpack1 || Cloud.Backpack2 || Cloud.Backpack3 || Cloud.EnergyDrink || Cloud.AdrenalineSyringe || Cloud.Painkillers || Cloud.Bandage || Cloud.FirstAidKit || Cloud.MedKit || Cloud.FragGrenade || Cloud.SmokeGrenade || Cloud.StunGrenade || Cloud.MolotovCocktail || Cloud.StickyBomb || Cloud.MM9 || Cloud.MM762 || Cloud.MM556 || Cloud.ACP45 || Cloud.Gauge12 || Cloud.Magnum300 || Cloud.Bolt ||Cloud.M16A4 || Cloud.SCARL || Cloud.AUG || Cloud.QBZ || Cloud.G36C || Cloud.M762 || Cloud.MK47 || Cloud.ACE32 ||Cloud.Vector || Cloud.UZI || Cloud.TommyGun || Cloud.MP5K || Cloud.P90 || Cloud.JS9 || Cloud.S1897 || Cloud.S686 || Cloud.DBS || Cloud.NS2000 || Cloud.Kar98K || Cloud.Win94 || Cloud.Mosin || Cloud.Mini14 || Cloud.SKS || Cloud.SLR || Cloud.QBU || Cloud.Mk12 || Cloud.VSS || Cloud.DP28 || Cloud.M249 || Cloud.Pan || Cloud.ExtendedMagAR || Cloud.QuickDrawMagAR || Cloud.ExtendedQuickDrawMagAR;
    if(Cloud.Items && !anyItemEnabled) {
        ItemReturn.bItem = false;
        return ItemReturn;
    }
    bool itemMatched = false;
    if(ItemID==101004) {
        if(!Cloud.M416) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "M416";
        ItemReturn.ItemColor = ImColor(209, 238, 238, 255);
    }
    else if(ItemID==101002) {
        if(!Cloud.M16A4) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "M16A4";
        ItemReturn.ItemColor = ImColor(200, 200, 200, 255);
    }
    else if(ItemID==101003) {
        if(!Cloud.SCARL) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "SCAR-L";
        ItemReturn.ItemColor = ImColor(150, 200, 150, 255);
    }
    else if(ItemID==101001) {
        if(!Cloud.AKM) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "AKM";
        ItemReturn.ItemColor = ImColor(252, 200, 157, 255);
    }
    else if(ItemID==101006) {
        if(!Cloud.AUG) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "AUG";
        ItemReturn.ItemColor = ImColor(100, 200, 200, 255);
    }
    else if(ItemID==101007) {
        if(!Cloud.QBZ) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "QBZ";
        ItemReturn.ItemColor = ImColor(200, 200, 100, 255);
    }
    else if(ItemID==101010) {
        if(!Cloud.G36C) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "G36C";
        ItemReturn.ItemColor = ImColor(150, 150, 200, 255);
    }
    else if(ItemID==101008) {
        if(!Cloud.M762) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "M762";
        ItemReturn.ItemColor = ImColor(255, 150, 100, 255);
    }
    else if(ItemID==101009) {
        if(!Cloud.MK47) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "MK47";
        ItemReturn.ItemColor = ImColor(200, 150, 200, 255);
    }
    else if(ItemID==101102) {
        if(!Cloud.ACE32) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "ACE32";
        ItemReturn.ItemColor = ImColor(200, 180, 100, 255);
    }
    else if(ItemID==102002) {
        if(!Cloud.UMP) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "UMP45";
        ItemReturn.ItemColor = ImColor(248, 250, 185, 255);
    }
    else if(ItemID==102003) {
        if(!Cloud.Vector) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Vector";
        ItemReturn.ItemColor = ImColor(100, 200, 255, 255);
    }
    else if(ItemID==102001) {
        if(!Cloud.UZI) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "UZI";
        ItemReturn.ItemColor = ImColor(200, 200, 255, 255);
    }
    else if(ItemID==102004) {
        if(!Cloud.TommyGun) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Tommy Gun";
        ItemReturn.ItemColor = ImColor(200, 180, 150, 255);
    }
    else if(ItemID==102005) {
        if(!Cloud.Bison) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Bison";
        ItemReturn.ItemColor = ImColor(145, 154, 255, 255);
    }
    else if(ItemID==102008) {
        if(!Cloud.JS9) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "JS9";
        ItemReturn.ItemColor = ImColor(100, 200, 200, 255);
    }
    else if(ItemID==102007) {
        if(!Cloud.MP5K) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "MP5K";
        ItemReturn.ItemColor = ImColor(150, 200, 255, 255);
    }
    else if(ItemID==104003) {
        if(!Cloud.S12K) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "S12K";
        ItemReturn.ItemColor = ImColor(255, 102, 201, 255);
    }
    else if(ItemID==104002) {
        if(!Cloud.S1897) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "S1897";
        ItemReturn.ItemColor = ImColor(200, 150, 100, 255);
    }
    else if(ItemID==104001) {
        if(!Cloud.S686) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "S686";
        ItemReturn.ItemColor = ImColor(255, 200, 100, 255);
    }
    else if(ItemID==104004) {
        if(!Cloud.DBS) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "DBS";
        ItemReturn.ItemColor = ImColor(200, 200, 150, 255);
    }
    else if(ItemID==104102) {
        if(!Cloud.NS2000) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "NS2000";
        ItemReturn.ItemColor = ImColor(255, 150, 100, 255);
    }
    else if(ItemID==103001) {
        if(!Cloud.Kar98K) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Kar98K";
        ItemReturn.ItemColor = ImColor(200, 180, 100, 255);
    }
    else if(ItemID==103902) {
        if(!Cloud.M24) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "M24";
        ItemReturn.ItemColor = ImColor(252, 200, 157, 255);
    }
    else if(ItemID==103008) {
        if(!Cloud.Win94) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Win94";
        ItemReturn.ItemColor = ImColor(200, 180, 150, 255);
    }
    else if(ItemID==103011) {
        if(!Cloud.Mosin) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Mosin";
        ItemReturn.ItemColor = ImColor(200, 180, 120, 255);
    }
    else if(ItemID==103006) {
        if(!Cloud.Mini14) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Mini14";
        ItemReturn.ItemColor = ImColor(100, 200, 150, 255);
    }
    else if(ItemID==103004) {
        if(!Cloud.SKS) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "SKS";
        ItemReturn.ItemColor = ImColor(200, 150, 100, 255);
    }
    else if(ItemID==103009) {
        if(!Cloud.SLR) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "SLR";
        ItemReturn.ItemColor = ImColor(255, 200, 100, 255);
    }
    else if(ItemID==103010) {
        if(!Cloud.QBU) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "QBU";
        ItemReturn.ItemColor = ImColor(150, 200, 100, 255);
    }
    else if(ItemID==103100) {
        if(!Cloud.Mk12) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Mk12";
        ItemReturn.ItemColor = ImColor(150, 200, 150, 255);
    }
    else if(ItemID==103005) {
        if(!Cloud.VSS) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "VSS";
        ItemReturn.ItemColor = ImColor(150, 150, 200, 255);
    }
    else if(ItemID==105002) {
        if(!Cloud.DP28) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "DP-28";
        ItemReturn.ItemColor = ImColor(200, 200, 100, 255);
    }
    else if(ItemID==105001) {
        if(!Cloud.M249) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "M249";
        ItemReturn.ItemColor = ImColor(200, 200, 150, 255);
    }
    else if(ItemID==108004) {
        if(!Cloud.Pan) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Pan";
        ItemReturn.ItemColor = ImColor(200, 200, 200, 255);
    }
    else if(ItemID==106007) {
        if(!Cloud.Flare) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Flare Gun";
        ItemReturn.ItemColor = ImColor(159, 189, 28, 255);
    }
    else if(ItemID==601007 || ItemID==601008) {
        if(!Cloud.ScopeX3) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Scope x3";
        ItemReturn.ItemColor = ImColor(100, 200, 255, 255);
    }
    else if(ItemID==601004 || ItemID==601005) {
        if(!Cloud.ScopeX4) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Scope x4";
        ItemReturn.ItemColor = ImColor(100, 200, 255, 255);
    }
    else if(ItemID==601001) {
        if(!Cloud.ScopeX6) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Scope x6";
        ItemReturn.ItemColor = ImColor(100, 200, 255, 255);
    }
    else if(ItemID==601002 || ItemID==601003) {
        if(!Cloud.ScopeX8) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Scope x8";
        ItemReturn.ItemColor = ImColor(100, 200, 255, 255);
    }
    else if(ItemID==501001) {
        if(!Cloud.Backpack1) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Backpack Lv.1";
        ItemReturn.ItemColor = ImColor(150, 200, 150, 255);
    }
    else if(ItemID==501002) {
        if(!Cloud.Backpack2) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Backpack Lv.2";
        ItemReturn.ItemColor = ImColor(200, 220, 150, 255);
    }
    else if(ItemID==501006) {
        if(!Cloud.Backpack3) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Backpack Lv.3";
        ItemReturn.ItemColor = ImColor(255, 242, 102, 255);
    }
    else if(ItemID==502001) {
        if(!Cloud.MotorcycleHelmet1) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Motorcycle Helmet Lv.1";
        ItemReturn.ItemColor = ImColor(150, 200, 255, 255);
    }
    else if(ItemID==502002) {
        if(!Cloud.MilitaryHelmet2) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Military Helmet Lv.2";
        ItemReturn.ItemColor = ImColor(180, 220, 255, 255);
    }
    else if(ItemID==502003) {
        if(!Cloud.SpetsnazHelmet3) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Spetsnaz Helmet Lv.3";
        ItemReturn.ItemColor = ImColor(255, 242, 102, 255);
    }
    else if(ItemID==503001) {
        if(!Cloud.PoliceVest1) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Police Vest Lv.1";
        ItemReturn.ItemColor = ImColor(200, 150, 150, 255);
    }
    else if(ItemID==503002) {
        if(!Cloud.PoliceVest2) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Police Vest Lv.2";
        ItemReturn.ItemColor = ImColor(220, 180, 180, 255);
    }
    else if(ItemID==503003) {
        if(!Cloud.MilitaryVest3) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Military Vest Lv.3";
        ItemReturn.ItemColor = ImColor(255, 242, 102, 255);
    }
    else if(ItemID==601001) {
        if(!Cloud.EnergyDrink) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Energy Drink";
        ItemReturn.ItemColor = ImColor(255, 200, 50, 255);
    }
    else if(ItemID==601002) {
        if(!Cloud.AdrenalineSyringe) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Adrenaline Syringe";
        ItemReturn.ItemColor = ImColor(255, 50, 50, 255);
    }
    else if(ItemID==601003) {
        if(!Cloud.Painkillers) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Painkillers";
        ItemReturn.ItemColor = ImColor(255, 150, 50, 255);
    }
    else if(ItemID==601004) {
        if(!Cloud.Bandage) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Bandage";
        ItemReturn.ItemColor = ImColor(200, 200, 200, 255);
    }
    else if(ItemID==601005) {
        if(!Cloud.FirstAidKit) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "First Aid Kit";
        ItemReturn.ItemColor = ImColor(255, 100, 100, 255);
    }
    else if(ItemID==601006) {
        if(!Cloud.MedKit) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Med Kit";
        ItemReturn.ItemColor = ImColor(255, 50, 50, 255);
    }
    else if(ItemID==602602) {
        if(!Cloud.SelfAED) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Self AED";
        ItemReturn.ItemColor = ImColor(159, 189, 28, 255);
    }
    else if(ItemID==602004) {
        if(!Cloud.FragGrenade) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Frag Grenade";
        ItemReturn.ItemColor = ImColor(255, 80, 80, 255);
    }
    else if(ItemID==602002) {
        if(!Cloud.SmokeGrenade) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Smoke Grenade";
        ItemReturn.ItemColor = ImColor(180, 180, 180, 255);
    }
    else if(ItemID==602001) {
        if(!Cloud.StunGrenade) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Stun Grenade";
        ItemReturn.ItemColor = ImColor(200, 200, 50, 255);
    }
    else if(ItemID==602003) {
        if(!Cloud.MolotovCocktail) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Molotov Cocktail";
        ItemReturn.ItemColor = ImColor(255, 140, 0, 255);
    }
    else if(ItemID==602123) {
        if(!Cloud.StickyBomb) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Sticky Bomb";
        ItemReturn.ItemColor = ImColor(255, 0, 120, 255);
    }
    else if(ItemID==301001) {
        if(!Cloud.MM9) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "9mm";
        ItemReturn.ItemColor = ImColor(200, 200, 100, 255);
    }
    else if(ItemID==302001) {
        if(!Cloud.MM762) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "7.62mm";
        ItemReturn.ItemColor = ImColor(200, 100, 50, 255);
    }
    else if(ItemID==303001) {
        if(!Cloud.MM556) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "5.56mm";
        ItemReturn.ItemColor = ImColor(100, 200, 100, 255);
    }
    else if(ItemID==304001) {
        if(!Cloud.Gauge12) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "12 Gauge";
        ItemReturn.ItemColor = ImColor(200, 150, 100, 255);
    }
    else if(ItemID==305001) {
        if(!Cloud.ACP45) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "45 ACP";
        ItemReturn.ItemColor = ImColor(150, 150, 200, 255);
    }
    else if(ItemID==306001) {
        if(!Cloud.Magnum300) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "300 Magnum";
        ItemReturn.ItemColor = ImColor(100, 100, 200, 255);
    }
    else if(ItemID==307001) {
        if(!Cloud.Bolt) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Bolt";
        ItemReturn.ItemColor = ImColor(150, 150, 150, 255);
    }
    else if(ItemID==204011) {
        if(!Cloud.ExtendedMagAR) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Extended Mag";
        ItemReturn.ItemColor = ImColor(100, 200, 200, 255);
    }
    else if(ItemID==204012) {
        if(!Cloud.QuickDrawMagAR) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "QuickDraw Mag";
        ItemReturn.ItemColor = ImColor(200, 200, 100, 255);
    }
    else if(ItemID==204013) {
        if(!Cloud.ExtendedQuickDrawMagAR) { ItemReturn.bItem = false; return ItemReturn; }
        itemMatched = true;
        ItemReturn.ItemName = "Extended QuickDraw Mag";
        ItemReturn.ItemColor = ImColor(255, 200, 100, 255);
    }
    if(itemMatched) {
        ItemReturn.bItem = true;
    } else {
        ItemReturn.bItem = false;
    }
    return ItemReturn;
}

struct VehicleItem {
    bool bVehicle;
    std::string Name;
};

VehicleItem GetVehicleItem(int VehicleID) {
    VehicleItem VehicleReturn;
    
    if (!Cloud.Vehicle) {
        VehicleReturn.bVehicle = false;
        return VehicleReturn;
    }
    
    bool anyVehicleEnabled = Cloud.Buggy || Cloud.Dacia || Cloud.CoupeRB || Cloud.UAZOpen || Cloud.UAZClosed || Cloud.Mirado ||
                             Cloud.PickupOpen || Cloud.PickupClosed || Cloud.Van ||
                             Cloud.Rony || Cloud.Tukshai || Cloud.Scooter ||
                             Cloud.MonsterTruck || Cloud.MotorGlider || Cloud.Snowmobile ||
                             Cloud.Zima || Cloud.PG117 || Cloud.JetSki ||
                             Cloud.BRDM2 || Cloud.UTV;
    
    if (!anyVehicleEnabled) {
        VehicleReturn.bVehicle = false;
        return VehicleReturn;
    }
    
    switch (VehicleID) {
        case 60:
            if (!Cloud.MotorGlider) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Glider";
            break;
        case 24:
            if (!Cloud.BRDM2) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "BRDM-2";
            break;
        case 4:
            if (!Cloud.Van) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Bus";
            break;
        case 30:
            if (!Cloud.UAZClosed) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "SUV";
            break;
        case 53:
            if (!Cloud.MonsterTruck) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Monster Truck";
            break;
        case 8:
            if (!Cloud.UAZOpen) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Jeep";
            break;
        case 6:
            if (!Cloud.PickupOpen) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Pickup";
            break;
        case 16:
            if (!Cloud.PickupClosed) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Ronin Pickup";
            break;
        case 3:
            if (!Cloud.Dacia) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Dacia";
            break;
        case 61:
            if (!Cloud.CoupeRB) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Coupe RB";
            break;
        case 15:
            if (!Cloud.Mirado) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Mirado";
            break;
        case 7:
            if (!Cloud.Buggy) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Buggy";
            break;
        case 66:
            if (!Cloud.UTV) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "ATV";
            break;
        case 19:
            if (!Cloud.Tukshai) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Tuk-Tuk";
            break;
        case 2:
            if (!Cloud.Scooter) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Motorcycle";
            break;
        case 18:
            if (!Cloud.Snowmobile) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Snowmobile";
            break;
        case 20:
            if (!Cloud.Zima) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Zima";
            break;
        case 1:
            if (!Cloud.Scooter) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Scooter";
            break;
        case 17:
            if (!Cloud.Scooter) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Scooter";
            break;
        case 67:
            if (!Cloud.Scooter) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Bicycle";
            break;
        case 11:
            if (!Cloud.PG117) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Boat";
            break;
        case 12:
            if (!Cloud.JetSki) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Jet Ski";
            break;
        case 64:
            if (!Cloud.MotorGlider) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Helicopter";
            break;
        case 87:
            if (!Cloud.Scooter) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Horse";
            break;
        case 88:
            if (!Cloud.PG117) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Hovercraft";
            break;
        case 59:
            if (!Cloud.Rony) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Mine Cart";
            break;
        case 93:
            if (!Cloud.Van) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Vending Machine Bus";
            break;
        case 114:
            if (!Cloud.BRDM2) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "BRDM-2";
            break;
        case 1805:
            if (!Cloud.CoupeRB) { VehicleReturn.bVehicle = false; return VehicleReturn; }
            VehicleReturn.Name = "Roadster";
            break;
        default:
            VehicleReturn.bVehicle = false;
            return VehicleReturn;
    }
    
    VehicleReturn.bVehicle = true;
    return VehicleReturn;
}

float 动作压枪(int 状态){
    if(状态==0)return 1;
    else if(状态==1)return 0.95;
    else if(状态==2)return 0.5;
    else return 1;
}

void Cleanup(){
    Driver.Off_Drive();
    Initptr.shutdown();
    return;
}

bool WindowClick() {
    static ImVec2 位置;
    static bool 点击;
    if(ImGui::IsItemActive()) {
        if(!点击) {
            点击 = true;
            位置 = ImGui::GetWindowPos();
            return false;
        }
    }
    else if(点击) {
        点击 = false;
        if(位置.x==ImGui::GetWindowPos().x && ImGui::GetWindowPos().y==位置.y) {
            return true;
        }
    }
    return false;
}

bool CheckVariable() {
    // Clamp invalid values in-place instead of wiping entire config
    bool fixed = false;
    if (Cloud.部位 != 0 && Cloud.部位 != 1) { Cloud.部位 = 0; fixed = true; }
    if (Cloud.过滤 != 1 && Cloud.过滤 != 2) { Cloud.过滤 = 1; fixed = true; }
    if (Cloud.自瞄范围 < 0 || Cloud.自瞄范围 > 1000) { Cloud.自瞄范围 = 180.0f; fixed = true; }
    if (Cloud.腰射压枪 < 0 || Cloud.腰射压枪 > 5) { Cloud.腰射压枪 = 0.5f; fixed = true; }
    if (Cloud.开镜压枪 < 0 || Cloud.开镜压枪 > 5) { Cloud.开镜压枪 = 1.3f; fixed = true; }
    if (Cloud.腰射速度 < 0 || Cloud.腰射速度 > 5) { Cloud.腰射速度 = 1.0f; fixed = true; }
    if (Cloud.开镜速度 < 0 || Cloud.开镜速度 > 5) { Cloud.开镜速度 = 1.0f; fixed = true; }
    if (Cloud.腰射距离 < 0 || Cloud.腰射距离 > 500) { Cloud.腰射距离 = 50.0f; fixed = true; }
    if (Cloud.开镜距离 < 0 || Cloud.开镜距离 > 500) { Cloud.开镜距离 = 500.0f; fixed = true; }
    if (Cloud.预判 < 0 || Cloud.预判 > 5) { Cloud.预判 = 0.75f; fixed = true; }
    if (Cloud.载具预判 < 0 || Cloud.载具预判 > 5) { Cloud.载具预判 = 0.3f; fixed = true; }
    if (Cloud.三称广角大小 < 0 || Cloud.三称广角大小 > 150) { Cloud.三称广角大小 = 90.0f; fixed = true; }
    if (Cloud.一称广角大小 < 0 || Cloud.一称广角大小 > 150) { Cloud.一称广角大小 = 103.0f; fixed = true; }
    if (Cloud.开镜广角大小 < 0 || Cloud.开镜广角大小 > 25) { Cloud.开镜广角大小 = 5.0f; fixed = true; }
    if (Cloud.相机广角大小 < 0 || Cloud.相机广角大小 > 500) { Cloud.相机广角大小 = 220.0f; fixed = true; }
    if (Cloud.目标帧率 < 30 || Cloud.目标帧率 > 240) { Cloud.目标帧率 = 120.0f; fixed = true; }
    if (Cloud.TouchRate < 60 || Cloud.TouchRate > 5000) { Cloud.TouchRate = 1000.0f; fixed = true; }
    if (fixed) {
        printf("   [-] Some config values were invalid, clamped to defaults.\n");
    }
    return false;  // never wipe — always fix in-place
}

void Initialization() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local_time{};
    localtime_r(&now_c, &local_time);
    char timeString[64];
    std::strftime(timeString, sizeof(timeString), "%m-%d %H:%M", &local_time);
    ReadConfig("NovaXConfig.ini");
    CheckVariable();
    androidVersion = GetAndroidVersion();
    kernelVersion = GetKernelVersion();
    static bool Start = true;
    CreateResolutionThread();
    GetModuleBase();
    while (定义.pid == -1) {
        usleep(200);
        GetModuleBase();
        printf(
            "=============================\n"
            "   [+] Time: %s\n"
            "-----------------------------\n"
            "   [-] Please open game!\n"
            "=============================\n",
            timeString
        );
        printf("\033[2J\033[H");
    }
}

std::vector<int> getBigCores() {
    std::vector<std::pair<long, int>> cores;
    char path[256];
    for (int i = 0; ; i++) {
        snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq", i);
        FILE* fp = fopen(path, "r");
        if (!fp) break;
        long freq = 0;
        fscanf(fp, "%ld", &freq);
        fclose(fp);
        cores.emplace_back(-freq, i);
    }
    std::sort(cores.begin(), cores.end());
    std::vector<int> bigs;
    int count = cores.size() / 2;
    for (int i = 0; i < count; i++)
        bigs.push_back(cores[i].second);
    return bigs;
}

bool bindToCores(const std::vector<int>& cpus) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    for (int cpu : cpus)
        CPU_SET(cpu, &mask);
    return sched_setaffinity(gettid(), sizeof(mask), &mask) == 0;
}
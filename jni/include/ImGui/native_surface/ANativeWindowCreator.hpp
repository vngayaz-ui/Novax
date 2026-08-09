#ifndef A_NATIVE_WINDOW_CREATOR_H
#define A_NATIVE_WINDOW_CREATOR_H

#include <android/native_window.h>
#include <android/log.h>
#include <dlfcn.h>
#include <sys/system_properties.h>
#include <iostream>
#include <cstddef>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string_view>
#include <array>
#include <unordered_set>

#define ResolveMethod(ClassName,MethodName,Handle,MethodSignature)                                                                    \
ClassName##__##MethodName = reinterpret_cast<decltype(ClassName##__##MethodName)>(symbolMethod.Find(Handle,MethodSignature));       \
if(nullptr==ClassName##__##MethodName)                                                                                            \
{                                                                                                                                    \
__android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Method not found: %s -> %s::%s",MethodSignature,#ClassName,#MethodName); \
}

namespace android
{
namespace detail
{
namespace ui
{
// A LayerStack identifies a Z-ordered group of layers. A layer can only be associated to a single
// LayerStack,but a LayerStack can be associated to multiple displays,mirroring the same content.
struct LayerStack
{
uint32_t id = UINT32_MAX;
};

enum class Rotation
{
Rotation0 = 0,
Rotation90 = 1,
Rotation180 = 2,
Rotation270 = 3
};

// A simple value type representing a two-dimensional size.
struct Size
{
int32_t width = -1;
int32_t height = -1;
};

// Transactional state of physical or virtual display. Note that libgui defines
// android::DisplayState as a superset of android::ui::DisplayState.
struct DisplayState
{
LayerStack layerStack;
Rotation 方向 = Rotation::Rotation0;
Size layerStackSpaceRect;
};

typedef int64_t nsecs_t; // nano-seconds
struct DisplayInfo
{
uint32_t w{0};
uint32_t h{0};
float xdpi{0};
float ydpi{0};
float fps{0};
float density{0};
uint8_t 方向{0};
bool secure{false};
nsecs_t appVsyncOffset{0};
nsecs_t presentationDeadline{0};
uint32_t viewportW{0};
uint32_t viewportH{0};
};

enum class DisplayType
{
DisplayIdMain = 0,
DisplayIdHdmi = 1
};

struct PhysicalDisplayId
{
uint64_t value;
};
}

struct String8;

struct LayerMetadata;

struct Surface;

struct SurfaceControl;

struct SurfaceComposerClientTransaction;

struct SurfaceComposerClient;

template <typename any_t>
struct StrongPointer
{
union
{
any_t*pointer;
char padding[sizeof(std::max_align_t)];
};

inline any_t*operator->() const { return pointer; }
inline any_t*get() const { return pointer; }
inline explicit operator bool() const { return nullptr != pointer; }
};

struct Functionals
{
struct SymbolMethod
{
void*(*Open)(const char*filename,int flag) = nullptr;
void*(*Find)(void*handle,const char*symbol) = nullptr;
int (*Close)(void*handle) = nullptr;
};
size_t 安卓版本 = -1;
void (*RefBase__IncStrong)(void*thiz,void*id) = nullptr;
void (*RefBase__DecStrong)(void*thiz,void*id) = nullptr;
void (*String8__Constructor)(void*thiz,const char*const data) = nullptr;
void (*String8__Destructor)(void*thiz) = nullptr;
void (*LayerMetadata__Constructor)(void*thiz) = nullptr;
void (*LayerMetadata__setInt32)(void*thiz,uint32_t key,int32_t value) = nullptr;
void (*SurfaceComposerClient__Constructor)(void*thiz) = nullptr;
void (*SurfaceComposerClient__Destructor)(void*thiz) = nullptr;
StrongPointer<void> (*SurfaceComposerClient__CreateSurface)(void*thiz,void*name,uint32_t w,uint32_t h,int32_t format,uint32_t flags,void*parentHandle,void*layerMetadata,uint32_t*outTransformHint) = nullptr;
StrongPointer<void> (*SurfaceComposerClient__CreateSurface_and9)(void*thiz,void*name,uint32_t w,uint32_t h,int32_t format,uint32_t flags,void*parentHandle,int32_t windowType,int32_t ownerUid) = nullptr;
StrongPointer<void> (*SurfaceComposerClient__GetInternalDisplayToken)() = nullptr;
StrongPointer<void> (*SurfaceComposerClient__GetBuiltInDisplay)(ui::DisplayType type) = nullptr;
int32_t (*SurfaceComposerClient__GetDisplayState)(StrongPointer<void> &display,ui::DisplayState*displayState) = nullptr;
int32_t (*SurfaceComposerClient__GetDisplayInfo)(StrongPointer<void> &display,ui::DisplayInfo*displayInfo) = nullptr;
std::vector<ui::PhysicalDisplayId> (*SurfaceComposerClient__GetPhysicalDisplayIds)() = nullptr;
StrongPointer<void> (*SurfaceComposerClient__GetPhysicalDisplayToken)(ui::PhysicalDisplayId displayId) = nullptr;
void (*SurfaceComposerClient__Transaction__Constructor)(void*thiz) = nullptr;
void*(*SurfaceComposerClient__Transaction__SetLayer)(void*thiz,StrongPointer<void> &surfaceControl,int32_t z) = nullptr;
void*(*SurfaceComposerClient__Transaction__SetTrustedOverlay)(void*thiz,StrongPointer<void> &surfaceControl,bool isTrustedOverlay) = nullptr;
int32_t (*SurfaceComposerClient__Transaction__Apply)(void*thiz,bool synchronous,bool oneWay) = nullptr;
int32_t (*SurfaceControl__Validate)(void*thiz) = nullptr;
StrongPointer<Surface> (*SurfaceControl__GetSurface)(void*thiz) = nullptr;
void (*SurfaceControl__DisConnect)(void*thiz) = nullptr;
void* (*SurfaceComposerClient__Transaction__SetInputWindowInfo_v15)(void* thiz, StrongPointer<void>& surfaceControl, void* windowInfo) = nullptr;
void* (*SurfaceComposerClient__Transaction__SetInputWindowInfo_v16)(void* thiz, StrongPointer<void>& surfaceControl, void* windowInfoHandle) = nullptr;
void* (*SurfaceComposerClient__Transaction__SetInputWindowInfo)(void* thiz, StrongPointer<void>& surfaceControl, void* info) = nullptr;
void* (*SurfaceComposerClient__Transaction__SetFlags)(void* thiz, StrongPointer<void>& surfaceControl, uint32_t flags, uint32_t mask) = nullptr;

// 镜像相关 API
StrongPointer<void> (*SurfaceComposerClient__MirrorSurface)(void* thiz, void* mirrorFromSurface) = nullptr;
void* (*SurfaceComposerClient__Transaction__SetLayerStack)(void* thiz, StrongPointer<void>& surfaceControl, uint32_t layerStack) = nullptr;
void* (*SurfaceComposerClient__Transaction__Show)(void* thiz, StrongPointer<void>& surfaceControl) = nullptr;
void* (*SurfaceComposerClient__Transaction__Hide)(void* thiz, StrongPointer<void>& surfaceControl) = nullptr;
void* (*SurfaceComposerClient__Transaction__Reparent)(void* thiz, StrongPointer<void>& surfaceControl, StrongPointer<void>& newParentHandle) = nullptr;
void* (*SurfaceComposerClient__Transaction__SetMatrix)(void* thiz, StrongPointer<void>& surfaceControl, float dsdx, float dtdx, float dtdy, float dsdy) = nullptr;
void* (*SurfaceComposerClient__Transaction__SetPosition)(void* thiz, StrongPointer<void>& surfaceControl, float x, float y) = nullptr;

Functionals(const SymbolMethod &symbolMethod)
{
std::string 安卓版本字符串(128,0);
安卓版本字符串.resize(__system_property_get("ro.build.version.release",安卓版本字符串.data()));
安卓版本 = 安卓版本字符串.empty() ? 0 : std::stoi(安卓版本字符串);
if(安卓版本 < 9)
{
__android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Unsupported system version: %zu",安卓版本);
printf("[-]Android version<9\n");
return;
}
#ifdef __LP64__
auto Gui系统库 = symbolMethod.Open("/system/lib64/libgui.so",RTLD_LAZY);
auto Utils系统库 = symbolMethod.Open("/system/lib64/libutils.so",RTLD_LAZY);
#else
auto Gui系统库 = symbolMethod.Open("/system/lib/libgui.so",RTLD_LAZY);
auto Utils系统库 = symbolMethod.Open("/system/lib/libutils.so",RTLD_LAZY);
#endif
ResolveMethod(RefBase, IncStrong, Utils系统库, "_ZNK7android7RefBase9incStrongEPKv");
ResolveMethod(RefBase, DecStrong, Utils系统库, "_ZNK7android7RefBase9decStrongEPKv");
ResolveMethod(String8, Constructor, Utils系统库, "_ZN7android7String8C2EPKc");
ResolveMethod(String8, Destructor, Utils系统库, "_ZN7android7String8D2Ev");
ResolveMethod(LayerMetadata, Constructor, Gui系统库, "_ZN7android13LayerMetadataC2Ev");
ResolveMethod(LayerMetadata, setInt32, Gui系统库, "_ZN7android13LayerMetadata8setInt32Eji");
ResolveMethod(SurfaceComposerClient, Constructor, Gui系统库, "_ZN7android21SurfaceComposerClientC2Ev");
ResolveMethod(SurfaceComposerClient, CreateSurface, Gui系统库, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijRKNS_2spINS_7IBinderEEENS_13LayerMetadataEPj");
ResolveMethod(SurfaceComposerClient, GetInternalDisplayToken, Gui系统库, "_ZN7android21SurfaceComposerClient23getInternalDisplayTokenEv");
ResolveMethod(SurfaceComposerClient, GetDisplayState, Gui系统库, "_ZN7android21SurfaceComposerClient15getDisplayStateERKNS_2spINS_7IBinderEEEPNS_2ui12DisplayStateE");
ResolveMethod(SurfaceComposerClient, GetDisplayInfo, Gui系统库, "_ZN7android21SurfaceComposerClient14getDisplayInfoERKNS_2spINS_7IBinderEEEPNS_11DisplayInfoE");
ResolveMethod(SurfaceComposerClient, GetPhysicalDisplayIds, Gui系统库, "_ZN7android21SurfaceComposerClient21getPhysicalDisplayIdsEv");
ResolveMethod(SurfaceComposerClient, GetPhysicalDisplayToken, Gui系统库, "_ZN7android21SurfaceComposerClient23getPhysicalDisplayTokenENS_17PhysicalDisplayIdE");
ResolveMethod(SurfaceComposerClient__Transaction, Constructor, Gui系统库, "_ZN7android21SurfaceComposerClient11TransactionC2Ev");
ResolveMethod(SurfaceComposerClient__Transaction, SetLayer, Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction8setLayerERKNS_2spINS_14SurfaceControlEEEi");
ResolveMethod(SurfaceComposerClient__Transaction, SetTrustedOverlay, Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction17setTrustedOverlayERKNS_2spINS_14SurfaceControlEEEb");
ResolveMethod(SurfaceComposerClient__Transaction, Apply, Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction5applyEbb");
ResolveMethod(SurfaceControl, Validate, Gui系统库, "_ZNK7android14SurfaceControl8validateEv");
ResolveMethod(SurfaceControl, GetSurface, Gui系统库, "_ZN7android14SurfaceControl10getSurfaceEv");
ResolveMethod(SurfaceControl, DisConnect, Gui系统库, "_ZN7android14SurfaceControl10disconnectEv");
void* 函数指针 = nullptr;
if(安卓版本==-1)
{
printf("[+]Android version:");
std::cin>>安卓版本;
}
// 解析 setFlags（Android 12+ 支持动态修改 Surface 标志）
if (安卓版本 >= 12) {
    void* funcPtr = symbolMethod.Find(Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction8setFlagsERKNS_2spINS_14SurfaceControlEEEjj");
    *reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__SetFlags) = funcPtr;
    if (!funcPtr) __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Method not found: SurfaceComposerClient::Transaction::setFlags");
}

// 解析镜像相关 API（Android 11+）
if (安卓版本 >= 11) {
    void* mirrorFunc = symbolMethod.Find(Gui系统库, "_ZN7android21SurfaceComposerClient13mirrorSurfaceEPNS_14SurfaceControlE");
    *reinterpret_cast<void**>(&SurfaceComposerClient__MirrorSurface) = mirrorFunc;
    if (!mirrorFunc) __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Method not found: SurfaceComposerClient::mirrorSurface");
}

// 解析 Transaction 扩展方法（Android 9+）
if (安卓版本 >= 9) {
    void* setLayerStackFunc = symbolMethod.Find(Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction13setLayerStackERKNS_2spINS_14SurfaceControlEEENS_2ui10LayerStackE");
    *reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__SetLayerStack) = setLayerStackFunc;
    if (!setLayerStackFunc) __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Method not found: Transaction::setLayerStack");

    void* showFunc = symbolMethod.Find(Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction4showERKNS_2spINS_14SurfaceControlEEE");
    *reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__Show) = showFunc;
    if (!showFunc) __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Method not found: Transaction::show");

    void* hideFunc = symbolMethod.Find(Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction4hideERKNS_2spINS_14SurfaceControlEEE");
    *reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__Hide) = hideFunc;
    if (!hideFunc) __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Method not found: Transaction::hide");

    void* setMatrixFunc = symbolMethod.Find(Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction9setMatrixERKNS_2spINS_14SurfaceControlEEEffff");
    *reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__SetMatrix) = setMatrixFunc;
    if (!setMatrixFunc) __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Method not found: Transaction::setMatrix");

    void* setPositionFunc = symbolMethod.Find(Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction11setPositionERKNS_2spINS_14SurfaceControlEEEff");
    *reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__SetPosition) = setPositionFunc;
    if (!setPositionFunc) __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Method not found: Transaction::setPosition");
}

if (安卓版本 >= 12) {
    void* reparentFunc = symbolMethod.Find(Gui系统库, "_ZN7android21SurfaceComposerClient11Transaction8reparentERKNS_2spINS_14SurfaceControlEEES6_");
    *reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__Reparent) = reparentFunc;
    if (!reparentFunc) __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Method not found: Transaction::reparent");
}

if(安卓版本>=16)
{
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android3gui13LayerMetadataC2Ev");
*reinterpret_cast<void**>(&LayerMetadata__Constructor) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android3gui13LayerMetadataC2Ev");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android3gui13LayerMetadata8setInt32Eji");
*reinterpret_cast<void**>(&LayerMetadata__setInt32) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android3gui13LayerMetadata8setInt32Eji");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient11Transaction18setInputWindowInfoERKNS_2spINS_14SurfaceControlEEENS2_INS_3gui16WindowInfoHandleEEE");
*reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__SetInputWindowInfo) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient11Transaction18setInputWindowInfoERKNS_2spINS_14SurfaceControlEEENS2_INS_3gui16WindowInfoHandleEEE");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjiiRKNS_2spINS_7IBinderEEENS_3gui13LayerMetadataEPj");
*reinterpret_cast<void**>(&SurfaceComposerClient__CreateSurface) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjiiRKNS_2spINS_7IBinderEEENS_3gui13LayerMetadataEPj");
}
else if(安卓版本==15)
{
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android3gui13LayerMetadataC2Ev");
*reinterpret_cast<void**>(&LayerMetadata__Constructor) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android3gui13LayerMetadataC2Ev");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android3gui13LayerMetadata8setInt32Eji");
*reinterpret_cast<void**>(&LayerMetadata__setInt32) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android3gui13LayerMetadata8setInt32Eji");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient11Transaction18setInputWindowInfoERKNS_2spINS_14SurfaceControlEEERKNS_3gui10WindowInfoE:");
*reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__SetInputWindowInfo) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient11Transaction18setInputWindowInfoERKNS_2spINS_14SurfaceControlEEERKNS_3gui10WindowInfoE:");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjiiRKNS_2spINS_7IBinderEEENS_3gui13LayerMetadataEPj");
*reinterpret_cast<void**>(&SurfaceComposerClient__CreateSurface) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjiiRKNS_2spINS_7IBinderEEENS_3gui13LayerMetadataEPj");
}
else if(安卓版本==14)
{
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android3gui13LayerMetadataC2Ev");
*reinterpret_cast<void**>(&LayerMetadata__Constructor) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android3gui13LayerMetadataC2Ev");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android3gui13LayerMetadata8setInt32Eji");
*reinterpret_cast<void**>(&LayerMetadata__setInt32) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android3gui13LayerMetadata8setInt32Eji");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient11Transaction18setInputWindowInfoERKNS_2spINS_14SurfaceControlEEERKNS_3gui10WindowInfoE:");
*reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__SetInputWindowInfo) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient11Transaction18setInputWindowInfoERKNS_2spINS_14SurfaceControlEEERKNS_3gui10WindowInfoE:");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjiiRKNS_2spINS_7IBinderEEENS_3gui13LayerMetadataEPj");
*reinterpret_cast<void**>(&SurfaceComposerClient__CreateSurface) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjiiRKNS_2spINS_7IBinderEEENS_3gui13LayerMetadataEPj");
}
else if(安卓版本==12)
{
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient11Transaction5applyEb");
*reinterpret_cast<void**>(&SurfaceComposerClient__Transaction__Apply) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient11Transaction5applyEb");
}
else if(安卓版本==11)
{
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlENS_13LayerMetadataEPj");
*reinterpret_cast<void**>(&SurfaceComposerClient__CreateSurface) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlENS_13LayerMetadataEPj");
函数指针 = symbolMethod.Find(Gui系统库,"_ZNK7android14SurfaceControl10getSurfaceEv");
*reinterpret_cast<void**>(&SurfaceControl__GetSurface) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZNK7android14SurfaceControl10getSurfaceEv");
}
else if(安卓版本==10)
{
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlENS_13LayerMetadataE");
*reinterpret_cast<void**>(&SurfaceComposerClient__CreateSurface) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlENS_13LayerMetadataE");
函数指针 = symbolMethod.Find(Gui系统库,"_ZNK7android14SurfaceControl10getSurfaceEv");
*reinterpret_cast<void**>(&SurfaceControl__GetSurface) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZNK7android14SurfaceControl10getSurfaceEv");
}
else if(安卓版本==9)
{
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlEii");
*reinterpret_cast<void**>(&SurfaceComposerClient__CreateSurface_and9) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlEii");
函数指针 = symbolMethod.Find(Gui系统库,"_ZN7android21SurfaceComposerClient17getBuiltInDisplayEi");
*reinterpret_cast<void**>(&SurfaceComposerClient__GetBuiltInDisplay) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZN7android21SurfaceComposerClient17getBuiltInDisplayEi");
函数指针 = symbolMethod.Find(Gui系统库,"_ZNK7android14SurfaceControl10getSurfaceEv");
*reinterpret_cast<void**>(&SurfaceControl__GetSurface) = 函数指针;
if(!函数指针) __android_log_print(ANDROID_LOG_ERROR,"ImGui","[-] Patch method not found: %s","_ZNK7android14SurfaceControl10getSurfaceEv");
}
symbolMethod.Close(Utils系统库);
symbolMethod.Close(Gui系统库);
}
static const Functionals &GetInstance(const SymbolMethod &symbolMethod = {.Open = dlopen,.Find = dlsym,.Close = dlclose}) {
static Functionals functionals(symbolMethod);
return functionals;
}
};

struct String8
{
char data[1024];

String8(const char*const string)
{
Functionals::GetInstance().String8__Constructor(data,string);
}

~String8()
{
Functionals::GetInstance().String8__Destructor(data);
}

operator void*()
{
return reinterpret_cast<void*>(data);
}
};

struct LayerMetadata {
char data[1024];

LayerMetadata() {
if(9 < Functionals::GetInstance().安卓版本) {
Functionals::GetInstance().LayerMetadata__Constructor(data);
}
}

void setInt32(uint32_t key,int32_t value) {
Functionals::GetInstance().LayerMetadata__setInt32(data,key,value);
}

operator void*() {
if(9 < Functionals::GetInstance().安卓版本)
return reinterpret_cast<void*>(data);
else
return nullptr;
}
};

struct Surface
{
};

struct SurfaceControl
{
    void* data;
    int32_t width;
    int32_t height;
    bool skipScreenshot;

    SurfaceControl() : data(nullptr), width(0), height(0), skipScreenshot(false) {}
    SurfaceControl(void* data) : data(data), width(0), height(0), skipScreenshot(false) {}
    SurfaceControl(void* data, int32_t w, int32_t h, bool skip) : data(data), width(w), height(h), skipScreenshot(skip) {}

    // 允许隐式转换为 StrongPointer<void>& 以便传递给 Transaction 方法
    operator StrongPointer<void>&() {
        static StrongPointer<void> ptr;
        ptr.pointer = data;
        return ptr;
    }

    int32_t Validate()
    {
        if(nullptr==data)
            return 0;
        return Functionals::GetInstance().SurfaceControl__Validate(data);
    }

    Surface* GetSurface()
    {
        if(nullptr==data)
            return nullptr;
        auto result = Functionals::GetInstance().SurfaceControl__GetSurface(data);
        return reinterpret_cast<Surface*>(reinterpret_cast<size_t>(result.pointer) + sizeof(std::max_align_t) / 2);
    }

    void DisConnect()
    {
        if(nullptr==data)
            return;
        Functionals::GetInstance().SurfaceControl__DisConnect(data);
    }

    void DestroySurface(Surface* surface)
    {
        if(nullptr==data||nullptr==surface)
            return;
        Functionals::GetInstance().RefBase__DecStrong(reinterpret_cast<Surface*>(reinterpret_cast<size_t>(surface) - sizeof(std::max_align_t) / 2),this);
        DisConnect();
        Functionals::GetInstance().RefBase__DecStrong(data,this);
    }
};

struct SurfaceComposerClientTransaction
{
    char data[1024];

    SurfaceComposerClientTransaction()
    {
        Functionals::GetInstance().SurfaceComposerClient__Transaction__Constructor(data);
    }

    void* SetLayer(StrongPointer<void> &surfaceControl, int32_t z)
    {
        return Functionals::GetInstance().SurfaceComposerClient__Transaction__SetLayer(data, surfaceControl, z);
    }

    void* SetTrustedOverlay(StrongPointer<void> &surfaceControl, bool isTrustedOverlay)
    {
        return Functionals::GetInstance().SurfaceComposerClient__Transaction__SetTrustedOverlay(data, surfaceControl, isTrustedOverlay);
    }

    int32_t Apply(bool synchronous, bool oneWay)
    {
        if (12 >= Functionals::GetInstance().安卓版本)
            return reinterpret_cast<int32_t (*)(void*, bool)>(Functionals::GetInstance().SurfaceComposerClient__Transaction__Apply)(data, synchronous);
        else
            return Functionals::GetInstance().SurfaceComposerClient__Transaction__Apply(data, synchronous, oneWay);
    }

    void* SetFlags(StrongPointer<void> &surfaceControl, uint32_t flags, uint32_t mask)
    {
        if (Functionals::GetInstance().SurfaceComposerClient__Transaction__SetFlags == nullptr)
            return nullptr;
        return Functionals::GetInstance().SurfaceComposerClient__Transaction__SetFlags(data, surfaceControl, flags, mask);
    }

    void* SetLayerStack(StrongPointer<void> &surfaceControl, uint32_t layerStack) {
        if (Functionals::GetInstance().SurfaceComposerClient__Transaction__SetLayerStack == nullptr)
            return nullptr;
        return Functionals::GetInstance().SurfaceComposerClient__Transaction__SetLayerStack(data, surfaceControl, layerStack);
    }

    void Show(StrongPointer<void> &surfaceControl) {
        if (Functionals::GetInstance().SurfaceComposerClient__Transaction__Show == nullptr)
            return;
        Functionals::GetInstance().SurfaceComposerClient__Transaction__Show(data, surfaceControl);
    }

    void Hide(StrongPointer<void> &surfaceControl) {
        if (Functionals::GetInstance().SurfaceComposerClient__Transaction__Hide == nullptr)
            return;
        Functionals::GetInstance().SurfaceComposerClient__Transaction__Hide(data, surfaceControl);
    }

    void Reparent(StrongPointer<void> &surfaceControl, StrongPointer<void> &newParentHandle) {
        if (Functionals::GetInstance().SurfaceComposerClient__Transaction__Reparent == nullptr)
            return;
        Functionals::GetInstance().SurfaceComposerClient__Transaction__Reparent(data, surfaceControl, newParentHandle);
    }

    void SetMatrix(StrongPointer<void> &surfaceControl, float dsdx, float dtdx, float dsdy, float dtdy) {
        if (Functionals::GetInstance().SurfaceComposerClient__Transaction__SetMatrix == nullptr)
            return;
        Functionals::GetInstance().SurfaceComposerClient__Transaction__SetMatrix(data, surfaceControl, dsdx, dtdx, dsdy, dtdy);
    }

    void SetPosition(StrongPointer<void> &surfaceControl, float x, float y) {
        if (Functionals::GetInstance().SurfaceComposerClient__Transaction__SetPosition == nullptr)
            return;
        Functionals::GetInstance().SurfaceComposerClient__Transaction__SetPosition(data, surfaceControl, x, y);
    }
};

struct SurfaceComposerClient
{
char data[1024];

SurfaceComposerClient()
{
Functionals::GetInstance().SurfaceComposerClient__Constructor(data);
Functionals::GetInstance().RefBase__IncStrong(data,this);
}

SurfaceControl CreateSurface(const char* name, int32_t width, int32_t height, bool skipScrenshot) {
    void* parentHandle = nullptr;
    String8 windowName(name);
    LayerMetadata layerMetadata;
    uint32_t flags = 0;

    if(skipScrenshot && Functionals::GetInstance().安卓版本 >= 12) {
        flags |= 0x40;   // eSkipScreenshot
    }
    if(skipScrenshot && (Functionals::GetInstance().安卓版本 == 10 || Functionals::GetInstance().安卓版本 == 11)) {
        layerMetadata.setInt32(2u, 441731);
    }

    if(12 <= Functionals::GetInstance().安卓版本) {
        static void* fakeParentHandleForBinder = nullptr;
        parentHandle = &fakeParentHandleForBinder;
    }

    StrongPointer<void> result;

    // Android 8 和 9 使用旧版 API
    if(Functionals::GetInstance().安卓版本 == 8 || Functionals::GetInstance().安卓版本 == 9) {
        int32_t windowType = skipScrenshot ? 441731 : -1;
        result = Functionals::GetInstance().SurfaceComposerClient__CreateSurface_and9(
            data, windowName, width, height, 1, flags, parentHandle, windowType, -1);
    }
    // Android 10+ 使用新版 API
    else if(Functionals::GetInstance().安卓版本 >= 10) {
        result = Functionals::GetInstance().SurfaceComposerClient__CreateSurface(
            data, windowName, width, height, 1, flags, parentHandle, layerMetadata, nullptr);
    }

    // Android 12+ 设置可信覆盖和最高图层
    if(12 <= Functionals::GetInstance().安卓版本) {
        static SurfaceComposerClientTransaction transaction;
        transaction.SetTrustedOverlay(result, true);
        transaction.SetLayer(result, INT_MAX);
        transaction.Apply(false, true);
    }
    SurfaceControl sc(result.get(), width, height, skipScrenshot);
    return sc;
}

bool GetDisplayInfo(ui::DisplayState*displayInfo)
{
StrongPointer<void> defaultDisplay;

if(9 >= Functionals::GetInstance().安卓版本)
defaultDisplay = Functionals::GetInstance().SurfaceComposerClient__GetBuiltInDisplay(ui::DisplayType::DisplayIdMain);
else
{
if(14 > Functionals::GetInstance().安卓版本)
defaultDisplay = Functionals::GetInstance().SurfaceComposerClient__GetInternalDisplayToken();
else
{
auto displayIds = Functionals::GetInstance().SurfaceComposerClient__GetPhysicalDisplayIds();
if(displayIds.empty())
return false;

defaultDisplay = Functionals::GetInstance().SurfaceComposerClient__GetPhysicalDisplayToken(displayIds[0]);
}
}

if(nullptr==defaultDisplay.get())
return false;

if(11 <= Functionals::GetInstance().安卓版本)
return 0==Functionals::GetInstance().SurfaceComposerClient__GetDisplayState(defaultDisplay,displayInfo);
else
{
ui::DisplayInfo realDisplayInfo{};
if(0 != Functionals::GetInstance().SurfaceComposerClient__GetDisplayInfo(defaultDisplay,&realDisplayInfo))
return false;

displayInfo->layerStackSpaceRect.width = realDisplayInfo.w;
displayInfo->layerStackSpaceRect.height = realDisplayInfo.h;
displayInfo->方向 = static_cast<ui::Rotation>(realDisplayInfo.方向);

return true;
}
}

SurfaceControl MirrorSurface(SurfaceControl &surface, uint32_t layerStack) {
    if (Functionals::GetInstance().安卓版本 < 11)
        return {};
    if (surface.skipScreenshot) {
        __android_log_print(ANDROID_LOG_INFO, "ImGui", "[=] Surface not need mirror: skipScreenshot is true");
        return {};
    }
    if (0 == surface.width || 0 == surface.height) {
        __android_log_print(ANDROID_LOG_INFO, "ImGui", "[=] Surface not need mirror: width or height is 0");
        return {};
    }

    auto mirrorSurfacePtr = Functionals::GetInstance().SurfaceComposerClient__MirrorSurface(data, surface.data);
    if (nullptr == mirrorSurfacePtr.get()) {
        __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Failed to mirror surface: %u", layerStack);
        return {};
    }

    std::string mirrorRootName = "MirrorRoot@" + std::to_string(layerStack);
    auto mirrorRootSurface = CreateSurface(mirrorRootName.c_str(), surface.width, surface.height, false);
    if (!mirrorRootSurface.data) {
        __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Failed to create mirror root surface: %u", layerStack);
        return {};
    }

    SurfaceComposerClientTransaction transaction;
    transaction.SetLayer(mirrorRootSurface, INT_MAX);
    transaction.SetLayerStack(mirrorRootSurface, layerStack);
    transaction.Apply(false, true);

    transaction.SetLayerStack(mirrorSurfacePtr, layerStack);
    transaction.Show(mirrorSurfacePtr);
    StrongPointer<void> rootPtr;
    rootPtr.pointer = mirrorRootSurface.data;
    transaction.Reparent(mirrorSurfacePtr, rootPtr);
    transaction.Apply(false, true);

    return mirrorRootSurface;
}

void ZoomSurface(SurfaceControl &surface, float scaleX, float scaleY) {
    SurfaceComposerClientTransaction transaction;
    transaction.SetMatrix(surface, scaleX, 0.f, 0.f, scaleY);
    transaction.Apply(false, true);
}

void MoveSurface(SurfaceControl &surface, float x, float y) {
    SurfaceComposerClientTransaction transaction;
    transaction.SetPosition(surface, x, y);
    transaction.Apply(false, true);
}
};

// 显示器解析辅助结构
struct DumpDisplayInfo {
    std::string uniqueId;
    uint32_t currentLayerStack;
    struct {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;
    } currentLayerStackRect;

    static DumpDisplayInfo MakeFromRawDumpInfo(const std::string_view &uniqueId, const std::string_view &currentLayerStack, const std::string_view &currentLayerStackRect) {
        DumpDisplayInfo result;
        result.uniqueId = std::string{uniqueId.begin(), uniqueId.end()};
        result.currentLayerStack = static_cast<uint32_t>(std::stoul(std::string{currentLayerStack.begin(), currentLayerStack.end()}));

        auto leftPos = currentLayerStackRect.find("(") + 1;
        auto topPos = currentLayerStackRect.find(", ", leftPos);
        auto rightPos = currentLayerStackRect.find(" - ", topPos + 2);
        auto bottomPos = currentLayerStackRect.find(", ", rightPos + 3);
        auto endPos = currentLayerStackRect.find(")", bottomPos + 2);

        result.currentLayerStackRect.left = std::stoi(std::string{currentLayerStackRect.begin() + leftPos, currentLayerStackRect.begin() + topPos});
        result.currentLayerStackRect.top = std::stoi(std::string{currentLayerStackRect.begin() + topPos + 2, currentLayerStackRect.begin() + rightPos});
        result.currentLayerStackRect.right = std::stoi(std::string{currentLayerStackRect.begin() + rightPos + 3, currentLayerStackRect.begin() + bottomPos});
        result.currentLayerStackRect.bottom = std::stoi(std::string{currentLayerStackRect.begin() + bottomPos + 2, currentLayerStackRect.begin() + endPos});

        return result;
    }
};

struct MirrorLayerTransform {
    bool isAspectRatioSimilar;
    float widthScale;
    float heightScale;
    float offsetX;
    float offsetY;
};

inline std::vector<DumpDisplayInfo> ParseDumpDisplayInfo(const std::string_view &dumpDisplayInfo) {
    constexpr auto SubStringView = [](const std::string_view &str, std::string_view start, std::string_view end, int startOffset = 0) -> std::string_view {
        auto startIt = str.find(start, startOffset);
        if (std::string::npos == startIt)
            return {};
        auto endIt = str.find(end, startIt + start.size());
        if (std::string::npos == endIt)
            return {};
        return str.substr(startIt + start.size(), endIt - startIt - start.size());
    };

    std::vector<DumpDisplayInfo> result;
    auto dumpDisplayInfoIt = std::string_view::npos;
    while (std::string_view::npos != (dumpDisplayInfoIt = dumpDisplayInfo.find("DisplayDeviceInfo", dumpDisplayInfoIt + 1))) {
        auto uniqueId = SubStringView(dumpDisplayInfo, "mUniqueId=", "\n", dumpDisplayInfoIt);
        auto currentLayerStack = SubStringView(dumpDisplayInfo, "mCurrentLayerStack=", "\n", dumpDisplayInfoIt);
        auto currentLayerStackRect = SubStringView(dumpDisplayInfo, "mCurrentLayerStackRect=", "\n", dumpDisplayInfoIt);

        if ("-1" == currentLayerStack) {
            __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] %s -> Current layer stack is -1, skipping", std::string{uniqueId.begin(), uniqueId.end()}.c_str());
            continue;
        }
        result.push_back(DumpDisplayInfo::MakeFromRawDumpInfo(uniqueId, currentLayerStack, currentLayerStackRect));
    }
    return result;
}

inline MirrorLayerTransform CalcMirrorLayerTransform(float targetWidth, float targetHeight, float sourceWidth, float sourceHeight, float epsilon = 0.002) {
    if (0.f == targetHeight || 0.f == sourceHeight)
        throw std::runtime_error("[-] Invalid height");

    MirrorLayerTransform result{
        .isAspectRatioSimilar = std::abs(targetWidth / targetHeight - sourceWidth / sourceHeight) < epsilon,
        .widthScale = sourceWidth / targetWidth,
        .heightScale = sourceHeight / targetHeight,
    };
    if (result.isAspectRatioSimilar)
        return result;

    if (result.widthScale > result.heightScale) {
        result.offsetX = (sourceWidth - targetWidth * result.heightScale) / 2;
        result.widthScale = result.heightScale;
    } else {
        result.offsetY = (sourceHeight - targetHeight * result.widthScale) / 2;
        result.heightScale = result.widthScale;
    }

    return result;
}

}

class ANativeWindowCreator
{
public:
struct DisplayInfo
{
int32_t 方向;
int32_t width;
int32_t height;
};

public:
static detail::SurfaceComposerClient &GetComposerInstance()
{
static detail::SurfaceComposerClient surfaceComposerClient;

return surfaceComposerClient;
}

static DisplayInfo GetDisplayInfo()
{
auto &surfaceComposerClient = GetComposerInstance();
detail::ui::DisplayState displayInfo{};

if(!surfaceComposerClient.GetDisplayInfo(&displayInfo))
return {};

DisplayInfo local_displayInfo{0};
int32_t local_orientation = static_cast<int32_t>(displayInfo.方向);
int32_t local_abs_x = (displayInfo.layerStackSpaceRect.width > displayInfo.layerStackSpaceRect.height ? displayInfo.layerStackSpaceRect.width : displayInfo.layerStackSpaceRect.height);
int32_t local_abs_y = (displayInfo.layerStackSpaceRect.width < displayInfo.layerStackSpaceRect.height ? displayInfo.layerStackSpaceRect.width : displayInfo.layerStackSpaceRect.height);
if(local_orientation==1||local_orientation==3) {
local_displayInfo.width = local_abs_x;
local_displayInfo.height = local_abs_y;
} else {
local_displayInfo.width = local_abs_y;
local_displayInfo.height = local_abs_x;
}
local_displayInfo.方向 = local_orientation;
return local_displayInfo;
}

static ANativeWindow*Create(const char*name,int32_t width = -1,int32_t height = -1,bool skipScrenshot_ = false)
{
auto &surfaceComposerClient = GetComposerInstance();

while (-1==width||-1==height)
{
detail::ui::DisplayState displayInfo{};

if(!surfaceComposerClient.GetDisplayInfo(&displayInfo))
break;

width = displayInfo.layerStackSpaceRect.width;
height = displayInfo.layerStackSpaceRect.height;

break;
}
auto surfaceControl = surfaceComposerClient.CreateSurface(name,width,height,skipScrenshot_);
auto nativeWindow = reinterpret_cast<ANativeWindow*>(surfaceControl.GetSurface());

m_cachedSurfaceControl.emplace(nativeWindow,std::move(surfaceControl));
return nativeWindow;
}

static void Destroy(ANativeWindow*nativeWindow)
{
auto it = m_cachedSurfaceControl.find(nativeWindow);
if(it == m_cachedSurfaceControl.end())
return;

m_cachedSurfaceControl[nativeWindow].DestroySurface(reinterpret_cast<detail::Surface*>(nativeWindow));
m_cachedSurfaceControl.erase(nativeWindow);
}

static void SetSkipScreenshot(ANativeWindow* nativeWindow, bool skipScreenshot) {
    auto it = m_cachedSurfaceControl.find(nativeWindow);
    if (it == m_cachedSurfaceControl.end())
        return;

    auto& surfaceControl = it->second;
    if (surfaceControl.skipScreenshot == skipScreenshot)
        return;

    surfaceControl.skipScreenshot = skipScreenshot;

    if (detail::Functionals::GetInstance().安卓版本 >= 12) {
        detail::SurfaceComposerClientTransaction transaction;
        const uint32_t mask = 0x40 | 0x80;
        uint32_t flags = skipScreenshot ? mask : 0;

        detail::StrongPointer<void> scPtr;
        scPtr.pointer = surfaceControl.data;
        transaction.SetFlags(scPtr, flags, mask);
        transaction.Apply(false, true);
    }
}

static void ProcessMirrorDisplay()
{
    static std::chrono::steady_clock::time_point lastTime{};
    static std::unordered_map<uint32_t, std::vector<detail::SurfaceControl>> cachedLayerStackMirrorSurfaces;
    static std::unordered_set<uint32_t> cachedLayerStackScales;

    if (detail::Functionals::GetInstance().安卓版本 < 11||detail::Functionals::GetInstance().安卓版本 >=16)
        return;
    if (std::chrono::steady_clock::now() - lastTime < std::chrono::seconds(1))
        return;

    std::vector<detail::SurfaceControl*> surfacesNeedToMirror;
    for (auto &[_, surfaceControl] : m_cachedSurfaceControl) {
        if (!surfaceControl.skipScreenshot)
            surfacesNeedToMirror.push_back(&surfaceControl);
    }
    if (surfacesNeedToMirror.empty())
        return;

    auto pipe = popen("dumpsys display", "r");
    if (!pipe) {
        __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Failed to run dumpsys command");
        return;
    }

    char buffer[512]{};
    std::string dumpDisplayResult;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        dumpDisplayResult += buffer;
    pclose(pipe);

    auto dumpDisplayInfos = detail::ParseDumpDisplayInfo(dumpDisplayResult);
    for (auto &displayInfo : dumpDisplayInfos) {
        static int32_t builtinDisplayWidth = -1, builtinDisplayHeight = -1;
        if (0 == displayInfo.currentLayerStack) {
            builtinDisplayWidth = displayInfo.currentLayerStackRect.right;
            builtinDisplayHeight = displayInfo.currentLayerStackRect.bottom;
        }

        if (0 == displayInfo.currentLayerStack)
            continue;

        if (!cachedLayerStackMirrorSurfaces.contains(displayInfo.currentLayerStack)) {
            __android_log_print(ANDROID_LOG_INFO, "ImGui", "[=] New display layerstack detected: [%s] -> %u", displayInfo.uniqueId.c_str(), displayInfo.currentLayerStack);
            for (auto surfaceControl : surfacesNeedToMirror) {
                auto mirrorLayer = GetComposerInstance().MirrorSurface(*surfaceControl, displayInfo.currentLayerStack);
                cachedLayerStackMirrorSurfaces[displayInfo.currentLayerStack].emplace_back(std::move(mirrorLayer));
            }
        }

        if (-1 != builtinDisplayWidth && -1 != builtinDisplayHeight && cachedLayerStackMirrorSurfaces.contains(displayInfo.currentLayerStack)) {
            if ((displayInfo.currentLayerStackRect.right != builtinDisplayWidth || displayInfo.currentLayerStackRect.bottom != builtinDisplayHeight) && !cachedLayerStackScales.contains(displayInfo.currentLayerStack)) {
                auto &composerInstance = GetComposerInstance();
                auto &mirrorLayers = cachedLayerStackMirrorSurfaces.at(displayInfo.currentLayerStack);
                auto transformParams = detail::CalcMirrorLayerTransform(
                    builtinDisplayWidth, builtinDisplayHeight,
                    displayInfo.currentLayerStackRect.right, displayInfo.currentLayerStackRect.bottom);

                for (auto &mirrorLayer : mirrorLayers) {
                    composerInstance.ZoomSurface(mirrorLayer, transformParams.widthScale, transformParams.heightScale);
                    if (!transformParams.isAspectRatioSimilar)
                        composerInstance.MoveSurface(mirrorLayer, transformParams.offsetX, transformParams.offsetY);
                    cachedLayerStackScales.emplace(displayInfo.currentLayerStack);
                }
            }
        }
    }

    lastTime = std::chrono::steady_clock::now();
}

private:
inline static std::unordered_map<ANativeWindow*,detail::SurfaceControl> m_cachedSurfaceControl;
};
}

#undef ResolveMethod

#endif // !A_NATIVE_WINDOW_CREATOR_H
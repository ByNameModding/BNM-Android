/********** USER AREA **************/

//#define UNITY_VER 171 // 2017.1.x // il2cpp version: 24
//#define UNITY_VER 172 // From 2017.2.x to 2017.4.x // il2cpp version: 24
//#define UNITY_VER 181 // 2018.1.x // il2cpp version: 24
//#define UNITY_VER 182 // 2018.2.x // il2cpp version: 24
//#define UNITY_VER 183 // From 2018.3.x to 2018.4.x // il2cpp version: 24.1
//#define UNITY_VER 191 // From 2019.1.x to 2019.2.x // il2cpp version: 24.2
//#define UNITY_VER 193 // 2019.3.x // il2cpp version: 24.3
//#define UNITY_VER 194 // 2019.4.x // il2cpp version: 24.3 and 24.4
//#define UNITY_VER 201 // 2020.1.x // il2cpp version: 24.3 and 24.4
#define UNITY_VER 202 // From 2020.2.x to 2020.3.x (They are same) // il2cpp version: 27.1
//#define UNITY_VER 211 // 2021.1.x // il2cpp version: 27.2
//#define UNITY_VER 212 // 2021.2.x // il2cpp version: 27 and 27.2
//#define UNITY_VER 213 // 2021.3.x - 2022.1.x // il2cpp version: 28

#ifndef NDEBUG

//! DEBUG LOGS
#define BNM_DEBUG

//! INFO LOGS
#define BNM_INFO

//! ERROR LOGS
#define BNM_ERROR
#endif

//! Include your string obfuscator
#define OBFUSCATE_BNM(str) str // const char *
#define OBFUSCATES_BNM(str) std::string(str) // std::string
#define BNMTAG OBFUSCATE_BNM("ByNameModding")

//! Include your hooking software
//! Substrate MSHook with And64InlineHook 
//!!!!!!!! They may don't work !!!!!!!!
/* 
#if defined(__ARM_ARCH_7A__) || defined(__i386__) // armv7 or x86
#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>
#elif defined(__aarch64__) //arm64-v8a
#include <And64InlineHook/And64InlineHook.hpp>
#endif
auto HOOK = [](auto ptr, auto newMethod, auto&& oldBytes) {
    if (ptr != 0){
#if defined(__aarch64__)
        A64HookFunction((void *)ptr, (void *) newMethod, (void **) &oldBytes);
#else
        MSHookFunction((void *)ptr, (void *) newMethod, (void **) &oldBytes);
#endif
    }
};
*/
//! DobbyHook
//!!!!!!!! Recommended !!!!!!!!
#include <dobby.h>
auto HOOK = [](auto ptr, auto newMethod, auto&& oldBytes) {
    if (ptr != 0){
        DobbyHook((void *)ptr, (void *) newMethod, (void **) &oldBytes);
    }
};

// For System.Collections.Generic.Dictionary

// If the game uses .NET 3.5 uncomment this define
// .NET 3.5 is deprecated but some old games use it

// #define BNM_DOTNET35

/********** USER AREA **************/



#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS // If you want try port for windows
#endif

#ifdef BNM_INFO
#define LOGIBNM(...) ((void)__android_log_print(4,  BNMTAG, __VA_ARGS__))
#else
#define LOGIBNM(...)
#endif

#ifdef BNM_DEBUG
#define LOGDBNM(...) ((void)__android_log_print(3,  BNMTAG, __VA_ARGS__))
#else
#define LOGDBNM(...)
#endif

#ifdef BNM_ERROR
#define LOGEBNM(...) ((void)__android_log_print(6,  BNMTAG, __VA_ARGS__))
#else
#define LOGEBNM(...)
#endif



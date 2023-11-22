#if __cplusplus < 202002L
static_assert(false, "ByNameModding requre C++20 and upper!");
#endif


/********** USER AREA **************/

//#define UNITY_VER 171 // 2017.1.x
//#define UNITY_VER 172 // From 2017.2.x to 2017.4.x
//#define UNITY_VER 181 // 2018.1.x
//#define UNITY_VER 182 // 2018.2.x
//#define UNITY_VER 183 // From 2018.3.x to 2018.4.x
//#define UNITY_VER 191 // From 2019.1.x to 2019.2.x
//#define UNITY_VER 193 // 2019.3.x
//#define UNITY_VER 194 // 2019.4.x
//#define UNITY_VER 201 // 2020.1.x
#define UNITY_VER 202 // From 2020.2.x to 2020.3.19 (They are probably same because BNM don't use things like Il2CppCodeGen)
//#define UNITY_VER 203 // From 2020.3.20 to 2020.3.xx
//#define UNITY_VER 211 // 2021.1.x (You need set UNITY_PATCH_VER to 24 if build 24 and upper)
//#define UNITY_VER 212 // 2021.2.x
//#define UNITY_VER 213 // 2021.3.x
//#define UNITY_VER 221 // 2022.1.x
//#define UNITY_VER 222 // 2022.2.x - 2022.3.x
//#define UNITY_VER 231 // 2023.1.x+

#define UNITY_PATCH_VER 11 // For some special cases

// Allow to use deprecated methods
// #define BNM_DEPRECATED

//! Allow use GetOffset
//! GetOffset can only be used in very special cases, it should not be used on a permanent basis
//! Wrong usage can increase the risk of errors
// #define BNM_ALLOW_GET_OFFSET

//! Remove the use of code to synchronize threads
//! Recommended for internal use
// #define BNM_DISABLE_MULTI_THREADING_SYNC

#ifndef NDEBUG

//! Methods str() in structures
#define BNM_ALLOW_STR_METHODS

//! Use signal in IsAllocated
#define BNM_ALLOW_SAFE_IS_ALLOCATED

//! Use signal when trying to find a generic object
#define BNM_ALLOW_SAFE_GENERIC_CREATION

//! Проверять объекты MONO_STRUCTS в их методах
#define BNM_ALLOW_SELF_CHECKS

#define BNM_DEBUG

#define BNM_INFO

#define BNM_ERROR

#define BNM_WARNING

#endif

//! Include your string obfuscator
#define OBFUSCATE_BNM(str) str // const char *
#define OBFUSCATES_BNM(str) std::string(str) // std::string

//! Include your hooking software
//! Substrate MSHook with And64InlineHook
/* 
#if defined(__ARM_ARCH_7A__) || defined(__i386__) // armv7 or x86
#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>
#elif defined(__aarch64__) // arm64-v8a
#include <And64InlineHook/And64InlineHook.hpp>
#endif
template<typename PTR_T, typename NEW_T, typename OLD_T>
inline void HOOK(PTR_T ptr, NEW_T newMethod, OLD_T&& oldBytes) {
    if (ptr != 0){
#if defined(__aarch64__)
        A64HookFunction((void *)ptr, (void *) newMethod, (void **) &oldBytes);
#else
        MSHookFunction((void *)ptr, (void *) newMethod, (void **) &oldBytes);
#endif
    }
}
*/
//! DobbyHook
//!!!!!!!! Recommended !!!!!!!!
#include <dobby.h>

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void HOOK(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) {
    if (((void *)ptr) != nullptr)
        DobbyHook((void *)ptr, (void *) newMethod, (void **) &oldBytes);
}

// For System.Collections.Generic.Dictionary (monoDictionary)

// If the game uses .NET 3.5 uncomment this define
// .NET 3.5 is deprecated but some old games use it

// #define BNM_DOTNET35

// If you need hide dl calls or use custom dl for external BNM initialization
#define BNM_dlopen dlopen
#define BNM_dlsym dlsym
#define BNM_dlclose dlclose
#define BNM_dladdr dladdr

#include <thread>
#define BNM_thread std::thread

// Disabling BNM automatic loading when your lib loaded
// Define it when you using BNM::HardBypass to speed up loading or when you externally loading BNM
#define BNM_DISABLE_AUTO_LOAD
#define BNM_DISABLE_NEW_CLASSES 0

// Can make game crashes on arm64
// #define BNM_USE_APPDOMAIN // Use System.AppDomain to find il2cpp::vm::Assembly::GetAllAssemblies


/********** USER AREA **************/

#include <android/log.h>

#define BNM_TAG "ByNameModding"

#ifdef BNM_ALLOW_SELF_CHECKS
#define BNM_CHECK_SELF(returnValue) if (!SelfCheck()) return returnValue
#else
#define BNM_CHECK_SELF(returnValue) ((void)0)
#endif

#ifdef BNM_INFO
#define BNM_LOG_INFO(...) ((void)__android_log_print(4,  BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_INFO(...) ((void)0)
#endif

#ifdef BNM_DEBUG
#define BNM_LOG_DEBUG(...) ((void)__android_log_print(3,  BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_DEBUG(...) ((void)0)
#endif

#ifdef BNM_ERROR
#define BNM_LOG_ERR(...) ((void)__android_log_print(6,  BNM_TAG, __VA_ARGS__))
#define BNM_LOG_ERR_IF(condition, ...) if (condition) ((void)__android_log_print(6,  BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_ERR(...) ((void)0)
#define BNM_LOG_ERR_IF(condition, ...) ((void)0)
#endif

#ifdef BNM_WARNING
#define BNM_LOG_WARN(...) ((void)__android_log_print(5,  BNM_TAG, __VA_ARGS__))
#define BNM_LOG_WARN_IF(condition, ...) if (condition) ((void)__android_log_print(5,  BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_WARN(...) ((void)0)
#define BNM_LOG_WARN_IF(condition, ...) ((void)0)
#endif

#define BNM_VER "1.0"
#if __cplusplus < 202002L
static_assert(false, "ByNameModding requires C++20 and above!");
#endif

#pragma once

//#define UNITY_VER 56 // 5.6.4f1
//#define UNITY_VER 171 // 2017.1.x
//#define UNITY_VER 172 // 2017.2.x - 2017.4.x
//#define UNITY_VER 181 // 2018.1.x
//#define UNITY_VER 182 // 2018.2.x
//#define UNITY_VER 183 // 2018.3.x - 2018.4.x
//#define UNITY_VER 191 // 2019.1.x - 2019.2.x
//#define UNITY_VER 193 // 2019.3.x
//#define UNITY_VER 194 // 2019.4.x
//#define UNITY_VER 201 // 2020.1.x
//#define UNITY_VER 202 // 2020.2.x - 2020.3.19
//#define UNITY_VER 203 // 2020.3.20 - 2020.3.xx
//#define UNITY_VER 211 // 2021.1.x (Need set UNITY_PATCH_VER to 24 if x (2021.1.x) >= 24)
//#define UNITY_VER 212 // 2021.2.x
//#define UNITY_VER 213 // 2021.3.x
//#define UNITY_VER 221 // 2022.1.x
#define UNITY_VER 222 // 2022.2.x - 2022.3.x
//#define UNITY_VER 231 // 2023.1.x
//#define UNITY_VER 232 // 2023.2.x+

#define UNITY_PATCH_VER 32 // Для особых случаев (For special cases)

//! Включить устаревший код (если есть)
//! Allow to use deprecated methods (if any)
// #define BNM_DEPRECATED

//! Разрешить использование кода для синхронизации потоков
//! Рекомендуется отключать при внутреннем использовании
//! Allow the use of code to synchronize streams
//! It is recommended to disable it during internal use
// #define BNM_ALLOW_MULTI_THREADING_SYNC

//! Для System.Collections.Generic.Dictionary (Dictionary)
//! Если игра использует .NET 3.5 раскомментируйте этот define
//! .NET 3.5 устарел, но часть старых игр используют его
//! For the System.Collections.Generic.Dictionary (Dictionary)
//! If game uses .NET 3.5 uncomment this define
//! .NET 3.5 is outdated, but some older games use it
// #define BNM_DOTNET35

//! Включить код создания новых классов и модификации существующих
//! Allow code for creating new classes and modifying existing ones
#define BNM_CLASSES_MANAGEMENT

//! Включить код создания coroutine. ТРЕБУЕТСЯ ClassesManagement!
//! Enable coroutine creation code. REQUIRED ClassesManagement!
#define BNM_COROUTINE

//! Отключить авто подмену через таблицу виртуальных методов в ClassesManagement
//! Disable auto hook via virtual method table in ClassesManagement
// #define BNM_AUTO_HOOK_DISABLE_VIRTUAL_HOOK

//! Старые добрые времена...
//! The good old days...
// #define BNM_OLD_GOOD_DAYS

#ifndef NDEBUG

//! Методы str() в структурах
//! str() methods in structures
#define BNM_ALLOW_STR_METHODS

//! Использовать signal в IsAllocated
//! Use signal in IsAllocated
#define BNM_ALLOW_SAFE_IS_ALLOCATED

//! Проверять объекты mono в их методах
//! Check mono's objects in their methods
#define BNM_ALLOW_SELF_CHECKS

#define BNM_DEBUG

#define BNM_INFO

#define BNM_ERROR

#define BNM_WARNING

#endif

//! Добавьте ваш шифровщик строк
//! Add your string encryptor
#define BNM_OBFUSCATE(str) str // const char *
//! Data obfuscated using this macro, can be freed, after BNM loaded. Only for advanced users! If you don't know what and how, just use your basic macro here.
//! Данные, защифрованне этим define, могут быть удалены после загрузки BNM. Только для опытных пользователей! Если вы не знаете, что и как, просто используйте свой базовый define.
#define BNM_OBFUSCATE_TMP(str) str // const char *

// Shadowhook
/*
template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *HOOK(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) {
    if ((void *) ptr != nullptr) return shadowhook_hook_func_addr((void *)ptr, (void *) newMethod, (void **) &oldBytes);
    return nullptr;
}

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *HOOK(PTR_T ptr, NEW_T newMethod, T_OLD &&oldBytes) {
    if ((void *) ptr != nullptr) return shadowhook_hook_func_addr((void *)ptr, (void *) newMethod, (void **) &oldBytes);
    return nullptr;
}

template<typename PTR_T>
inline void UNHOOK(PTR_T ptr) {
    if ((void *) ptr != nullptr) shadowhook_unhook((void *)ptr);
}
*/


// Dobby
/*
#include <dobby.h>

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *HOOK(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) {
    if ((void *) ptr != nullptr) DobbyHook((void *)ptr, (void *) newMethod, (void **) &oldBytes);
    return (void *) ptr;
}

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *HOOK(PTR_T ptr, NEW_T newMethod, T_OLD &&oldBytes) {
    if ((void *) ptr != nullptr) DobbyHook((void *)ptr, (void *) newMethod, (void **) &oldBytes);
    return (void *) ptr;
}

template<typename PTR_T>
inline void UNHOOK(PTR_T ptr) {
    if ((void *) ptr != nullptr) DobbyDestroy((void *)ptr);
}
*/

// Dummy
#include <cassert>

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *HOOK(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) {
    assert("Нет ПО для подмены! (No hooking software!)");
    if ((void *) ptr != nullptr) ((void)0);
    return nullptr;
}

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *HOOK(PTR_T ptr, NEW_T newMethod, T_OLD &&oldBytes) {
    assert("Нет ПО для подмены! (No hooking software!)");
    if ((void *) ptr != nullptr) ((void)0);
    return nullptr;
}

template<typename PTR_T>
inline void UNHOOK(PTR_T ptr) {
    assert("Нет ПО для подмены! (No hooking software!)");
    if ((void *) ptr != nullptr) ((void)0);
}

#include <dlfcn.h>

// Если вам нужно скрыть вызовы dlfcn или использовать ваш dl для загрузки BNM в игре извне
// If you need to hide dlfcn calls or use your dl to load BNM in the game from the outside
#define BNM_dlopen dlopen
#define BNM_dlsym dlsym
#define BNM_dlclose dlclose
#define BNM_dladdr dladdr


#include <cstdlib>

// Если вам нужно скрыть методы работы с памятью
// If you need to hide memory management methods
#define BNM_malloc malloc
#define BNM_free free

#include <android/log.h>

#define BNM_TAG "ByNameModding"

#ifdef BNM_ALLOW_SELF_CHECKS
#define BNM_CHECK_SELF(returnValue) if (!SelfCheck()) return returnValue
#else
#define BNM_CHECK_SELF(returnValue) ((void)0)
#endif

#ifdef BNM_INFO
#define BNM_LOG_INFO(...) ((void)__android_log_print(4, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_INFO(...) ((void)0)
#endif

#ifdef BNM_DEBUG
#define BNM_LOG_DEBUG(...) ((void)__android_log_print(3, BNM_TAG, __VA_ARGS__))
#define BNM_LOG_DEBUG_IF(condition, ...) if (condition) ((void)__android_log_print(3, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_DEBUG(...) ((void)0)
#define BNM_LOG_DEBUG_IF(...) ((void)0)
#endif

#ifdef BNM_ERROR
#define BNM_LOG_ERR(...) ((void)__android_log_print(6, BNM_TAG, __VA_ARGS__))
#define BNM_LOG_ERR_IF(condition, ...) if (condition) ((void)__android_log_print(6, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_ERR(...) ((void)0)
#define BNM_LOG_ERR_IF(condition, ...) ((void)0)
#endif

#ifdef BNM_WARNING
#define BNM_LOG_WARN(...) ((void)__android_log_print(5, BNM_TAG, __VA_ARGS__))
#define BNM_LOG_WARN_IF(condition, ...) if (condition) ((void)__android_log_print(5, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_WARN(...) ((void)0)
#define BNM_LOG_WARN_IF(condition, ...) ((void)0)
#endif

namespace BNM {
#if defined(__LP64__)
    typedef long BNM_INT_PTR;
    typedef unsigned long BNM_PTR;
#else
    typedef int BNM_INT_PTR;
    typedef unsigned int BNM_PTR;
#endif
}

#define BNM_VER "2.2.3"
#if __cplusplus < 202002L
static_assert(false, "ByNameModding требуется C++20 и выше!");
#endif


/********** ОБЛАСТЬ ПОЛЬЗОВАТЕЛЯ **************/

//#define UNITY_VER 171 // 2017.1.x
//#define UNITY_VER 172 // От 2017.2.x до 2017.4.x
//#define UNITY_VER 181 // 2018.1.x
//#define UNITY_VER 182 // 2018.2.x
//#define UNITY_VER 183 // От 2018.3.x до 2018.4.x
//#define UNITY_VER 191 // От 2019.1.x до 2019.2.x
//#define UNITY_VER 193 // 2019.3.x
//#define UNITY_VER 194 // 2019.4.x
//#define UNITY_VER 201 // 2020.1.x
#define UNITY_VER 202 // От 2020.2.x до 2020.3.19 (Они почти одинаковы, потому что BNM не использует структуры на подобии Il2CppCodeGen)
//#define UNITY_VER 203 // От 2020.3.20 до 2020.3.xx
//#define UNITY_VER 211 // 2021.1.x (Нужно установить UNITY_PATCH_VER на 24 если x (2021.1.x) равен или больше 24)
//#define UNITY_VER 212 // 2021.2.x
//#define UNITY_VER 213 // 2021.3.x
//#define UNITY_VER 221 // 2022.1.x
//#define UNITY_VER 222 // 2022.2.x - 2022.3.x
//#define UNITY_VER 231 // 2023.1.x+

#define UNITY_PATCH_VER 11 // Для особых случаев

//! Включить устаревший код (если есть)
// #define BNM_DEPRECATED

//! Разрешить использовать GetOffset
//! GetOffset можно использовать только в крайних случаях, он не должен быть использован на постоянной основе
//! Неправильное использование может повысить риск ошибок
// #define BNM_ALLOW_GET_OFFSET

//! Разрешить использование кода для синхронизации потоков
//! Рекомендуется отключать при внутреннем использовании
#define BNM_ALLOW_MULTI_THREADING_SYNC

//! Для System.Collections.Generic.Dictionary (monoDictionary)
//! Если игра использует .NET 3.5 раскоментируйте этот define
//! .NET 3.5 устарел, но часть старых игр используют его
// #define BNM_DOTNET35

//! Отключение автоматической загрузки BNM при загрузке вашей библиотеки
//! Раскоментируйте его, когда вы используете BNM::TryForceLoadIl2CppByPath для увелечения скорости загрузки или когда вы загружаете BNM извне
#define BNM_DISABLE_AUTO_LOAD

//! Отключить код создания новых классов и модификации старых
#define BNM_DISABLE_NEW_CLASSES 0

//! Использовать System.AppDomain для получения il2cpp::vm::Assembly::GetAllAssemblies
//! Может привести к сбоям игры на arm64
// #define BNM_USE_APPDOMAIN

#ifndef NDEBUG

//! Методы str() в структурах
#define BNM_ALLOW_STR_METHODS

//! Использовать signal в IsAllocated
#define BNM_ALLOW_SAFE_IS_ALLOCATED

//! Проверять объекты MONO_STRUCTS в их методах
#define BNM_ALLOW_SELF_CHECKS

#define BNM_DEBUG

#define BNM_INFO

#define BNM_ERROR

#define BNM_WARNING

#endif

//! Добавьте ваш шифровщик строк
#define OBFUSCATE_BNM(str) str // const char *
#define OBFUSCATES_BNM(str) std::string(str) // std::string

//! Добавьте вашу утилиту для подмены методов
//! Substrate MSHook with And64InlineHook
/* 
#if defined(__ARM_ARCH_7A__) || defined(__i386__) // armv7 или x86
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
};
*/
//! DobbyHook
//!!!!!!!! Рекомендуется !!!!!!!!
#include <dobby.h>

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void HOOK(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) {
    if (((void *)ptr) != nullptr)
        DobbyHook((void *)ptr, (void *) newMethod, (void **) &oldBytes);
}

// Если вам нужно скрыть вызовы dlfcn или использовать ваш dl для загрузки BNM в игре извне
#define BNM_dlopen dlopen
#define BNM_dlsym dlsym
#define BNM_dlclose dlclose
#define BNM_dladdr dladdr

#include <thread>
#define BNM_thread std::thread


/********** ОБЛАСТЬ ПОЛЬЗОВАТЕЛЯ **************/

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

#define BNM_VER "1.3"
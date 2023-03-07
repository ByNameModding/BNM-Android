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
//#define UNITY_VER 221 // 2022.1.x // TODO: Разобратся с 2022.2.0+

#define UNITY_PATCH_VER 11 // Для особых случаёв 

// Включить устаревшие методы (если есть)
// #define BNM_DEPRECATED

#ifndef NDEBUG

//! Отладочный журнал
#define BNM_DEBUG

//! Информационый журнал
#define BNM_INFO

//! Журнал ошибок
#define BNM_ERROR

//! Журнал предупреждений
#define BNM_WARNING

#endif

//! Добавте ваш шифровщик строк
#define OBFUSCATE_BNM(str) str // const char *
#define OBFUSCATES_BNM(str) std::string(str) // std::string
#define BNMTAG OBFUSCATE_BNM("ByNameModding")

//! Добавте вашу утилиту для подметы методов
//! Substrate MSHook with And64InlineHook 
//!!!!!!!! Они могут не работать !!!!!!!!
/* 
#if defined(__ARM_ARCH_7A__) || defined(__i386__) // armv7 or x86
#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>
#elif defined(__aarch64__) // arm64-v8a
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
//!!!!!!!! Рекомендуется !!!!!!!!
#include <dobby.h>
auto HOOK = [](auto ptr, auto newMethod, auto&& oldBytes) {
    if (((void *)ptr) != nullptr)
        DobbyHook((void *)ptr, (void *) newMethod, (void **) &oldBytes);
};

// Для System.Collections.Generic.Dictionary (monoDictionary)

// Если игра использует .NET 3.5 раскоментируйте этот define
// .NET 3.5 усторел, но часть старых игр используют его

// #define BNM_DOTNET35

// Если вам нужно скрыть вызовы dlfcn или использовать ваш dl для загрузки BNM в игре извне
#define BNM_dlopen dlopen
#define BNM_dlsym dlsym
#define BNM_dlclose dlclose
#define BNM_dladdr dladdr

// Отключение автоматической загрузки BNM при загрузке вашей библиотеки
// Раскоментируйте его, когда вы используете BNM::HardBypass для ускорения скорсти загрузки или когда вы загружаете BNM извне
// #define BNM_DISABLE_AUTO_LOAD
#define BNM_DISABLE_NEW_CLASSES 0

// Может привести к сбоям игры на arm64
// #define BNM_USE_APPDOMAIN // Использовать System.AppDomain для получения il2cpp::vm::Assembly::GetAllAssemblies
/********** ОБЛАСТЬ ПОЛЬЗОВАТЕЛЯ **************/


#ifdef BNM_INFO
#define LOGIBNM(...) ((void)__android_log_print(4,  BNMTAG, __VA_ARGS__))
#else
#define LOGIBNM(...) ((void)0)
#endif

#ifdef BNM_DEBUG
#define LOGDBNM(...) ((void)__android_log_print(3,  BNMTAG, __VA_ARGS__))
#else
#define LOGDBNM(...) ((void)0)
#endif

#ifdef BNM_ERROR
#define LOGEBNM(...) ((void)__android_log_print(6,  BNMTAG, __VA_ARGS__))
#else
#define LOGEBNM(...) ((void)0)
#endif

#ifdef BNM_WARNING
#define LOGWBNM(...) ((void)__android_log_print(5,  BNMTAG, __VA_ARGS__))
#else
#define LOGWBNM(...) ((void)0)
#endif

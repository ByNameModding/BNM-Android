#pragma once

#include <csetjmp>

#include <string_view>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"

namespace BNM {
    // Проверить, действителен ли указатель
    template <typename T>
    inline bool IsAllocated(T x) {
#ifdef BNM_ALLOW_SAFE_IS_ALLOCATED
        static jmp_buf jump;
        static sighandler_t handler = [](int) { longjmp(jump, 1); };
        char c;
        bool ok = true;
        sighandler_t old_handler = signal(SIGSEGV, handler);
        if (!setjmp (jump)) c = *(char *) (x); else ok = false;
        signal(SIGSEGV, old_handler);
        return ok;
#else
        return (char *)x != nullptr;
#endif
    }
    template <typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    inline T CheckObj(T obj) {
        if (obj && IsAllocated(obj)) return obj;
        return nullptr;
    }

    namespace Structures::Mono { struct String; }

    // Метод создания C#-строк собираемых сборщиком мусора, если он включён в игре
    Structures::Mono::String *CreateMonoString(const char *str);
    Structures::Mono::String *CreateMonoString(const std::string_view &str);

    // Получить внешние методы (icall)
    void *GetExternMethod(const std::string_view &str);

    // Истина, когда il2cpp и BNM загружены
    bool IsLoaded();

    // Не закрывать его! BNM вызовет сбой из-за этого.
    void *GetIl2CppLibraryHandle();

    // Распаковка объекта, просто копия метода из il2cpp
    template<typename T>
    inline T UnboxObject(T obj) { return (T)(void *)(((char *)obj) + sizeof(BNM::IL2CPP::Il2CppObject)); }

    template<typename MET_T, typename PTR_T>
    inline void InitFunc(MET_T& method, PTR_T ptr) {
        *(void **)&method = (void *)ptr;
    }

    // Для статических полей потоков
    namespace PRIVATE_FieldUtils {
        void GetStaticValue(IL2CPP::FieldInfo *info, void *value);
        void SetStaticValue(IL2CPP::FieldInfo *info, void *value);
    }

    struct CompileTimeClass;
#ifdef BNM_DEBUG
    namespace Utils {
        void *OffsetInLib(void *);
        void LogCompileTimeClass(const BNM::CompileTimeClass &compileTimeClass);
    }
#endif
}
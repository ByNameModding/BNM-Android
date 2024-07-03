#pragma once

#include <csetjmp>

#include <string_view>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"

namespace BNM {
    // Check if the pointer is valid
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

    // The method of creating C# strings collected by the garbage collector, if it is enabled in the game
    Structures::Mono::String *CreateMonoString(const char *str);
    Structures::Mono::String *CreateMonoString(const std::string_view &str);

    // Get external methods (icall)
    void *GetExternMethod(const std::string_view &str);

    // True when il2cpp and BNM are loaded
    bool IsLoaded();

    // Don't close it! BNM will cause a crash because of this.
    void *GetIl2CppLibraryHandle();

    // Unpacking the object, just a copy of the method from il2cpp
    template<typename T>
    inline T UnboxObject(T obj) { return (T)(void *)(((char *)obj) + sizeof(BNM::IL2CPP::Il2CppObject)); }

    template<typename MET_T, typename PTR_T>
    inline void InitFunc(MET_T& method, PTR_T ptr) {
        *(void **)&method = (void *)ptr;
    }

    // For thread static fields
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

#if UNITY_VER >= 232
    inline BNM_INT_PTR UnmarshalUnityObject(BNM_INT_PTR gcHandlePtr) {
        auto gcHandle = *(BNM_INT_PTR *)gcHandlePtr;
        return gcHandle & ~(BNM_INT_PTR)1;
    }
#endif

}
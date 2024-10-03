#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Class.hpp"

namespace BNM {

#pragma pack(push, 1)

    template <typename Ret> struct Method;

    struct MethodBase {
        inline constexpr MethodBase() = default;
        inline MethodBase(const MethodBase &other) = default;
        MethodBase(const IL2CPP::MethodInfo *info);
        MethodBase(const IL2CPP::Il2CppReflectionMethod *reflectionMethod);

        // Set object
        MethodBase &SetInstance(IL2CPP::Il2CppObject *val);

        inline IL2CPP::MethodInfo *GetInfo() const { return _data; }
        inline BNM_PTR GetOffset() const { return _data ? (BNM_PTR) _data->methodPointer : 0; }

        // If the method is `generic`, then you can try to get it with a specific set of types
        MethodBase GetGeneric(const std::initializer_list<CompileTimeClass> &templateTypes) const;

        // Get the virtual version of the method from the installed object. Only for non-static methods.
        MethodBase Virtualize() const;

        // Fast set instance
        inline MethodBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline MethodBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline MethodBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Check is method alive
        inline bool Initialized() const noexcept { return _init; }

#ifdef BNM_ALLOW_STR_METHODS
        // Get data
        inline std::string str() const {
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            if (!_init) return BNM_OBFUSCATE(DBG_BNM_MSG_MethodBase_str_nullptr);
            return Class(_data->return_type).str() + BNM_OBFUSCATE(" ") +
                Class(_data->kls).str() + BNM_OBFUSCATE(".(") +
                    _data->name + BNM_OBFUSCATE("){" DBG_BNM_MSG_MethodBase_str_args_count ": ") +
                std::to_string(_data->parameters_count) + BNM_OBFUSCATE("}") +
                (_isStatic ? BNM_OBFUSCATE("(" DBG_BNM_MSG_MethodBase_str_static ")") : BNM_OBFUSCATE(""));
#undef kls
        }
#endif

        // Cast method
        template<typename NewType> inline Method<NewType> &cast() const { return (Method<NewType> &)*this; }

        IL2CPP::MethodInfo *_data{};
        IL2CPP::Il2CppObject *_instance{};
        uint8_t _init : 1 = false, _isStatic : 1 = false, _isVirtual : 1 = false;
    };

#pragma pack(pop)

    // Method hook by changing MethodInfo
    bool InvokeHookImpl(IL2CPP::MethodInfo *m, void *newMet, void **oldMet);

    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &oldMet) {
        if (targetMethod.Initialized()) return InvokeHookImpl(targetMethod._data, (void *)newMet, (void **)&oldMet);
        return false;
    }
    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &&oldMet) {
        if (targetMethod.Initialized()) return InvokeHookImpl(targetMethod._data, (void *)newMet, (void **)&oldMet);
        return false;
    }
    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(IL2CPP::MethodInfo *m, T_NEW newMet, T_OLD &oldMet) { return InvokeHookImpl(m, (void *)newMet, (void **)&oldMet); }
    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(IL2CPP::MethodInfo *m, T_NEW newMet, T_OLD &&oldMet) { return InvokeHookImpl(m, (void *)newMet, (void **)&oldMet); }

    // Hook of method by changing the table of virtual methods of a class
    bool VirtualHookImpl(BNM::Class targetClass, IL2CPP::MethodInfo *m, void *newMet, void **oldMet);

    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::Class targetClass, const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &oldMet) {
        if (targetClass && targetMethod.Initialized()) return VirtualHookImpl(targetClass, targetMethod._data, (void *)newMet, (void **)&oldMet);
        return false;
    }
    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::Class targetClass, const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &&oldMet) {
        if (targetClass && targetMethod.Initialized()) return VirtualHookImpl(targetClass, targetMethod._data, (void *)newMet, (void **)&oldMet);
        return false;
    }
    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::Class targetClass, IL2CPP::MethodInfo *m, T_NEW newMet, T_OLD &oldMet) {
        return VirtualHookImpl(targetClass, m, (void *)newMet, (void **)&oldMet);
    }
    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::Class targetClass, IL2CPP::MethodInfo *m, T_NEW newMet, T_OLD &&oldMet) {
        return VirtualHookImpl(targetClass, m, (void *)newMet, (void **)&oldMet);
    }

    template<typename NEW_T, typename T_OLD>
    void HOOK(const BNM::MethodBase &targetMethod, NEW_T newMethod, T_OLD &oldBytes) {
        if (targetMethod.Initialized()) ::HOOK((void *) targetMethod._data->methodPointer, newMethod, oldBytes);
    }
    template<typename NEW_T, typename T_OLD>
    void HOOK(const BNM::MethodBase &targetMethod, NEW_T newMethod, T_OLD &&oldBytes) {
        if (targetMethod.Initialized()) ::HOOK((void *) targetMethod._data->methodPointer, newMethod, oldBytes);
    }
}

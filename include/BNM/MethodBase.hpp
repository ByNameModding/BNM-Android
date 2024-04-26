#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Class.hpp"


namespace BNM {
    namespace UnityEngine {
        struct Object;
    }

#pragma pack(push, 1)

    struct CompileTimeClass;
    template <typename T> struct Method;

    struct MethodBase {
        inline constexpr MethodBase() = default;
        inline MethodBase(const MethodBase &other) = default;
        MethodBase(const IL2CPP::MethodInfo *info);
        MethodBase(const IL2CPP::Il2CppReflectionMethod *reflectionMethod);

        // Установить объект
        MethodBase &SetInstance(IL2CPP::Il2CppObject *val);

        inline IL2CPP::MethodInfo *GetInfo() const { return _data; }
        inline BNM_PTR GetOffset() const { return _data ? (BNM_PTR) _data->methodPointer : 0; }

        // Если метод является `generic`, то можно попытаться получить его с определённым набором типов
        MethodBase GetGeneric(const std::initializer_list<CompileTimeClass> &templateTypes) const;

        // Получить virtual версию метода из установленного объекта. Только для нестатических методов.
        MethodBase Virtualize() const;

        // Быстрая установка объекта
        inline MethodBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline MethodBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline MethodBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Проверить, жив ли метод
        inline bool Initialized() const noexcept { return _init; }
        inline operator bool() noexcept { return Initialized(); }
        inline operator bool() const noexcept { return Initialized(); }

#ifdef BNM_ALLOW_STR_METHODS
        // Получить данные
        inline std::string str() const {
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            if (!_init) return OBFUSCATE_BNM(DBG_BNM_MSG_MethodBase_str_nullptr);
            return Class(_data->return_type).str() + OBFUSCATE_BNM(" ") +
                Class(_data->kls).str() + OBFUSCATE_BNM(".(") +
                    _data->name + OBFUSCATE_BNM("){" DBG_BNM_MSG_MethodBase_str_args_count ": ") +
                std::to_string(_data->parameters_count) + OBFUSCATE_BNM("}") +
                (_isStatic ? OBFUSCATE_BNM("(" DBG_BNM_MSG_MethodBase_str_static ")") : OBFUSCATE_BNM(""));
#undef kls
        }
#endif

        // Получить метод с указанным типом
        template<typename NewType> inline Method<NewType> &cast() const { return (Method<NewType> &)*this; }

        IL2CPP::MethodInfo *_data{};
        IL2CPP::Il2CppObject *_instance{};
        uint8_t _init : 1 = false, _isStatic : 1 = false, _isVirtual : 1 = false;
    };

#pragma pack(pop)

    // Подмена метода путем изменения MethodInfo
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

    // Подмена метода путем изменения таблицы виртуальных методов класса
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

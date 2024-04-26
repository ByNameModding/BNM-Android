#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "MethodBase.hpp"
#include "Utils.hpp"


namespace BNM {
    namespace UnityEngine {
        struct Object;
    }

#pragma pack(push, 1)

    template<typename Ret = void>
    struct Method : public MethodBase {
        inline constexpr Method() = default;
        template<typename OtherType>
        Method(const Method<OtherType> &other) : MethodBase(other) {}
        Method(const IL2CPP::MethodInfo *info) : MethodBase(info) {}
        Method(const IL2CPP::Il2CppReflectionMethod *reflectionMethod) : MethodBase(reflectionMethod) {}
        Method(const MethodBase &other) : MethodBase(other) {}

        // Быстрая установка объекта
        inline Method<Ret> &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline Method<Ret> &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline Method<Ret> &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Вызвать метод
        template<typename ...Parameters>
        Ret Call(Parameters ...parameters) const {
            if (!_init) { if constexpr (std::is_same<Ret, void>::value) return; else return {}; }
            bool canInfo = true;
            if (sizeof...(Parameters) != _data->parameters_count){
                canInfo = false;
                BNM_LOG_WARN(DBG_BNM_MSG_Method_Call_Warn, str().c_str());
            }
            if (!_isStatic && !CheckObj(_instance)) {
                BNM_LOG_ERR(DBG_BNM_MSG_Method_Call_Error, str().c_str());
                if constexpr (std::is_same_v<Ret, void>) return; else return {};
            }
            auto method = _data;
            if (!_isStatic) {
                if (canInfo) return (((Ret(*)(IL2CPP::Il2CppObject *, Parameters..., IL2CPP::MethodInfo *)) method->methodPointer)(_instance, parameters..., method));
                return (((Ret(*)(IL2CPP::Il2CppObject *, Parameters...)) method->methodPointer)(_instance, parameters...));
            }
#if UNITY_VER > 174
            if (canInfo) return ((Ret(*)(Parameters..., IL2CPP::MethodInfo *)) method->methodPointer)(parameters..., method);
            return (((Ret(*)(Parameters...)) method->methodPointer)(parameters...));
#else
            if (canInfo) return ((Ret(*)(void*,Parameters...,IL2CPP::MethodInfo *))method->methodPointer)(nullptr, parameters..., method);
            return (((Ret(*)(void*,Parameters...))method->methodPointer)(nullptr, parameters...));
#endif
        }

        // Быстро вызвать метод
        template<typename ...Parameters> inline Ret operator ()(Parameters ...parameters) { return Call(parameters...); }
        template<typename ...Parameters> inline Ret operator ()(Parameters ...parameters) const { return Call(parameters...); }

        // Скопировать другой метод, только для автоматического приведения типов
        inline Method<Ret> &operator =(const MethodBase &other) {
            _data = other._data;
            _instance = other._instance;
            _init = other._init;
            _isStatic = other._isStatic;
            _isVirtual = other._isVirtual;
            return *this;
        }
    };

#pragma pack(pop)

    template<typename ...Parameters>
    BNM::IL2CPP::Il2CppObject *Class::CreateNewObjectParameters(Parameters ...parameters) const {
        if (!_data) return nullptr;
        TryInit();
        auto name = OBFUSCATE_BNM(".ctor");
        auto method = GetMethod(name, sizeof...(Parameters));
        auto instance = CreateNewInstance();
        method.template cast<void>()[instance](parameters...);
        return instance;
    }

    template<typename ...Parameters>
    BNM::IL2CPP::Il2CppObject *Class::CreateNewObjectTypes(const std::initializer_list<std::string_view> &parameterNames, Parameters ...parameters) const {
        if (!_data) return nullptr;
        TryInit();
        auto name = OBFUSCATE_BNM(".ctor");
        auto method = GetMethod(name, parameterNames);
        auto instance = CreateNewInstance();
        method.template cast<void>()[instance](parameters...);
        return instance;
    }
}

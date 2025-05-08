#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "MethodBase.hpp"
#include "Utils.hpp"

// NOLINTBEGIN
namespace BNM {

#pragma pack(push, 1)

    /**
        @brief Typed class for working with il2cpp methods.

        This class provides API for calling methods.

        @tparam Ret Return type of method
    */
    template<typename Ret>
    struct Method : public MethodBase {

        /**
            @brief Create empty method.
        */
        constexpr Method() noexcept = default;

        /**
            @brief Copy method.
            @param other Other method
            @tparam OtherType Type of other method
        */
        template<typename OtherType>
        Method(const Method<OtherType> &other) : MethodBase(other) {}

        /**
            @brief Create method from il2cpp method.
            @param info Il2cpp method
        */
        Method(const IL2CPP::MethodInfo *info) : MethodBase(info) {}

        /**
            @brief Create method from il2cpp reflection method.
            @param info Il2cpp reflection method
        */
        Method(const IL2CPP::Il2CppReflectionMethod *reflectionMethod) : MethodBase(reflectionMethod) {}

        /**
            @brief Convert base method to typed method.
            @param other Base method
        */
        Method(const MethodBase &other) : MethodBase(other) {}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Method
        */
        inline Method<Ret> &operator[](void *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Method
        */
        inline Method<Ret> &operator[](IL2CPP::Il2CppObject *instance) { SetInstance(instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Method
        */
        inline Method<Ret> &operator[](UnityEngine::Object *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Call method.
            @param parameters Parameters of method
            @return Result of method call if method is valid, otherwise default value.
        */
        template<typename ...Parameters>
        Ret Call(Parameters ...parameters) const {
            if (!_data) { BNM_LOG_ERR(DBG_BNM_MSG_Method_Call_Dead); return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>(); }
            bool canInfo = true;
            if (sizeof...(Parameters) != _data->parameters_count){
                canInfo = false;
                BNM_LOG_WARN(DBG_BNM_MSG_Method_Call_Warn, str().c_str());
            }
            if (!_isStatic && !IsAllocated(_instance)) {
                BNM_LOG_ERR(DBG_BNM_MSG_Method_Call_Error, str().c_str());
                return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>();
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

        /**
            @brief Call method operator. Alias for BNM::Method::Call.
        */
        template<typename ...Parameters> inline Ret operator ()(Parameters ...parameters) const { return Call(parameters...); }


        /**
            @brief Convert base method to typed method.
            @param other Base method
        */
        inline Method<Ret> &operator =(const MethodBase &other) {
            _data = other._data;
            _instance = other._instance;
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
        auto name = BNM_OBFUSCATE(".ctor");
        auto method = GetMethod(name, sizeof...(Parameters));
        auto instance = CreateNewInstance();
        method.template cast<void>()[instance](parameters...);
        return instance;
    }

    template<typename ...Parameters>
    BNM::IL2CPP::Il2CppObject *Class::CreateNewObjectTypes(const std::initializer_list<std::string_view> &parameterNames, Parameters ...parameters) const {
        if (!_data) return nullptr;
        TryInit();
        auto name = BNM_OBFUSCATE(".ctor");
        auto method = GetMethod(name, parameterNames);
        auto instance = CreateNewInstance();
        method.template cast<void>()[instance](parameters...);
        return instance;
    }
}
// NOLINTEND

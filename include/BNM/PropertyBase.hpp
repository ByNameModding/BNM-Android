#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "MethodBase.hpp"

namespace BNM {

#pragma pack(push, 1)

    template <typename T> struct Property;

    struct PropertyBase {
        inline constexpr PropertyBase() noexcept = default;
        inline PropertyBase(const PropertyBase &other) = default;
        PropertyBase(const IL2CPP::PropertyInfo *info);
        PropertyBase(const MethodBase &newGetter, const MethodBase &newSetter);

        // Set object
        PropertyBase &SetInstance(IL2CPP::Il2CppObject *val);

#ifdef BNM_ALLOW_STR_METHODS
        // Get data
        [[nodiscard]] inline std::string str() const {
            if (!_hasGetter && !_hasSetter) return BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_nullptr);
            auto isStatic = _hasGetter ? _getter._isStatic : _setter._isStatic;
            return Class(_data->parent).str() + BNM_OBFUSCATE(" ") +
                Class(_data->parent).str() + BNM_OBFUSCATE(".(") +
                ((Structures::Mono::String *)_data->name)->str() + BNM_OBFUSCATE("){" DBG_BNM_MSG_PropertyBase_str_getter ": ") + (_hasGetter ? BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_not_exists)) + BNM_OBFUSCATE(", " DBG_BNM_MSG_PropertyBase_str_setter ": ") + (_hasSetter ? BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_not_exists)) + BNM_OBFUSCATE("}") +
                (isStatic ? BNM_OBFUSCATE("(" DBG_BNM_MSG_PropertyBase_str_static ")") : BNM_OBFUSCATE(""));
        }
#endif

        // Fast set instance
        inline PropertyBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline PropertyBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline PropertyBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Check is method alive
        inline bool Initialized() const noexcept { return _hasGetter || _hasSetter; }

        // Cast property
        template<typename NewRet> inline Property<NewRet> &cast() const { return (Property<NewRet> &)*this; }

        IL2CPP::PropertyInfo *_data{};
        MethodBase _getter{}, _setter{};
        uint8_t _hasGetter : 1 = false, _hasSetter : 1 = false;
    };

#pragma pack(pop)

}

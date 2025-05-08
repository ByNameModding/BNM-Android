#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "MethodBase.hpp"

// NOLINTBEGIN
namespace BNM {

#pragma pack(push, 1)

    template <typename T> struct Property;

    /**
        @brief Class base for working with il2cpp properties.

        This class provides fields and some basic functions to allow toget and set properties.
    */
    struct PropertyBase {
        /**
            @brief Create empty property base.
        */
        inline constexpr PropertyBase() noexcept = default;

        /**
            @brief Copy property.
            @param other Other property
        */
        inline PropertyBase(const PropertyBase &other) = default;

        /**
            @brief Create property from il2cpp property.
            @param info Il2cpp property
        */
        PropertyBase(const IL2CPP::PropertyInfo *info);

        /**
            @brief Set property instance if it's non-static.
            @param instance Instance
            @return Reference to current PropertyBase
        */
        PropertyBase &SetInstance(IL2CPP::Il2CppObject *val);

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current PropertyBase
        */
        inline PropertyBase &operator[](void *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current PropertyBase
        */
        inline PropertyBase &operator[](IL2CPP::Il2CppObject *instance) { SetInstance(instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current PropertyBase
        */
        inline PropertyBase &operator[](UnityEngine::Object *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Check if property is valid.
            @return State of property
        */
        inline bool IsValid() const noexcept { return _hasGetter || _hasSetter; }

#ifdef BNM_ALLOW_STR_METHODS

        /**
            @brief Get full property name.

            Returns string with full property name: ClassFullName.(property name){getter: exists/not exist, setter: exists/not exist} (static)/nothing

            @return Property's full name or "Dead property"
        */
        [[nodiscard]] inline std::string str() const {
            if (!_data) return BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_nullptr);
            auto isStatic = _hasGetter ? _getter._isStatic : _setter._isStatic;
            return Class(_data->parent).str() + BNM_OBFUSCATE(".(") +
                   ((Structures::Mono::String *)_data->name)->str() + BNM_OBFUSCATE("){" DBG_BNM_MSG_PropertyBase_str_getter ": ") + (_hasGetter ? BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_not_exists)) + BNM_OBFUSCATE(", " DBG_BNM_MSG_PropertyBase_str_setter ": ") + (_hasSetter ? BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_PropertyBase_str_not_exists)) + BNM_OBFUSCATE("}") +
                   (isStatic ? BNM_OBFUSCATE("(" DBG_BNM_MSG_PropertyBase_str_static ")") : BNM_OBFUSCATE(""));
        }
#endif

        /**
           @brief Cast event to be able to get and set it.
        */
        template<typename NewRet> inline Property<NewRet> &cast() const { return (Property<NewRet> &)*this; }

        IL2CPP::PropertyInfo *_data{};
        MethodBase _getter{}, _setter{};
        uint8_t _hasGetter : 1 = false, _hasSetter : 1 = false;
    };

#pragma pack(pop)

}
// NOLINTEND

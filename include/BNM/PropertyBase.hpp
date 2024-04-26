#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "UnityStructures.hpp"
#include "MethodBase.hpp"


namespace BNM {

#pragma pack(push, 1)

    template <typename T> struct Property;

    struct PropertyBase {
        inline constexpr PropertyBase() noexcept = default;
        inline PropertyBase(const PropertyBase &other) = default;
        PropertyBase(const IL2CPP::PropertyInfo *info);
        PropertyBase(const MethodBase &newGetter, const MethodBase &newSetter);

        // Установить объект
        PropertyBase &SetInstance(IL2CPP::Il2CppObject *val);
#ifdef BNM_ALLOW_STR_METHODS
        // Получить данные
        [[nodiscard]] inline std::string str() const {
            if (!_hasGetter && !_hasSetter) return OBFUSCATE_BNM(DBG_BNM_MSG_PropertyBase_str_nullptr);
            auto isStatic = _hasGetter ? _getter._isStatic : _setter._isStatic;
            return Class(_data->parent).str() + OBFUSCATE_BNM(" ") +
                Class(_data->parent).str() + OBFUSCATE_BNM(".(") +
                ((Structures::Mono::String *)_data->name)->str() + OBFUSCATE_BNM("){" DBG_BNM_MSG_PropertyBase_str_getter ": ") + (_hasGetter ? OBFUSCATE_BNM(DBG_BNM_MSG_PropertyBase_str_exists) : OBFUSCATE_BNM(DBG_BNM_MSG_PropertyBase_str_not_exists)) + OBFUSCATE_BNM(", " DBG_BNM_MSG_PropertyBase_str_setter ": ") + (_hasSetter ? OBFUSCATE_BNM(DBG_BNM_MSG_PropertyBase_str_exists) : OBFUSCATE_BNM(DBG_BNM_MSG_PropertyBase_str_not_exists)) + OBFUSCATE_BNM("}") +
                (isStatic ? OBFUSCATE_BNM("(" DBG_BNM_MSG_PropertyBase_str_static ")") : OBFUSCATE_BNM(""));
        }
#endif

        // Быстрая установка объекта
        inline PropertyBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline PropertyBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline PropertyBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Изменить тип свойства
        template<typename NewRet> inline Property<NewRet> &cast() const { return (Property<NewRet> &)*this; }

        IL2CPP::PropertyInfo *_data{};
        MethodBase _getter{}, _setter{};
        uint8_t _hasGetter : 1 = false, _hasSetter : 1 = false;
    };

#pragma pack(pop)

}

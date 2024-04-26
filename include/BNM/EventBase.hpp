#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "UnityStructures.hpp"
#include "MethodBase.hpp"


namespace BNM {

#pragma pack(push, 1)

    template <typename Ret, typename ...Args> struct Event;

    struct EventBase {
        inline constexpr EventBase() noexcept = default;
        inline EventBase(const EventBase &other) = default;
        EventBase(const IL2CPP::EventInfo *info);
        EventBase(const MethodBase &newAdd, const MethodBase &newRemove, const MethodBase &newRaise);

        // Установить объект
        EventBase &SetInstance(IL2CPP::Il2CppObject *val);
#ifdef BNM_ALLOW_STR_METHODS
        // Получить данные
        [[nodiscard]] inline std::string str() const {
            if (!_hasAdd && !_hasRemove && !_hasRaise) return OBFUSCATE_BNM(DBG_BNM_MSG_EventBase_str_nullptr);
            auto isStatic = _hasAdd ? _add._isStatic : _hasRemove ? _remove._isStatic : _raise._isStatic;
            return Class(_data->parent).str() + OBFUSCATE_BNM(" ") +
                   Class(_data->parent).str() + OBFUSCATE_BNM(".(") +
                   ((Structures::Mono::String *)_data->name)->str() +
                   OBFUSCATE_BNM("){" DBG_BNM_MSG_EventBase_str_add ": ") + (_hasAdd ? OBFUSCATE_BNM(DBG_BNM_MSG_EventBase_str_exists) : OBFUSCATE_BNM(DBG_BNM_MSG_EventBase_str_not_exists)) +
                   OBFUSCATE_BNM(", " DBG_BNM_MSG_EventBase_str_remove ": ") + (_hasRemove ? OBFUSCATE_BNM(DBG_BNM_MSG_EventBase_str_exists) : OBFUSCATE_BNM(DBG_BNM_MSG_EventBase_str_not_exists)) +
                   OBFUSCATE_BNM(", " DBG_BNM_MSG_EventBase_str_raise ": ") + (_hasRaise ? OBFUSCATE_BNM(DBG_BNM_MSG_EventBase_str_exists) : OBFUSCATE_BNM(DBG_BNM_MSG_EventBase_str_not_exists)) +
                   OBFUSCATE_BNM("}") + (isStatic ? OBFUSCATE_BNM("(" DBG_BNM_MSG_EventBase_str_static ")") : OBFUSCATE_BNM(""));
        }
#endif

        // Быстрая установка объекта
        inline EventBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline EventBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline EventBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Изменить тип свойства
        template<typename NewRet, typename ...NewArgs> inline Event<NewRet, NewArgs...> &cast() const { return (Event<NewRet, NewArgs...> &)*this; }

        IL2CPP::EventInfo *_data{};
        MethodBase _add{}, _remove{}, _raise{};
        uint8_t _hasAdd : 1 = false, _hasRemove : 1 = false, _hasRaise : 1 = false;
    };

#pragma pack(pop)

}

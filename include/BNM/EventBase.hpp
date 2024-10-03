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

        // Set object
        EventBase &SetInstance(IL2CPP::Il2CppObject *val);
#ifdef BNM_ALLOW_STR_METHODS
        // Get data
        [[nodiscard]] inline std::string str() const {
            if (!_hasAdd && !_hasRemove && !_hasRaise) return BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_nullptr);
            auto isStatic = _hasAdd ? _add._isStatic : _hasRemove ? _remove._isStatic : _raise._isStatic;
            return Class(_data->parent).str() + BNM_OBFUSCATE(" ") +
                   Class(_data->parent).str() + BNM_OBFUSCATE(".(") +
                   ((Structures::Mono::String *)_data->name)->str() +
                   BNM_OBFUSCATE("){" DBG_BNM_MSG_EventBase_str_add ": ") + (_hasAdd ? BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_not_exists)) +
                   BNM_OBFUSCATE(", " DBG_BNM_MSG_EventBase_str_remove ": ") + (_hasRemove ? BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_not_exists)) +
                   BNM_OBFUSCATE(", " DBG_BNM_MSG_EventBase_str_raise ": ") + (_hasRaise ? BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_not_exists)) +
                   BNM_OBFUSCATE("}") + (isStatic ? BNM_OBFUSCATE("(" DBG_BNM_MSG_EventBase_str_static ")") : BNM_OBFUSCATE(""));
        }
#endif

        // Fast set instance
        inline EventBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline EventBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline EventBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Cast event
        template<typename NewRet, typename ...NewArgs> inline Event<NewRet, NewArgs...> &cast() const { return (Event<NewRet, NewArgs...> &)*this; }

        IL2CPP::EventInfo *_data{};
        MethodBase _add{}, _remove{}, _raise{};
        uint8_t _hasAdd : 1 = false, _hasRemove : 1 = false, _hasRaise : 1 = false;
    };

#pragma pack(pop)

}

#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "UnityStructures.hpp"
#include "MethodBase.hpp"

// NOLINTBEGIN
namespace BNM {

#pragma pack(push, 1)

    template <typename Ret, typename ...Parameters> struct Event;

    /**
        @brief Class base for working with il2cpp events.

        This class provides fields and some basic functions to allow call, add and remove methods in event.
    */
    struct EventBase {
        /**
            @brief Create empty event base.
        */
        inline constexpr EventBase() noexcept = default;

        /**
            @brief Copy event.
            @param other Other event
        */
        inline EventBase(const EventBase &other) = default;

        /**
            @brief Create event from il2cpp event.
            @param info Il2cpp event
        */
        EventBase(const IL2CPP::EventInfo *info);

        /**
            @brief Set event instance if it's non-static.
            @param instance Instance
            @return Reference to current EventBase
        */
        EventBase &SetInstance(IL2CPP::Il2CppObject *instance);

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current EventBase
        */
        inline EventBase &operator[](void *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current EventBase
        */
        inline EventBase &operator[](IL2CPP::Il2CppObject *instance) { SetInstance(instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current EventBase
        */
        inline EventBase &operator[](UnityEngine::Object *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Check if event is valid.
            @return State of event
        */
        [[nodiscard]] inline bool IsValid() const noexcept { return _data; }

#ifdef BNM_ALLOW_STR_METHODS

        /**
            @brief Get full event name.

            Returns string with full event name: ClassFullName.(event name){add method: exists/not exist, remove method: exists/not exist, raise method: exists/not exist} (static)/nothing

            @return Event's full name or "Dead event".
        */
        [[nodiscard]] inline std::string str() const {
            if (!_data) return BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_nullptr);
            auto isStatic = _hasAdd ? _add._isStatic : _hasRemove ? _remove._isStatic : _raise._isStatic;
            return Class(_data->parent).str() + BNM_OBFUSCATE(".(") +
                   ((Structures::Mono::String *)_data->name)->str() +
                   BNM_OBFUSCATE("){" DBG_BNM_MSG_EventBase_str_add ": ") + (_hasAdd ? BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_not_exists)) +
                   BNM_OBFUSCATE(", " DBG_BNM_MSG_EventBase_str_remove ": ") + (_hasRemove ? BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_not_exists)) +
                   BNM_OBFUSCATE(", " DBG_BNM_MSG_EventBase_str_raise ": ") + (_hasRaise ? BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_exists) : BNM_OBFUSCATE(DBG_BNM_MSG_EventBase_str_not_exists)) +
                   BNM_OBFUSCATE("}") + (isStatic ? BNM_OBFUSCATE("(" DBG_BNM_MSG_EventBase_str_static ")") : BNM_OBFUSCATE(""));
        }
#endif

        /**
           @brief Cast event to be able to add, remove and raise it.
        */
        template<typename NewRet, typename ...NewParameters> inline Event<NewRet, NewParameters...> &cast() const { return (Event<NewRet, NewParameters...> &)*this; }

        IL2CPP::EventInfo *_data{};
        MethodBase _add{}, _remove{}, _raise{};
        uint8_t _hasAdd : 1 = false, _hasRemove : 1 = false, _hasRaise : 1 = false;
    };

#pragma pack(pop)

}
// NOLINTEND

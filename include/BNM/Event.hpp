#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "EventBase.hpp"
#include "Utils.hpp"

#include <type_traits>

// NOLINTBEGIN
namespace BNM {

#pragma pack(push, 1)

    /**
        @brief Typed class for working with il2cpp events.

        This class provides API for adding, removing and rising event.

        @tparam Ret Return type of event
        @tparam Parameters Parameters of event
    */
    template<typename Ret = void, typename ...Parameters>
    struct Event : EventBase {

        /**
            @brief Create empty event base.
        */
        inline constexpr Event() = default;

        /**
            @brief Copy event.
            @param other Other event
            @tparam OtherType Type of other event
        */
        template<typename OtherType>
        Event(const Event<OtherType> &other) : EventBase(other) {}

        /**
            @brief Create event from il2cpp event.
            @param info Il2cpp event
        */
        Event(const IL2CPP::EventInfo *info) : EventBase(info) {}

        /**
            @brief Convert base event to typed event.
            @param other Base event
        */
        Event(const EventBase &other) : EventBase(other) {}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Event
        */
        inline Event<Ret, Parameters...> &operator[](void *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Event
        */
        inline Event<Ret, Parameters...> &operator[](IL2CPP::Il2CppObject *instance) { SetInstance(instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Event
        */
        inline Event<Ret, Parameters...> &operator[](UnityEngine::Object *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Add delegate to event.
            @param delegate Delegate to add
        */
        inline void Add(Delegate<Ret> *delegate) {
            if (_hasAdd) return _add.cast<void>()(delegate);
            BNM_LOG_ERR(DBG_BNM_MSG_Event_Add_Error, str().c_str());
        }

        /**
            @brief Operator for adding delegate to event.
            @param delegate Delegate to add
        */
        inline Event<Ret, Parameters...> &operator+=(Delegate<Ret> *delegate) { Add(delegate); return *this; }

        /**
            @brief Remove delegate from event.
            @param delegate Delegate to remove
        */
        inline void Remove(Delegate<Ret> *delegate) {
            if (_hasRemove) return _remove.cast<void>()(delegate);
            BNM_LOG_ERR(DBG_BNM_MSG_Event_Remove_Error, str().c_str());
        }

        /**
            @brief Operator for removing delegate from event.
            @param delegate Delegate to remove
        */
        inline Event<Ret, Parameters...> &operator-=(Delegate<Ret> *v) { Remove(v); return *this; }

        /**
            @brief Raise (call) event.
            @param parameters Parameters of event
            @return Value of Ret type
        */
        inline Ret Raise(Parameters ...parameters) const {
            if (_hasRaise) return _raise.cast<Ret>()(parameters...);
            BNM_LOG_ERR(DBG_BNM_MSG_Event_Raise_Error, str().c_str());
            return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>();
        }

        /**
            @brief Raise (call) event.
            @param parameters Parameters of event
            @return Value of Ret type
        */
        inline Ret operator()(Parameters ...parameters) const { return Raise(parameters...); }

        /**
            @brief Convert base event to typed event.
            @param other Base event
        */
        Event<Ret> &operator =(const EventBase &other) {
            _data = other._data;
            _add = other._add;
            _remove = other._remove;
            _raise = other._raise;
            _hasAdd = other._hasAdd;
            _hasRemove = other._hasRemove;
            _hasRaise = other._hasRaise;
            return *this;
        }
    };

#pragma pack(pop)

}
// NOLINTEND

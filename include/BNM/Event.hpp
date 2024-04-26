#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "EventBase.hpp"
#include "Utils.hpp"

namespace BNM {

#pragma pack(push, 1)

    template<typename Ret = void, typename ...Args>
    struct Event : EventBase {
        inline constexpr Event() = default;
        template<typename OtherType>
        Event(const Event<OtherType> &other) : EventBase(other) {}
        Event(const IL2CPP::EventInfo *info) : EventBase(info) {}
        Event(const EventBase &other) : EventBase(other) {}

        inline Event<Ret, Args...> &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline Event<Ret, Args...> &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline Event<Ret, Args...> &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        inline void Add(Delegate<Ret> *v) const {
            if (_hasAdd) return _add.cast<void>()(v);
            BNM_LOG_ERR(DBG_BNM_MSG_Event_Add_Error, str().c_str());
        }

        inline Event<Ret, Args...> &operator+=(Delegate<Ret> *v) { Add(v); return *this; }
        inline Event<Ret, Args...> &operator+=(Delegate<Ret> *v) const { Add(v); return *this; }


        inline void Remove(Delegate<Ret> *v) const {
            if (_hasRemove) return _remove.cast<void>()(v);
            BNM_LOG_ERR(DBG_BNM_MSG_Event_Remove_Error, str().c_str());
        }

        inline Event<Ret, Args...> &operator-=(Delegate<Ret> *v) { Remove(v); return *this; }
        inline Event<Ret, Args...> &operator-=(Delegate<Ret> *v) const { Remove(v); return *this; }

        inline Ret Raise(Args ...args) const {
            if (_hasRaise) return _raise.cast<Ret>()(args...);
            BNM_LOG_ERR(DBG_BNM_MSG_Event_Raise_Error, str().c_str());
            if constexpr (std::is_same_v<Ret, void>) return; else return {};
        }

        inline Ret operator()(Args ...args) { return Raise(args...); }
        inline Ret operator()(Args ...args) const { return Raise(args...); }

        // Проверить, живо ли свойство
        inline bool Initialized() const noexcept { return _hasAdd || _hasRemove || _hasRaise; }

        // Скопировать другое свойство, только для автоматического приведения типов
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

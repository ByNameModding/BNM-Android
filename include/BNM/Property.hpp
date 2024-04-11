#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "PropertyBase.hpp"
#include "Utils.hpp"

namespace BNM {

#pragma pack(push, 1)

    template<typename T = bool>
    struct Property : PropertyBase {
        inline constexpr Property() = default;
        template<typename OtherType>
        Property(const Property<OtherType> &other) : PropertyBase(other) {}
        Property(const IL2CPP::PropertyInfo *info) : PropertyBase(info) {}
        Property(const PropertyBase &other) : PropertyBase(other) {}

        // Быстрая установка объекта
        inline Property<T> &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline Property<T> &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline Property<T> &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Вызвать получатель
        inline T Get() const {
            if (_hasGetter) return _getter.cast<T>()();
            BNM_LOG_ERR(DBG_BNM_MSG_Property_Get_Error, str().c_str());
            if constexpr (std::is_same_v<T, void>) return; else return {};
        }
        inline operator T() { return Get(); }
        inline T operator()() { return Get(); }
        inline operator T() const { return Get(); }
        inline T operator()() const { return Get(); }

        // Вызвать установщик
        inline void Set(T v) const {
            if (_hasSetter) return _setter.cast<void>()(v);
            BNM_LOG_ERR(DBG_BNM_MSG_Property_Set_Error, str().c_str());
        }

        inline Property<T> &operator=(T val) { Set(std::move(val)); return *this; }
        inline Property<T> &operator=(T val) const { Set(std::move(val)); return *this; }

        // Проверить, живо ли свойство
        inline bool Initialized() const noexcept { return _hasGetter || _hasSetter; }

        // Скопировать другое свойство, только для автоматического приведения типов
        Property<T> &operator =(const PropertyBase &other) {
            _data = other._data;
            _getter = other._getter;
            _setter = other._setter;
            _hasGetter = other._hasGetter;
            _hasSetter = other._hasSetter;
            return *this;
        }
    };

#pragma pack(pop)

}

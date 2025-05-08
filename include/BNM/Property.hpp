#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "PropertyBase.hpp"
#include "Utils.hpp"

// NOLINTBEGIN
namespace BNM {

#pragma pack(push, 1)

    /**
        @brief Typed class for working with il2cpp properties.

        This class provides API for getting and setting properties.

        @tparam T Type of property
    */
    template<typename T = bool>
    struct Property : PropertyBase {
        /**
            @brief Create empty property.
        */
        inline constexpr Property() = default;

        /**
            @brief Copy property.
            @param other Other property
            @tparam OtherType Type of other property
        */
        template<typename OtherType>
        Property(const Property<OtherType> &other) : PropertyBase(other) {}

        /**
            @brief Create property from il2cpp property.
            @param info Il2cpp property
        */
        Property(const IL2CPP::PropertyInfo *info) : PropertyBase(info) {}

        /**
            @brief Convert base property to typed property.
            @param other Base property
        */
        Property(const PropertyBase &other) : PropertyBase(other) {}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Property
        */
        inline Property<T> &operator[](void *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Property
        */
        inline Property<T> &operator[](IL2CPP::Il2CppObject *instance) { SetInstance(instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Property
        */
        inline Property<T> &operator[](UnityEngine::Object *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Call getter.
            @return Property value if it's valid, otherwise default value.
        */
        inline T Get() const {
            if (_hasGetter) return _getter.cast<T>()();
            BNM_LOG_ERR(DBG_BNM_MSG_Property_Get_Error, str().c_str());
            return BNM::PRIVATE_INTERNAL::ReturnEmpty<T>();
        }

        /**
            @brief Operator to call getter.
        */
        inline operator T() const { return Get(); }

        /**
            @brief Operator to call getter.
        */
        inline T operator()() const { return Get(); }

        /**
            @brief Call setter.
            @param value New property value
        */
        inline void Set(T value) {
            if (_hasSetter) return _setter.cast<void>()(value);
            BNM_LOG_ERR(DBG_BNM_MSG_Property_Set_Error, str().c_str());
        }

        /**
            @brief Operator to call setter.
        */
        inline Property<T> &operator=(T value) { Set(std::move(value)); return *this; }

        /**
            @brief Convert base property to typed property.
            @param other Base property
        */
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
// NOLINTEND

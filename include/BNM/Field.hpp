#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "FieldBase.hpp"
#include "Utils.hpp"

// NOLINTBEGIN
namespace BNM {

#pragma pack(push, 1)

    /**
        @brief Typed class for working with il2cpp fields.

        This class provides API for getting and setting fields.

        @tparam T Type of field
    */
    template<typename T>
    struct Field : public FieldBase {
        /**
            @brief Create empty field.
        */
        constexpr Field() noexcept = default;

        /**
            @brief Copy field.
            @param other Other field
            @tparam OtherType Type of other field
        */
        template<typename OtherType>
        Field(const Field<OtherType> &other) : FieldBase(other) {}

        /**
            @brief Create field from il2cpp field.
            @param info Il2cpp field
        */
        Field(IL2CPP::FieldInfo *info) : FieldBase(info) {}

        /**
            @brief Convert base field to typed field.
            @param other Base field
        */
        Field(const FieldBase &other) : FieldBase(other) {}


        /**
            @brief Typed wrapper of GetFieldPointer
        */
        inline T *GetPointer() const {
            auto ptr = GetFieldPointer();
            BNM_LOG_ERR_IF(ptr == nullptr, DBG_BNM_MSG_Field_GetPointer_Error, _data ? str().c_str() : DBG_BNM_MSG_Field_GetPointer_Dead);
            return (T *) ptr;
        }

        /**
            @brief Get field value.
            @return Field value if it's valid, otherwise default value.
        */
        T Get() const {
            BNM_LOG_ERR_IF(!_data, DBG_BNM_MSG_Field_GetSet_Error);
            if (!_data) return {};
            if (_isThreadStatic || _isConst) {
                T val{};
                PRIVATE_FieldUtils::GetStaticValue(_data, (void *)&val);
                return val;
            }
            if (auto ptr = GetPointer(); ptr != nullptr) return *ptr;
            return {};
        }

        /**
            @brief Operator to get field value.
        */
        inline operator T() const { return Get(); }

        /**
            @brief Operator to get field value.
        */
        inline T operator()() const { return Get(); }

        /**
            @brief Set field value.
            @param value New field value
        */
        void Set(T value) const {
            BNM_LOG_ERR_IF(!_data, DBG_BNM_MSG_Field_GetSet_Error);
            if (!_data) return;
            if (_isConst) {
                BNM_LOG_ERR(DBG_BNM_MSG_Field_Set_const_Error, str().c_str());
                return;
            }
            if (_isThreadStatic) {
                PRIVATE_FieldUtils::SetStaticValue(_data, (void *)&value);
                return;
            }
            if (auto ptr = GetPointer(); ptr != nullptr) *ptr = value;
        }

        /**
            @brief Operator to set field value.
        */
        inline Field<T> &operator=(T value) { Set(std::move(value)); return *this; }

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Field
        */
        inline Field<T> &operator[](void *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Field
        */
        inline Field<T> &operator[](IL2CPP::Il2CppObject *instance) { SetInstance(instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Field
        */
        inline Field<T> &operator[](UnityEngine::Object *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Convert base field to typed.
            @param other Base field
        */
        Field<T> &operator =(const FieldBase &other)  {
            _data = other._data;
            _instance = other._instance;
            _isStatic = other._isStatic;
            _isThreadStatic = other._isThreadStatic;
            _isInStruct = other._isInStruct;
            _isConst = other._isConst;
            return *this;
        }
    };

#pragma pack(pop)

}
// NOLINTEND

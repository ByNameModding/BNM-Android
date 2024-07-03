#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "FieldBase.hpp"
#include "Utils.hpp"

namespace BNM {

#pragma pack(push, 1)

    template<typename T>
    struct Field : public FieldBase {
        constexpr Field() noexcept = default;
        template<typename OtherType>
        Field(const Field<OtherType> &other) : FieldBase(other) {}
        Field(IL2CPP::FieldInfo *info) : FieldBase(info) {}
        Field(const FieldBase &other) : FieldBase(other) {}

        // Get pointer to field
        inline T *GetPointer() const {
            auto ptr = GetFieldPointer();
            BNM_LOG_ERR_IF(ptr == nullptr, DBG_BNM_MSG_Field_GetPointer_Error, _init ? str().c_str() : DBG_BNM_MSG_Field_GetPointer_Dead);
            return (T *)ptr;
        }

        // Get value from field
        T Get() const {
            BNM_LOG_ERR_IF(!_init, DBG_BNM_MSG_Field_GetSet_Error);
            if (!_init) return {};
            if (_isThreadStatic) {
                T val{};
                PRIVATE_FieldUtils::GetStaticValue(_data, (void *)&val);
                return val;
            }
            if (auto ptr = GetPointer(); ptr != nullptr) return *ptr;
            return {};
        }
        inline operator T() const { return Get(); }
        inline T operator()() const { return Get(); }

        // Set value to field
        void Set(T val) const {
            BNM_LOG_ERR_IF(!_init, DBG_BNM_MSG_Field_GetSet_Error);
            if (!_init) return;
            if (_isThreadStatic) {
                PRIVATE_FieldUtils::SetStaticValue(_data, (void *)&val);
                return;
            }
            if (auto ptr = GetPointer(); ptr != nullptr) *ptr = val;
        }
        inline Field<T> &operator=(T val) { Set(std::move(val)); return *this; }
        inline Field<T> &operator=(T val) const { Set(std::move(val)); return *this; }

        // Fast set instance
        inline Field<T> &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline Field<T> &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline Field<T> &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Copy other field, only for auto casts
        Field<T> &operator =(const FieldBase &other)  {
            _data = other._data;
            _instance = other._instance;
            _init = other._init;
            _isStatic = other._isStatic;
            _isThreadStatic = other._isThreadStatic;
            _isInStruct = other._isInStruct;
            return *this;
        }
    };

#pragma pack(pop)

}

#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Class.hpp"

namespace BNM {

#pragma pack(push, 1)
    template <typename T> struct Field;

    struct FieldBase {
        inline constexpr FieldBase() = default;
        inline FieldBase(const FieldBase &other) = default;
        FieldBase(IL2CPP::FieldInfo *info);

        // Set object
        FieldBase &SetInstance(IL2CPP::Il2CppObject *val);

        inline IL2CPP::FieldInfo *GetInfo() const { return _data; }
        inline BNM_PTR GetOffset() const { return _data ? (BNM_PTR) _data->offset - (_isInStruct && !_isStatic && !_isThreadStatic ? sizeof(IL2CPP::Il2CppObject) : 0x0) : 0; }

        // Get pointer to field
        void *GetFieldPointer() const;

#ifdef BNM_ALLOW_STR_METHODS
        // Get data
        inline std::string str() const {
            if (_init) return Class(_data->parent).str() + OBFUSCATE_BNM(".(") + _data->name + OBFUSCATE_BNM(")");
            return OBFUSCATE_BNM(DBG_BNM_MSG_FieldBase_str_nullptr);
        }
#endif

        // Fast set instance
        inline FieldBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline FieldBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline FieldBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Check is field alive
        inline bool Initialized() const noexcept { return _init; }

        // Cast field
        template<typename NewType> inline Field<NewType> &cast() const { return (Field<NewType> &)*this; }

        BNM::IL2CPP::FieldInfo *_data{};
        BNM::IL2CPP::Il2CppObject *_instance{};
        uint8_t _init : 1 = false, _isStatic : 1 = false, _isThreadStatic : 1 = false, _isInStruct : 1 = false;
    };

#pragma pack(pop)

}

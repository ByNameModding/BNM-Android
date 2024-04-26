#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Class.hpp"

#pragma pack(push, 1)

namespace BNM {
    struct Class;
    template <typename T> struct Field;

    struct FieldBase {
        inline constexpr FieldBase() = default;
        inline FieldBase(const FieldBase &other) = default;
        FieldBase(IL2CPP::FieldInfo *info);

        // Установить объект
        FieldBase &SetInstance(IL2CPP::Il2CppObject *val);

        inline IL2CPP::FieldInfo *GetInfo() const { return _data; }
        inline BNM_PTR GetOffset() const { return _data ? (BNM_PTR) _data->offset - (_isInStruct && !_isStatic && !_isThreadStatic ? sizeof(IL2CPP::Il2CppObject) : 0x0) : 0; }

        // Получить указатель на поле
        void *GetFieldPointer() const;

#ifdef BNM_ALLOW_STR_METHODS
        // Получить данные о поле
        inline std::string str() const {
            if (_init) return Class(_data->parent).str() + OBFUSCATE_BNM(".(") + _data->name + OBFUSCATE_BNM(")");
            return OBFUSCATE_BNM(DBG_BNM_MSG_FieldBase_str_nullptr);
        }
#endif

        // Быстрая установка объекта
        inline FieldBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline FieldBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline FieldBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Проверить, живо ли поле
        inline bool Initialized() const noexcept { return _init; }

        // Получить поле с указанным типом
        template<typename NewType> inline Field<NewType> &cast() const { return (Field<NewType> &)*this; }

        BNM::IL2CPP::FieldInfo *_data{};
        BNM::IL2CPP::Il2CppObject *_instance{};
        uint8_t _init : 1 = false, _isStatic : 1 = false, _isThreadStatic : 1 = false, _isInStruct : 1 = false;
    };
}

#pragma pack(pop)
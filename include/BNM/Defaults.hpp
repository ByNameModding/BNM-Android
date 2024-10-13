#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"

namespace BNM {
    namespace Structures::Unity {
        struct Vector2;
        struct Vector3;
        struct Vector4;
        struct Color;
        struct Color32;
        struct Ray;
        struct RaycastHit;
        struct Quaternion;
        struct Matrix3x3;
        struct Matrix4x4;
    }
    namespace Structures::Mono {
        struct String;
    }
    namespace UnityEngine {
        struct Object;
        struct MonoBehaviour;
    }
    struct CompileTimeClass;
    struct Class;
}

namespace BNM::Defaults {

    namespace Internal {
        typedef IL2CPP::Il2CppClass *ClassType;
        extern ClassType Void, Boolean, Byte, SByte, Int16, UInt16, Int32, UInt32, IntPtr, Int64, UInt64, Single, Double, String, Object;
        extern ClassType Vector2, Vector3, Vector4, Color, Color32, Ray, Quaternion, Matrix3x3, Matrix4x4, RaycastHit;
        extern ClassType UnityObject, MonoBehaviour;
    }

    struct DefaultTypeRef {
        Internal::ClassType *reference{};
        [[nodiscard]] constexpr inline bool Valid() const { return reference && *reference; }
        constexpr inline operator Internal::ClassType() const { return reference ? *reference : nullptr; }
        operator CompileTimeClass() const;
        operator BNM::Class() const;
        [[nodiscard]] BNM::Class ToClass() const;
    };

    template<typename T>
    constexpr DefaultTypeRef Get() {
        using namespace Structures::Unity;
        using namespace Structures::Mono;

        if constexpr (std::is_same_v<T, void>)
            return {&Internal::Void};
        else if constexpr (std::is_same_v<T, bool>)
            return {&Internal::Boolean};
        else if constexpr (std::is_same_v<T, uint8_t>)
            return {&Internal::Byte};
        else if constexpr (std::is_same_v<T, int8_t>)
            return {&Internal::SByte};
        else if constexpr (std::is_same_v<T, int16_t>)
            return {&Internal::Int16};
        else if constexpr (std::is_same_v<T, uint16_t>)
            return {&Internal::UInt16};
        else if constexpr (std::is_same_v<T, int32_t>)
            return {&Internal::Int32};
        else if constexpr (std::is_same_v<T, uint32_t>)
            return {&Internal::UInt32};
        else if constexpr (std::is_same_v<T, intptr_t>)
            return {&Internal::IntPtr};
        else if constexpr (std::is_same_v<T, int64_t>)
            return {&Internal::Int64};
        else if constexpr (std::is_same_v<T, uint64_t>)
            return {&Internal::UInt64};
        else if constexpr (std::is_same_v<T, float>)
            return {&Internal::Single};
        else if constexpr (std::is_same_v<T, double>)
            return {&Internal::Double};
        else if constexpr (std::is_same_v<T, BNM::IL2CPP::Il2CppString *> || std::is_same_v<T, Structures::Mono::String *>)
            return {&Internal::String};
        else if constexpr (std::is_same_v<T, Vector2>)
            return {&Internal::Vector2};
        else if constexpr (std::is_same_v<T, Vector3>)
            return {&Internal::Vector3};
        else if constexpr (std::is_same_v<T, Vector4>)
            return {&Internal::Vector4};
        else if constexpr (std::is_same_v<T, Color>)
            return {&Internal::Color};
        else if constexpr (std::is_same_v<T, Color32>)
            return {&Internal::Color32};
        else if constexpr (std::is_same_v<T, Ray>)
            return {&Internal::Ray};
        else if constexpr (std::is_same_v<T, Quaternion>)
            return {&Internal::Quaternion};
        else if constexpr (std::is_same_v<T, Matrix3x3>)
            return {&Internal::Matrix3x3};
        else if constexpr (std::is_same_v<T, Matrix4x4>)
            return {&Internal::Matrix4x4};
        else if constexpr (std::is_same_v<T, RaycastHit>)
            return {&Internal::RaycastHit};
        else if constexpr (std::is_same_v<T, BNM::UnityEngine::Object *>)
            return {&Internal::UnityObject};
        else if constexpr (std::is_same_v<T, BNM::UnityEngine::MonoBehaviour *>)
            return {&Internal::MonoBehaviour};
        else if constexpr (std::is_pointer_v<T>)
            return {&Internal::Object};
        return {};
    }
}
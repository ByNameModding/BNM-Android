#pragma once

#include <type_traits>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"

namespace BNM {
    /**
        @brief Namespace that holds Unity math and helper structs.

        It contains: Vector2, Vector3, Vector4, Color, Color32, Ray, RaycastHit, RaycastHit2D, Quaternion, Matrix3x3, Matrix4x4.
    */
    namespace Structures::Unity {
        struct Vector2;
        struct Vector3;
        struct Vector4;
        struct Color;
        struct Color32;
        struct Ray;
        struct RaycastHit;
        struct RaycastHit2D;
        struct Quaternion;
        struct Matrix3x3;
        struct Matrix4x4;
    }
    namespace Structures::Mono {
        struct String;
        struct decimal;
    }
    namespace UnityEngine {
        struct Object;
        struct MonoBehaviour;
    }
    struct CompileTimeClass;
    struct Class;
}

/**
    @brief Namespace that holds C# primitives.

    It contains: byte, sbyte, ushort, uint, ulong, nint, nuint, decimal.
*/
namespace BNM::Types {
    typedef uint8_t byte;
    typedef int8_t sbyte;
    typedef unsigned short ushort;
    typedef unsigned int uint;
    typedef unsigned long ulong;
    enum nint : intptr_t {};
    enum nuint : uintptr_t {};
    typedef BNM::Structures::Mono::decimal decimal;
}

// NOLINTBEGIN
/**
    @brief Namespace that helps to get references to primitives and common used C# and Unity types.
*/
namespace BNM::Defaults {
    /// @cond
    namespace Internal {
        typedef IL2CPP::Il2CppClass *ClassType;
        extern ClassType Void, Boolean, Byte, SByte, Int16, UInt16, Int32, UInt32, IntPtr, UIntPtr, Int64, UInt64, Single, Double, Decimal, String, Object;
        extern ClassType Vector2, Vector3, Vector4, Color, Color32, Ray, Quaternion, Matrix3x3, Matrix4x4, RaycastHit, RaycastHit2D;
        extern ClassType UnityObject, MonoBehaviour;
    }
    /// @endcond

    /**
        @brief Struct that holds reference to Il2CppClass.
    */
    struct DefaultTypeRef {
        Internal::ClassType *_reference{};
        [[nodiscard]] constexpr inline bool IsValid() const { return _reference && *_reference; }
        constexpr inline operator Internal::ClassType() const { return _reference ? *_reference : nullptr; }
        operator CompileTimeClass() const;
        operator BNM::Class() const;
        [[nodiscard]] BNM::Class ToClass() const;
    };

    /**
        @brief Method that helps to get il2cpp class type from C++ and BNM types.
        @tparam T Class type

        <h3>Supported types list:</h3>
        \code
        C++, C# keyword, .NET type

        void, void, System.Void
        bool, bool, System.Boolean

        BNM::Types::byte (uint8_t), byte, System.Byte
        BNM::Types::sbyte (int8_t), sbyte, System.SByte

        short (int16_t), short, System.Int16
        BNM::Types::ushort (uint16_t), ushort, System.UInt16

        int (int32_t), int, System.Int32
        BNM::Types::uint (unsigned int), uint, System.UInt32

        BNM::Types::nint (intptr_t), nint, System.IntPtr
        BNM::Types::nuint (uintptr_t), nuint, System.UIntPtr

        long, long, System.Int64
        BNM::Types::ulong (unsigned long), ulong, System.UInt64

        float, float, System.Single
        double, double, System.Double
        BNM::Types::decimal, decimal, System.Double

        BNM::IL2CPP::Il2CppString * (or) BNM::Structures::Mono::String *, string, System.String
        BNM::Structures::Unity::Vector2, Vector2, UnityEngine.Vector2
        BNM::Structures::Unity::Vector3, Vector3, UnityEngine.Vector3
        BNM::Structures::Unity::Vector4, Vector4, UnityEngine.Vector4
        BNM::Structures::Unity::Color, Color, UnityEngine.Color
        BNM::Structures::Unity::Color32, Color32, UnityEngine.Color32
        BNM::Structures::Unity::Ray, Ray, UnityEngine.Ray
        BNM::Structures::Unity::Quaternion, Quaternion, UnityEngine.Quaternion
        BNM::Structures::Unity::Matrix3x3, Matrix3x3, UnityEngine.Matrix3x3
        BNM::Structures::Unity::Matrix4x4, Matrix4x4, UnityEngine.Matrix4x4
        BNM::Structures::Unity::RaycastHit, RaycastHit, UnityEngine.RaycastHit
        BNM::Structures::Unity::RaycastHit2D, RaycastHit2D, UnityEngine.RaycastHit2D
        BNM::UnityEngine::Object *, Object, UnityEngine.Object
        BNM::UnityEngine::MonoBehaviour *, MonoBehaviour, UnityEngine.MonoBehaviour

        Any other pointer, object, System.Object
        \endcode

        @return Type reference if type is listed, otherwise empty reference.
    */
    template<typename T>
    constexpr DefaultTypeRef Get() {
        using namespace Structures::Unity;
        using namespace Structures::Mono;
        using namespace BNM::Types;

        if constexpr (std::is_same_v<T, void>)
            return {&Internal::Void};
        else if constexpr (std::is_same_v<T, bool>)
            return {&Internal::Boolean};
        else if constexpr (std::is_same_v<T, byte>)
            return {&Internal::Byte};
        else if constexpr (std::is_same_v<T, sbyte>)
            return {&Internal::SByte};
        else if constexpr (std::is_same_v<T, short>)
            return {&Internal::Int16};
        else if constexpr (std::is_same_v<T, ushort>)
            return {&Internal::UInt16};
        else if constexpr (std::is_same_v<T, int>)
            return {&Internal::Int32};
        else if constexpr (std::is_same_v<T, uint>)
            return {&Internal::UInt32};
        else if constexpr (std::is_same_v<T, nint>)
            return {&Internal::IntPtr};
        else if constexpr (std::is_same_v<T, nuint>)
            return {&Internal::UIntPtr};
        else if constexpr (std::is_same_v<T, long>)
            return {&Internal::Int64};
        else if constexpr (std::is_same_v<T, unsigned long>)
            return {&Internal::UInt64};
        else if constexpr (std::is_same_v<T, float>)
            return {&Internal::Single};
        else if constexpr (std::is_same_v<T, double>)
            return {&Internal::Double};
        else if constexpr (std::is_same_v<T, decimal>)
            return {&Internal::Decimal};
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
        else if constexpr (std::is_same_v<T, RaycastHit2D>)
            return {&Internal::RaycastHit2D};
        else if constexpr (std::is_same_v<T, BNM::UnityEngine::Object *>)
            return {&Internal::UnityObject};
        else if constexpr (std::is_same_v<T, BNM::UnityEngine::MonoBehaviour *>)
            return {&Internal::MonoBehaviour};
        else if constexpr (std::is_pointer_v<T>)
            return {&Internal::Object};
        return {};
    }

    /**
        @brief Simple method for boxing values.
        @param value Value that needs to be packed
        @tparam T Value type

        Method is based on BNM::Defaults::Get so only types supported by it will be boxed. Others will result null.

        @return Boxed value if type is supported, otherwise null.
    */
    template<typename T>
    inline IL2CPP::Il2CppObject *Box(T value) {
        if constexpr (std::is_pointer_v<T>)
            return (IL2CPP::Il2CppObject *) value;
        return BNM::Defaults::Get<T>().ToClass().BoxObject(value);
    }
}
// NOLINTEND
#pragma once
#include "Vector3.h"
namespace BNM::Structures::Unity {
    template<typename A, typename B>
    union UnionTuple {
        A first;
        B second;
    };
    template<typename B, typename A>
    inline UnionTuple<A, B>& AliasAs(A& a) { return reinterpret_cast<UnionTuple<A, B>&>(a); }

    template<typename B, typename A>
    inline UnionTuple<A, B> const& AliasAs(A const& a) { return reinterpret_cast<UnionTuple<A, B> const&>(a); }
    inline bool IsFinite(const float& value) {
        uint32_t intval = AliasAs<uint32_t>(value).second;
        return (intval & 0x7f800000) != 0x7f800000;
    }
    struct Vector4 {
        union {
            struct { float x, y, z, w; };
            float data[4];
        };
        Vector4() {}
        Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
        Vector4(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}
        explicit Vector4(const Vector3& v, float inW) : x(v.x), y(v.y), z(v.z), w(inW) {}
        explicit Vector4(const float* v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}

        void Set(float inX, float inY, float inZ, float inW) { x = inX; y = inY; z = inZ; w = inW; }
        void Set(const float* array) { x = array[0]; y = array[1]; z = array[2]; w = array[3]; }
        void SetZero() { x = 0.0f; y = 0.0f; z = 0.0f; w = 0.0f; }


        bool operator==(const Vector4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
        bool operator!=(const Vector4& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }
        bool operator==(const float v[4]) const { return x == v[0] && y == v[1] && z == v[2] && w == v[3]; }
        bool operator!=(const float v[4]) const { return x != v[0] || y != v[1] || z != v[2] || w != v[3]; }

        Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }


        static const float infinity;
        static const Vector4 infinityVec;
        static const Vector4 zero;
        static const Vector4 one;

        inline static Vector4 Project(const Vector4& a, const Vector4& b);
        inline static Vector4 Lerp(const Vector4& from, const Vector4& to, float t);
        inline static bool CompareApproximately(const Vector4& inV0, const Vector4& inV1, const float inMaxDist = Vector3::kEpsilon);
        inline static bool IsFinite(const Vector4& f);
        inline static float Magnitude(const Vector4& inV);
        inline static float SqrMagnitude(const Vector4& inV);
        inline static float Dot(const Vector4& lhs, const Vector4& rhs);
        inline static Vector4 Normalize(const Vector4& a);
        inline operator Vector3();
    };
    inline Vector4::operator Vector3() { return {x, y, z}; }
    inline Vector4 operator*(const Vector4& lhs, const Vector4& rhs) { return Vector4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
    inline Vector4 operator*(const Vector4& inV, const float s) { return Vector4(inV.x * s, inV.y * s, inV.z * s, inV.w * s); }
    inline Vector4 operator+(const Vector4& lhs, const Vector4& rhs) { return Vector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
    inline Vector4 operator-(const Vector4& lhs, const Vector4& rhs) { return Vector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
    inline Vector4 operator/(const Vector4& inV, const float s) { return {inV.x / s, inV.y / s, inV.z / s, inV.w / s}; }
    inline Vector4 operator/(const Vector4& lhs, const Vector4& rhs) { return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w}; }
    inline float Vector4::Dot(const Vector4& lhs, const Vector4& rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w; }

    inline float Vector4::SqrMagnitude(const Vector4& inV) { return Dot(inV, inV); }
    inline float Vector4::Magnitude(const Vector4& inV) { return sqrt(Dot(inV, inV)); }

    inline bool Vector4::IsFinite(const Vector4& f) { return Structures::Unity::IsFinite(f.x) & Structures::Unity::IsFinite(f.y) & Structures::Unity::IsFinite(f.z) && Structures::Unity::IsFinite(f.w); }

    inline bool Vector4::CompareApproximately(const Vector4& inV0, const Vector4& inV1, const float inMaxDist) { return SqrMagnitude(inV1 - inV0) <= inMaxDist * inMaxDist; }

    inline Vector4 Vector4::Lerp(const Vector4& from, const Vector4& to, float t) { return to * t + from * (1.0f - t); }
    inline Vector4 Vector4::Project(const Vector4& a, const Vector4& b) { return b * (Dot(a, b) / Dot(b, b)); }
    inline Vector4 Vector4::Normalize(const Vector4& a) {
        float num = Magnitude(a);
        if (num > 1E-05f) return a / num;
        return zero;
    }
}
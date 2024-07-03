#pragma once

#include "Vector3.hpp"

namespace BNM::Structures::Unity {
    struct Vector4;

    inline bool IsFinite(float value) { return (*(uint32_t *) &value & 0x7f800000) != 0x7f800000; }

    struct Vector4 {
        union {
            struct { float x, y, z, w; };
            float data[4]{0.f, 0.f, 0.f, 0.f};
        };
        inline constexpr Vector4() = default;
        inline constexpr Vector4(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}
        inline constexpr Vector4(Vector3 v, float inW) : x(v.x), y(v.y), z(v.z), w(inW) {}
        inline Vector4(Color c);

        inline float* GetPtr() { return data; }
        [[nodiscard]] inline const float* GetPtr() const { return data; }

        inline float& operator[](int i) { return data[i]; }
        inline const float& operator[](int i) const { return data[i]; }

        inline static float Component(Vector4 a, Vector4 b) { return Dot(a, b) / Magnitude(b); }
        inline static float Distance(Vector4 a, Vector4 b) { return Magnitude(a - b); }
        inline static float Dot(Vector4 lhs, Vector4 rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w; }
        inline static bool IsFinite(Vector4 f) { return Unity::IsFinite(f.x) & Unity::IsFinite(f.y) & Unity::IsFinite(f.z) && Unity::IsFinite(f.w); }
        inline static Vector4 Lerp(Vector4 from, Vector4 to, float t) { return to * t + from * (1.0f - t); }
        inline static Vector4 LerpUnclamped(Vector4 from, Vector4 to, float t) { return (to - from) * t + from; }
        inline static float Magnitude(Vector4 inV) { return sqrt(Dot(inV, inV)); }
        inline void Normalize() { *this = Normalize(*this); }
        inline static Vector4 Normalize(Vector4 v) { float n = Magnitude(v); if (n > 1E-05f) return v / n; return zero; }
        inline static Vector4 Project(Vector4 a, Vector4 b) { return b * (Dot(a, b) / Dot(b, b)); }
        inline static bool CompareApproximately(Vector4 a, Vector4 b, float inMaxDist = Vector3::kEpsilon) { return SqrMagnitude(b - a) <= inMaxDist * inMaxDist; }
        inline static float SqrMagnitude(Vector4 inV) { return Dot(inV, inV); }

        bool operator==(Vector4 v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
        bool operator!=(Vector4 v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }
        inline friend Vector4 operator*(Vector4 lhs, Vector4 rhs) { return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w}; }
        inline friend Vector4 operator*(Vector4 v, float s) { return {v.x * s, v.y * s, v.z * s, v.w * s}; }
        inline friend Vector4 operator+(Vector4 lhs, Vector4 rhs) { return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w}; }
        inline friend Vector4 operator-(Vector4 lhs, Vector4 rhs) { return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w}; }
        inline friend Vector4 operator/(Vector4 v, float s) { return {v.x / s, v.y / s, v.z / s, v.w / s}; }
        inline friend Vector4 operator/(Vector4 lhs, Vector4 rhs) { return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w}; }
        Vector4 operator-() const { return {-x, -y, -z, -w}; }
        inline operator Vector3() const { return {x, y, z}; }

        static const Vector4 positiveInfinity;
        static const Vector4 negativeInfinity;
        static const Vector4 zero;
        static const Vector4 one;
    };
}
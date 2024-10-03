#pragma once

#include <vector>

#include "Vector2.hpp"

namespace BNM::Structures::Unity {
    struct Quaternion;

    struct Vector3 {
        union {
            struct { float x, y, z; };
            float data[3]{0.f, 0.f, 0.f};
        };
        inline constexpr Vector3() = default;;
        inline constexpr Vector3(float x, float y, float z) : x(x), y(y), z(z) {};

        inline operator Vector2&() { return *(Vector2 *) this; }
        inline operator Vector2() const { return {x, y}; }

        inline float* GetPtr() { return data; }
        [[nodiscard]] inline const float* GetPtr() const { return data; }

        inline float& operator[](int i) { return data[i]; }
        inline const float& operator[](int i) const { return data[i]; }

        inline static float Angle(Vector3 from, Vector3 to);
        inline static float AngleBetween(Vector3 from, Vector3 to);
        inline static Vector3 ClampMagnitude(Vector3 vector, float maxLength);
        inline static Vector3 Cross(Vector3 lhs, Vector3 rhs) { return {lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x}; }
        inline static float Component(Vector3 a, Vector3 b) { return Dot(a, b) / Magnitude(b); }
        inline static float Distance(Vector3 a, Vector3 b) { return Magnitude(a - b); }
        inline static float Dot(Vector3 lhs, Vector3 rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; };
        inline static Vector3 FromSpherical(float rad, float theta, float phi);
        inline static Vector3 FromString(const std::string &str);
        inline static Vector3 Lerp(Vector3 from, Vector3 to, float t) { return to * t + from * (1.0f - t); }
        inline static Vector3 LerpUnclamped(Vector3 from, Vector3 to, float t) { return (to - from) * t + from; }
        inline static float Magnitude(Vector3 vector) { return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z); }
        inline static Vector3 Max(Vector3 lhs, Vector3 rhs);
        inline static Vector3 Min(Vector3 lhs, Vector3 rhs);
        inline static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta);
        inline void Normalize() { *this = Normalize(*this); }
        inline static float NormalizeAngle(float f, bool is180 = true);
        inline static Vector3 NormalizeEuler(Vector3 vector, bool is180 = true);
        inline static Vector3 Normalize(Vector3 value);
        inline static Vector3 Orthogonal(Vector3 vector);
        inline static void OrthoNormalize(Vector3 &normal, Vector3 &tangent);
        inline static void OrthoNormalize(Vector3 &normal, Vector3 &tangent, Vector3 &binormal);
        inline static void OrthoNormalizeFast(Vector3 &normal, Vector3 &tangent, Vector3 &binormal);
        inline static Vector3 OrthoNormalVectorFast(const Vector3& n);
        inline static Vector3 Project(Vector3 a, Vector3 b);
        inline static Vector3 ProjectOnPlane(Vector3 vector, Vector3 planeNormal);
        inline static Vector3 Reflect(Vector3 inDirection, Vector3 inNormal);
        inline static Vector3 RotateTowards(Vector3 current, Vector3 target, float maxRadiansDelta, float maxMagnitudeDelta);
        inline void Scale(Vector3 scale) { *this = *this * scale; }
        inline static Vector3 Scale(Vector3 a, Vector3 b) { return a * b; }
        inline static float SignedAngle(Vector3 from, Vector3 to, Vector3 axis);
        inline static Vector3 Slerp(Vector3 a, Vector3 b, float t);
        inline static Vector3 SlerpUnclamped(Vector3 a, Vector3 b, float t);
        inline static Vector3 SmoothDamp(Vector3 current, Vector3 target, Vector3 &currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
        inline static float SqrMagnitude(Vector3 vector) { return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z; }
        inline static void ToSpherical(Vector3 vector, float &rad, float &theta, float &phi);

        inline std::string str() { return std::to_string(x) + std::string(BNM_OBFUSCATE(", ")) + std::to_string(y) + std::string(BNM_OBFUSCATE(", ")) + std::to_string(z); }
        inline Vector3 orthogonal() const { return Orthogonal(*this); }
        inline float magnitude() const { return Magnitude(*this); }
        inline Vector3 normalized() const { return Normalize(*this); }
        inline float sqrMagnitude() const { return SqrMagnitude(*this); }
        inline Vector3 normalizedEuler(bool is180 = true) const { return NormalizeEuler(*this, is180); }

        inline bool operator==(Vector3 v) const { return x == v.x && y == v.y && z == v.z; }
        inline bool operator!=(Vector3 v) const { return x != v.x || y != v.y || z != v.z; }
        inline Vector3& operator+=(Vector3 inV) { x += inV.x; y += inV.y; z += inV.z; return *this; }
        inline Vector3& operator-=(Vector3 inV) { x -= inV.x; y -= inV.y; z -= inV.z; return *this; }
        inline Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
        inline Vector3& operator/=(float s)  { x /= s; y /= s; z /= s; return *this; }
        inline Vector3& operator/=(Vector3 inV) { x /= inV.x; y /= inV.y; z /= inV.z; return *this; }
        inline friend Vector3 operator+(Vector3 lhs, Vector3 rhs) { return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z}; }
        inline friend Vector3 operator-(Vector3 lhs, Vector3 rhs) { return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z}; }
        inline friend Vector3 operator*(Vector3 v, float s) { return {v.x * s, v.y * s, v.z * s}; }
        inline friend Vector3 operator*(float s, Vector3 v) { return {v.x * s, v.y * s, v.z * s}; }
        inline friend Vector3 operator*(Vector3 lhs, Vector3 rhs) { return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z}; }
        inline friend Vector3 operator/(Vector3 v, float s) { Vector3 temp(v); temp /= s; return temp; }
        inline friend Vector3 operator/(Vector3 lhs, Vector3 rhs) { return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z}; }
        inline Vector3 operator-() const { return {-x, -y, -z}; }

        inline static constexpr const float kEpsilon = 1E-05f;
        inline static constexpr const float kEpsilonNormalSqrt = 1E-15f;
        static const Vector3 positiveInfinity;
        static const Vector3 negativeInfinity;
        static const Vector3 back;
        static const Vector3 down;
        static const Vector3 forward;
        static const Vector3 left;
        static const Vector3 one;
        static const Vector3 right;
        static const Vector3 up;
        static const Vector3 zero;
    };


    float Vector3::Angle(Vector3 from, Vector3 to) {
        float v = Dot(from, to) / (Magnitude(from) * Magnitude(to));
        v = fmaxf(v, -1.f);
        v = fminf(v, 1.f);
        return acosf(v);
    }

    float Vector3::AngleBetween(Vector3 from, Vector3 to) {
        float v = Dot(Normalize(from), Normalize(to));
        v = fmaxf(v, -1.f);
        v = fminf(v, 1.f);
        return acosf(v);
    }

    Vector3 Vector3::ClampMagnitude(Vector3 vector, float maxLength) {
        float length = Magnitude(vector);
        if (length > maxLength) vector *= maxLength / length;
        return vector;
    }

    Vector3 Vector3::FromSpherical(float rad, float theta, float phi) {
        Vector3 v{};
        v.x = rad * sinf(theta) * cosf(phi);
        v.y = rad * sinf(theta) * sinf(phi);
        v.z = rad * cosf(theta);
        return v;
    }

    Vector3 Vector3::FromString(const std::string& str) {
        std::vector<std::string> args{};
        std::string buffer{};
        for (char i : str) {
            bool no = true;
            if (i == 'f') no = false;
            if (i != ',') buffer += i;
            else if (no) {
                args.push_back(buffer);
                buffer.clear();
            }
        }
        if (!buffer.empty()) args.push_back(buffer);
        if (args.size() != 3) return {};
        return {std::stof(args[0]), std::stof(args[1]), std::stof(args[2])};
    }

    Vector3 Vector3::Max(Vector3 a, Vector3 b) {
        float x = a.x > b.x ? a.x : b.x;
        float y = a.y > b.y ? a.y : b.y;
        float z = a.z > b.z ? a.z : b.z;
        return {x, y, z};
    }

    Vector3 Vector3::Min(Vector3 a, Vector3 b) {
        float x = a.x > b.x ? b.x : a.x;
        float y = a.y > b.y ? b.y : a.y;
        float z = a.z > b.z ? b.z : a.z;
        return {x, y, z};
    }

    Vector3 Vector3::MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta) {
        Vector3 d = target - current;
        float m = Magnitude(d);
        if (m < maxDistanceDelta || m == 0)
            return target;
        return current + (d * maxDistanceDelta / m);
    }

    Vector3 Vector3::Normalize(Vector3 v) {
        float mag = Magnitude(v);
        if (mag == 0) return Vector3::zero;
        return v / mag;
    }

    float Vector3::NormalizeAngle(float f, bool is180) {
        while (f > (is180 ? 180.f : 360.f)) f -= 360.f;
        while (f < (is180 ? -180.f : 0.f)) f += 360.f;
        return f;
    }

    Vector3 Vector3::NormalizeEuler(Vector3 vec, bool is180) {
        vec.x = NormalizeAngle(vec.x, is180);
        vec.y = NormalizeAngle(vec.y, is180);
        vec.z = NormalizeAngle(vec.z, is180);
        return vec;
    }

    Vector3 Vector3::Orthogonal(Vector3 vector) {
        return vector.z < vector.x ? Vector3(vector.y, -vector.x, 0) : Vector3(0, -vector.z, vector.y);
    }

    void Vector3::OrthoNormalize(Vector3 &inU, Vector3 &inV) {
        float mag = Magnitude(inU);
        if (mag > 0.00001) inU /= mag;
        else inU = {1, 0, 0};
        float dot0 = Dot(inU, inV);
        inV -= dot0 * inU;
        mag = Magnitude(inV);
        if (mag < 0.00001) inV = OrthoNormalVectorFast(inU);
        else inV /= mag;
    }

    void Vector3::OrthoNormalize(Vector3 &inU, Vector3 &inV, Vector3 &inW) {
        float mag = Magnitude(inU);
        if (mag > kEpsilon) inU /= mag;
        else inU = {1, 0, 0};
        float dot0 = Dot(inU, inV);
        inV -= dot0 * inU;
        mag = Magnitude(inV);
        if (mag > kEpsilon) inV /= mag;
        else inV = OrthoNormalVectorFast(inU);
        float dot1 = Dot(inV, inW);
        dot0 = Dot(inU, inW);
        inW -= dot0 * inU + dot1 * inV;
        mag = Magnitude(inW);
        if (mag > kEpsilon) inW /= mag;
        else inW = Cross(inU, inV);
    }

    void Vector3::OrthoNormalizeFast(Vector3 &inU, Vector3 &inV, Vector3 &inW) {
        inU = Normalize(inU);
        float dot0 = Dot(inU, inV);
        inV -= dot0 * inU;
        inV = Normalize(inV);
        float dot1 = Dot(inV, inW);
        dot0 = Dot(inU, inW);
        inW -= dot0 * inU + dot1 * inV;
        inW = Normalize(inW);
    }

    Vector3 Vector3::OrthoNormalVectorFast(const Vector3& n) {
        Vector3 res{};
        constexpr float k1OverSqrt2 = 0.7071067811865475244008443621048490f;
        if (abs(n.z) > k1OverSqrt2) {
            float a = n.y * n.y + n.z * n.z;
            float k = 1 / sqrt(a);
            res.x = 0;
            res.y = -n.z * k;
            res.z = n.y * k;
        } else {
            float a = n.x * n.x + n.y * n.y;
            float k = 1 / sqrt(a);
            res.x = -n.y * k;
            res.y = n.x * k;
            res.z = 0;
        }
        return res;
    }

    Vector3 Vector3::Project(Vector3 vector, Vector3 onNormal) {
        float sqrMag = Dot(onNormal, onNormal);
        if (sqrMag < kEpsilon) return Vector3::zero;
        float dot = Dot(vector, onNormal);
        return onNormal * dot / sqrMag;
    }

    Vector3 Vector3::ProjectOnPlane(Vector3 vector, Vector3 planeNormal) {
        float sqrMag = Dot(planeNormal, planeNormal);
        if (sqrMag < kEpsilon) return vector;
        float dot = Dot(vector, planeNormal);
        return vector - planeNormal * dot / sqrMag;
    }

    Vector3 Vector3::Reflect(Vector3 inDirection, Vector3 inNormal) {
        return (-2.f * Dot(inNormal, inDirection)) * inNormal + inDirection;
    }

    Vector3 Vector3::RotateTowards(Vector3 current, Vector3 target, float maxRadiansDelta, float maxMagnitudeDelta) {
        float magCur = Magnitude(current);
        float magTar = Magnitude(target);
        float newMag = magCur + maxMagnitudeDelta * (float) ((magTar > magCur) - (magCur > magTar));
        newMag = fminf(newMag, fmaxf(magCur, magTar));
        newMag = fmaxf(newMag, fminf(magCur, magTar));
        float totalAngle = Angle(current, target) - maxRadiansDelta;
        if (totalAngle <= 0) return Normalize(target) * newMag;
        else if (totalAngle >= M_PI) return -Normalize(target) * newMag;
        Vector3 axis = Cross(current, target);
        float magAxis = Magnitude(axis);
        if (magAxis == 0) axis = Normalize(Cross(current, current + Vector3(3.95, 5.32, -4.24)));
        else axis /= magAxis;
        current = Normalize(current);
        Vector3 newVector = current * cosf(maxRadiansDelta) + Cross(axis, current) * sinf(maxRadiansDelta);
        return newVector * newMag;
    }

    float Vector3::SignedAngle(Vector3 from, Vector3 to, Vector3 axis) {
        float unsignedAngle = Angle(from, to);
        float cross_x = from.y * to.z - from.z * to.y;
        float cross_y = from.z * to.x - from.x * to.z;
        float cross_z = from.x * to.y - from.y * to.x;
        float sign = (axis.x * cross_x + axis.y * cross_y + axis.z * cross_z) >= 0.f ? 1.f : -1.f;
        return unsignedAngle * sign;
    }

    Vector3 Vector3::Slerp(Vector3 a, Vector3 b, float t) {
        if (t < 0) return a;
        else if (t > 1) return b;
        return SlerpUnclamped(a, b, t);
    }

    Vector3 Vector3::SlerpUnclamped(Vector3 a, Vector3 b, float t) {
        float magA = Magnitude(a);
        float magB = Magnitude(b);
        a /= magA;
        b /= magB;
        float dot = Dot(a, b);
        dot = fmaxf(dot, -1.f);
        dot = fminf(dot, 1.f);
        float theta = acosf(dot) * t;
        Vector3 relativeVec = b - a * dot;
        Vector3 newVec = a * cosf(theta) + Normalize(relativeVec) * sinf(theta);
        return newVec * (magA + (magB - magA) * t);
    }

    Vector3 Vector3::SmoothDamp(Vector3 current, Vector3 target, Vector3 &currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
        smoothTime = std::max(0.0001f, smoothTime);
        float omega = 2.f / smoothTime;
        float x = omega * deltaTime;
        float exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
        Vector3 change = current - target;
        Vector3 originalTo = target;
        float maxChange = maxSpeed * smoothTime;
        float maxChangeSq = maxChange * maxChange;
        float sqrMag = SqrMagnitude(change);
        if (sqrMag > maxChangeSq) change /= sqrtf(sqrMag) * maxChange;
        target = current - change;
        Vector3 temp = (currentVelocity + omega * change) * deltaTime;
        currentVelocity = (currentVelocity - omega * temp) * exp;
        Vector3 output = target + (change + temp) * exp;
        Vector3 origMinusCurrent = originalTo - current;
        Vector3 outMinusOrig = output - originalTo;
        if (origMinusCurrent.x * outMinusOrig.x + origMinusCurrent.y * outMinusOrig.y + origMinusCurrent.z * outMinusOrig.z > 0.f) {
            currentVelocity = (output - originalTo) / deltaTime;
            return originalTo;
        }
        return output;
    }

    void Vector3::ToSpherical(Vector3 vector, float &rad, float &theta, float &phi) {
        rad = Magnitude(vector);
        float v = vector.z / rad;
        v = fmaxf(v, -1.f);
        v = fminf(v, 1.f);
        theta = acosf(v);
        phi = atan2f(vector.y, vector.x);
    }

    struct Vector3Int {
        union {
            struct { int x, y, z; };
            int data[3]{0, 0, 0};
        };
    };
}

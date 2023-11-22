#pragma once
#include "Vector2.h"
#define k1OverSqrt2 float(0.7071067811865475244008443621048490)
namespace BNM::Structures::Unity {
    struct Quaternion;
    struct Vector3 {
        union {
            struct { float x{}, y{}, z{}; };
            float data[3];
        };
        inline float* GetPtr()                                { return data; }
        inline const float* GetPtr() const                     { return data; }
        inline float& operator[](int i)                       { return data[i]; }
        inline const float& operator[](int i) const            { return data[i]; }
        inline constexpr Vector3() noexcept : x(0.f), y(0.f), z(0.f) {};
        inline constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {};
        inline operator Vector2&() { return *(Vector2 *)this; }

        inline static float Angle(Vector3 from, Vector3 to);
        inline static float AngleBetween(Vector3 from, Vector3 to);
        inline static float Dot(Vector3, Vector3);
        inline static Vector3 ClampMagnitude(Vector3, float);
        inline static float Component(Vector3, Vector3);
        inline static Vector3 Cross(Vector3, Vector3);
        inline static float Distance(Vector3, Vector3);
        inline static float Dist(Vector3, Vector3);
        inline static Vector3 FromSpherical(float, float, float);
        inline static Vector3 Lerp(Vector3, Vector3, float);
        inline static Vector3 LerpUnclamped(Vector3, Vector3, float);
        inline static Vector3 Max(Vector3, Vector3);
        inline static Vector3 Min(Vector3, Vector3);
        inline static Vector3 MoveTowards(Vector3, Vector3, float);
        inline static Vector3 Orthogonal(Vector3);
        inline static Vector3 RotateTowards(Vector3, Vector3, float, float);
        inline static Vector3 Slerp(Vector3, Vector3, float);
        inline static Vector3 SlerpUnclamped(Vector3, Vector3, float);
        inline static void ToSpherical(Vector3 vector, float &, float &, float &);
        inline static float Magnitude(Vector3);
        inline static float SqrMagnitude(Vector3);
        inline static Vector3 SmoothDamp(Vector3 current, Vector3 target, Vector3 &currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
        inline static Vector3 Normalize(Vector3);
        inline static Vector3 NormalizeEuler(Vector3, bool is180 = true);
        inline static float NormalizeAngle(float f, bool is180 = true);
        inline static Vector3 FromString(std::string);

        inline Vector3 orthogonal() { return Orthogonal(*this); }
        inline float magnitude() { return Magnitude(*this); }
        inline Vector3 normalized() { return Normalize(*this); }
        inline float sqrMagnitude() { return SqrMagnitude(*this); }
        inline std::string str() { return std::to_string(x) + OBFUSCATES_BNM(", ") + std::to_string(y) + OBFUSCATES_BNM(", ") + std::to_string(z); }

        inline Vector3 normalizedEuler(bool is180 = true) { return NormalizeEuler(*this, is180); }

        bool operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z; }
        bool operator!=(const Vector3& v) const { return x != v.x || y != v.y || z != v.z; }
        Vector3& operator+=(const Vector3& inV) { x += inV.x; y += inV.y; z += inV.z; return *this; }
        Vector3& operator-=(const Vector3& inV) { x -= inV.x; y -= inV.y; z -= inV.z; return *this; }
        Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
        Vector3& operator/=(float s);
        Vector3& operator/=(const Vector3& inV) { x /= inV.x; y /= inV.y; z /= inV.z; return *this; }
        Vector3 operator-() const { return Vector3(-x, -y, -z); }
        static const Vector3 back;
        static const Vector3 down;
        static const Vector3 forward;
        static const float kEpsilon;
        static const float kEpsilonNormalSqrt;
        static const Vector3 left;
        static const Vector3 negativeInfinity;
        static const Vector3 one;
        static const Vector3 positiveInfinity;
        static const Vector3 right;
        static const Vector3 up;
        static const Vector3 zero;

        inline static void OrthoNormalize(Vector3 &inU, Vector3 &inV);
        inline static void OrthoNormalize(Vector3 &inU, Vector3 &inV, Vector3 &inW);
        inline static void OrthoNormalizeFast(Vector3 &inU, Vector3 &inV, Vector3 &inW);
        inline static Vector3 OrthoNormalVectorFast(const Vector3& n);
        inline operator Vector2() { return {x, y}; }
    };


    inline Vector3 operator+(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); }
    inline Vector3 operator-(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }
    inline Vector3 operator*(const Vector3& inV, const float s) { return Vector3(inV.x * s, inV.y * s, inV.z * s); }
    inline Vector3 operator*(const float s, const Vector3& inV) { return Vector3(inV.x * s, inV.y * s, inV.z * s); }
    inline Vector3 operator*(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); }
    inline Vector3 operator/(const Vector3& inV, const float s) { Vector3 temp(inV); temp /= s; return temp; }
    inline Vector3 operator/(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); }
    inline Vector3& Vector3::operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

    float Vector3::Angle(Vector3 from, Vector3 to) {
        float v = Dot(from, to) / (from.magnitude() * to.magnitude());
        v = fmax(v, -1.0);
        v = fmin(v, 1.0);
        return acos(v);
    }
    float Vector3::AngleBetween(Vector3 from, Vector3 to) {
        float v = Dot(from.normalized(), to.normalized());
        v = fmax(v, -1.0);
        v = fmin(v, 1.0);
        return acos(v);
    }
    Vector3 Vector3::ClampMagnitude(Vector3 vector, float maxLength) {
        float length = vector.magnitude();
        if (length > maxLength) vector *= maxLength / length;
        return vector;
    }
    float Vector3::Component(Vector3 a, Vector3 b) {
        return Dot(a, b) / b.magnitude();
    }
    Vector3 Vector3::Cross(Vector3 lhs, Vector3 rhs) {
        float x = lhs.y * rhs.z - lhs.z * rhs.y;
        float y = lhs.z * rhs.x - lhs.x * rhs.z;
        float z = lhs.x * rhs.y - lhs.y * rhs.x;
        return Vector3(x, y, z);
    }
    float Vector3::Distance(Vector3 a, Vector3 b) {
        return (a - b).magnitude();
    }
    float Vector3::Dist(Vector3 a, Vector3 b) {
        return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y) + (b.z - a.z) * (b.z - a.z));
    }
    float Vector3::Dot(Vector3 lhs, Vector3 rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }
    Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float t) {
        if (t < 0) return a;
        else if (t > 1) return b;
        return LerpUnclamped(a, b, t);
    }
    Vector3 Vector3::LerpUnclamped(Vector3 a, Vector3 b, float t) {
        return (b - a) * t + a;
    }
    float Vector3::Magnitude(Vector3 v) {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }
    Vector3 Vector3::Max(Vector3 a, Vector3 b) {
        float x = a.x > b.x ? a.x : b.x;
        float y = a.y > b.y ? a.y : b.y;
        float z = a.z > b.z ? a.z : b.z;
        return Vector3(x, y, z);
    }
    Vector3 Vector3::Min(Vector3 a, Vector3 b) {
        float x = a.x > b.x ? b.x : a.x;
        float y = a.y > b.y ? b.y : a.y;
        float z = a.z > b.z ? b.z : a.z;
        return Vector3(x, y, z);
    }
    Vector3 Vector3::MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta) {
        Vector3 d = target - current;
        float m = d.magnitude();
        if (m < maxDistanceDelta || m == 0)
            return target;
        return current + (d * maxDistanceDelta / m);
    }
    Vector3 Vector3::Normalize(Vector3 v) {
        float mag = v.magnitude();
        if (mag == 0) return Vector3::zero;
        return v / mag;
    }
    float Vector3::NormalizeAngle(float f, bool is180) {
        while (f > (is180 ? 180 : 360)) f -= 360;
        while (f < (is180 ? -180 : 0.f)) f += 360;
        return f;
    }
    inline void Vector3::OrthoNormalize(Vector3 &inU, Vector3 &inV) {
        float mag = inU.magnitude();
        if (mag > 0.00001) inU /= mag;
        else inU = {1, 0, 0};
        float dot0 = Dot(inU, inV);
        inV -= dot0 * inU;
        mag = inV.magnitude();
        if (mag < 0.00001) inV = OrthoNormalVectorFast(inU);
        else inV /= mag;
    }
    inline void Vector3::OrthoNormalize(Vector3 &inU, Vector3 &inV, Vector3 &inW) {
        float mag = inU.magnitude();
        if (mag > Vector3::kEpsilon)
            inU /= mag;
        else
            inU = {1, 0, 0};
        float dot0 = Dot(inU, inV);
        inV -= dot0 * inU;
        mag = inV.magnitude();
        if (mag > Vector3::kEpsilon)
            inV /= mag;
        else
            inV = OrthoNormalVectorFast(inU);
        float dot1 = Dot(inV, inW);
        dot0 = Dot(inU, inW);
        inW -= dot0 * inU + dot1 * inV;
        mag = inW.magnitude();
        if (mag > Vector3::kEpsilon)
            inW /= mag;
        else
            inW = Cross(inU, inV);
    }
    inline void Vector3::OrthoNormalizeFast(Vector3 &inU, Vector3 &inV, Vector3 &inW) {
        inU = inU.normalized();
        float dot0 = Vector3::Dot(inU, inV);
        inV -= dot0 * inU;
        inV = inV.normalized();
        float dot1 = Vector3::Dot(inV, inW);
        dot0 = Vector3::Dot(inU, inW);
        inW -= dot0 * inU + dot1 * inV;
        inW = inW.normalized();
    }
    Vector3 Vector3::OrthoNormalVectorFast(const Vector3& n) {
        Vector3 res;
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
    Vector3 Vector3::RotateTowards(Vector3 current, Vector3 target, float maxRadiansDelta, float maxMagnitudeDelta) {
        float magCur = current.magnitude();
        float magTar = target.magnitude();
        float newMag = magCur + maxMagnitudeDelta * ((magTar > magCur) - (magCur > magTar));
        newMag = fmin(newMag, fmax(magCur, magTar));
        newMag = fmax(newMag, fmin(magCur, magTar));
        float totalAngle = Angle(current, target) - maxRadiansDelta;
        if (totalAngle <= 0) return Normalize(target) * newMag;
        else if (totalAngle >= M_PI) return -target.normalized() * newMag;
        Vector3 axis = Cross(current, target);
        float magAxis = axis.magnitude();
        if (magAxis == 0) axis = Cross(current, current + Vector3(3.95, 5.32, -4.24)).normalized();
        else axis /= magAxis;
        current = current.normalized();
        Vector3 newVector = current * cos(maxRadiansDelta) + Cross(axis, current) * sin(maxRadiansDelta);
        return newVector * newMag;
    }
    Vector3 Vector3::Slerp(Vector3 a, Vector3 b, float t) {
        if (t < 0) return a;
        else if (t > 1) return b;
        return SlerpUnclamped(a, b, t);
    }
    Vector3 Vector3::SlerpUnclamped(Vector3 a, Vector3 b, float t) {
        float magA = a.magnitude();
        float magB = b.magnitude();
        a /= magA;
        b /= magB;
        float dot = Dot(a, b);
        dot = fmax(dot, -1.0);
        dot = fmin(dot, 1.0);
        float theta = acos(dot) * t;
        Vector3 relativeVec = b - a * dot;
        Vector3 newVec = a * cos(theta) + relativeVec.normalized() * sin(theta);
        return newVec * (magA + (magB - magA) * t);
    }
    Vector3 Vector3::SmoothDamp(Vector3 current, Vector3 target, Vector3 &currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
        smoothTime = std::max(0.0001f, smoothTime);
        float omega = 2 / smoothTime;
        float x = omega * deltaTime;
        float exp = 1 / (1 + x + 0.48f * x * x + 0.235f * x * x * x);
        float change_x = current.x - target.x;
        float change_y = current.y - target.y;
        float change_z = current.z - target.z;
        Vector3 originalTo = target;
        float maxChange = maxSpeed * smoothTime;
        float maxChangeSq = maxChange * maxChange;
        float sqrmag = change_x * change_x + change_y * change_y + change_z * change_z;
        bool flag = sqrmag > maxChangeSq;
        if (flag) {
            float mag = sqrt(sqrmag);
            change_x = change_x / mag * maxChange;
            change_y = change_y / mag * maxChange;
            change_z = change_z / mag * maxChange;
        }
        target.x = current.x - change_x;
        target.y = current.y - change_y;
        target.z = current.z - change_z;
        float temp_x = (currentVelocity.x + omega * change_x) * deltaTime;
        float temp_y = (currentVelocity.y + omega * change_y) * deltaTime;
        float temp_z = (currentVelocity.z + omega * change_z) * deltaTime;
        currentVelocity.x = (currentVelocity.x - omega * temp_x) * exp;
        currentVelocity.y = (currentVelocity.y - omega * temp_y) * exp;
        currentVelocity.z = (currentVelocity.z - omega * temp_z) * exp;
        float output_x = target.x + (change_x + temp_x) * exp;
        float output_y = target.y + (change_y + temp_y) * exp;
        float output_z = target.z + (change_z + temp_z) * exp;
        float origMinusCurrent_x = originalTo.x - current.x;
        float origMinusCurrent_y = originalTo.y - current.y;
        float origMinusCurrent_z = originalTo.z - current.z;
        float outMinusOrig_x = output_x - originalTo.x;
        float outMinusOrig_y = output_y - originalTo.y;
        float outMinusOrig_z = output_z - originalTo.z;
        bool flag2 = origMinusCurrent_x * outMinusOrig_x + origMinusCurrent_y * outMinusOrig_y + origMinusCurrent_z * outMinusOrig_z > 0;
        if (flag2) {
            output_x = originalTo.x;
            output_y = originalTo.y;
            output_z = originalTo.z;
            currentVelocity.x = (output_x - originalTo.x) / deltaTime;
            currentVelocity.y = (output_y - originalTo.y) / deltaTime;
            currentVelocity.z = (output_z - originalTo.z) / deltaTime;
        }
        return Vector3(output_x, output_y, output_z);
    }
    float Vector3::SqrMagnitude(Vector3 v) {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }
    Vector3 Vector3::FromSpherical(float rad, float theta, float phi) {
        Vector3 v;
        v.x = rad * sin(theta) * cos(phi);
        v.y = rad * sin(theta) * sin(phi);
        v.z = rad * cos(theta);
        return v;
    }
    Vector3 Vector3::Orthogonal(Vector3 v) {
        return v.z < v.x ? Vector3(v.y, -v.x, 0) : Vector3(0, -v.z, v.y);
    }
    void Vector3::ToSpherical(Vector3 vector, float &rad, float &theta, float &phi) {
        rad = vector.magnitude();
        float v = vector.z / rad;
        v = fmax(v, -1.0);
        v = fmin(v, 1.0);
        theta = acos(v);
        phi = atan2(vector.y, vector.x);
    }
    Vector3 Vector3::NormalizeEuler(Vector3 vec, bool is180) {
        vec.x = NormalizeAngle(vec.x, is180);
        vec.y = NormalizeAngle(vec.y, is180);
        vec.z = NormalizeAngle(vec.z, is180);
        return vec;
    }
    Vector3 Vector3::FromString(std::string str) {
        std::vector<std::string> commands;
        std::string buffer = "";
        for (int i = 0; i < str.size(); i++) {
            bool no = true;
            if (str[i] == 'f') no = false;
            if (str[i] != ',') buffer += str[i];
            else if (no) {
                commands.push_back(buffer);
                buffer = "";
            }
        }
        if (!buffer.empty()) commands.push_back(buffer);
        return Vector3(std::atof(commands[0].c_str()), std::atof(commands[1].c_str()), std::atof(commands[2].c_str()));
    }
}
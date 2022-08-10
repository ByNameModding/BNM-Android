#pragma once
#define SMALL_FLOAT 0.0000000001
#define PI 3.14159265358979323846264338327950288419716939937510f
#define Deg2Rad (2.f * PI / 360.f)
#define Rad2Deg (1.f / Deg2Rad)
#include <string>
#include "Vector3.h"
struct Quaternion {
    union {
        struct { float x, y, z, w; };
        float data[4];
    };
    inline Quaternion() : x(0), y(0), z(0), w(1) {};
    inline Quaternion(float data[]) : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}
    inline Quaternion(Vector3 vector, float scalar) : x(vector.x), y(vector.y), z(vector.z), w(scalar) {};
    inline Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
    inline Quaternion(float Pitch, float Yaw, float Roll) {
        Quaternion tmp = Quaternion::FromEuler(Pitch, Yaw, Roll);
        x = tmp.x;
        y = tmp.y;
        z = tmp.z;
        w = tmp.w;
    };
    inline static Quaternion Identity() { return Quaternion(0, 0, 0, 1); };
    [[maybe_unused]] inline static Vector3 Up(Quaternion q);
    [[maybe_unused]] inline static Vector3 Down(Quaternion q);
    [[maybe_unused]] inline static Vector3 Left(Quaternion q);
    [[maybe_unused]] inline static Vector3 Right(Quaternion q);
    [[maybe_unused]] inline static Vector3 Forward(Quaternion q);
    [[maybe_unused]] inline static Vector3 Back(Quaternion q);
    inline static float Angle(Quaternion a, Quaternion b);
    inline static Quaternion Conjugate(Quaternion rotation);
    inline static float Dot(Quaternion lhs, Quaternion rhs);
    [[maybe_unused]] inline static Quaternion FromAngleAxis(float angle, Vector3 axis);
    [[maybe_unused]] inline static Quaternion FromEuler(Vector3 rotation);
    inline static Quaternion FromEuler(float x, float y, float z);
    inline static Quaternion FromToRotation(Vector3 fromVector, Vector3 toVector);
    [[maybe_unused]] inline static Quaternion Inverse(Quaternion rotation);
    [[maybe_unused]] inline static Quaternion Lerp(Quaternion a, Quaternion b, float t);
    inline static Quaternion LerpUnclamped(Quaternion a, Quaternion b, float t);
    [[maybe_unused]] inline static Quaternion LookRotation(Vector3 forward);
    inline static Quaternion LookRotation(Vector3 forward, Vector3 upwards);
    inline static float Norm(Quaternion rotation);
    inline static Quaternion Normalize(Quaternion rotation);
    [[maybe_unused]] inline static Quaternion RotateTowards(Quaternion from, Quaternion to, float maxRadiansDelta);
    [[maybe_unused]] inline static Quaternion Slerp(Quaternion a, Quaternion b, float t);
    inline static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t);
    [[maybe_unused]] inline static void ToAngleAxis(Quaternion rotation, float &angle, Vector3 &axis);
    inline static Vector3 ToEuler(Quaternion rotation);
    [[maybe_unused]] inline Vector3 euler() { return ToEuler(*this); }
    inline Quaternion normalized() { return Normalize(*this); }
    inline std::string str() {return std::to_string(x) + OBFUSCATES_BNM(", ") + std::to_string(y) + OBFUSCATES_BNM(", ") + std::to_string(z) + OBFUSCATES_BNM(", ") + std::to_string(w); }
    inline Quaternion& operator+=(float v) { x += v; y += v; z += v; w += v; return *this; }
    inline Quaternion& operator-=(float v) { x -= v; y -= v; z -= v; w -= v; return *this; }
    inline Quaternion& operator*=(float v) { x *= v; y *= v; z *= v; w *= v; return *this; }
    inline Quaternion& operator/=(float v) { x /= v; y /= v; z /= v; w /= v; return *this; }
    inline Quaternion& operator+=(Quaternion v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    inline Quaternion& operator-=(Quaternion v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    inline Quaternion& operator*=(Quaternion v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
};

inline Quaternion operator+(Quaternion lhs, const float rhs) { return Quaternion(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs); }
inline Quaternion operator-(Quaternion lhs, const float rhs) { return Quaternion(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs); }
inline Quaternion operator*(Quaternion lhs, const float rhs) { return Quaternion(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }
inline Quaternion operator/(Quaternion lhs, const float rhs) { return Quaternion(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs); }
inline Quaternion operator+(const float lhs, Quaternion rhs) { return Quaternion(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w); }
inline Quaternion operator-(const float lhs, Quaternion rhs) { return Quaternion(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w); }
inline Quaternion operator*(const float lhs, Quaternion rhs) { return Quaternion(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w); }
inline Quaternion operator/(const float lhs, Quaternion rhs) { return Quaternion(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w); }
inline Quaternion operator+(Quaternion lhs, const Quaternion rhs) { return Quaternion(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
inline Quaternion operator-(Quaternion lhs, const Quaternion rhs) { return Quaternion(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
inline Quaternion operator*(Quaternion lhs, const Quaternion rhs) { return Quaternion(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
inline Quaternion operator/(Quaternion lhs, const Quaternion rhs) { return Quaternion(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); }
inline bool operator==(const Quaternion lhs, const Quaternion rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w; }
inline bool operator!=(const Quaternion lhs, const Quaternion rhs) { return lhs.x != rhs.x && lhs.y != rhs.y && lhs.z != rhs.z && lhs.w != rhs.w; }
inline Quaternion operator-(Quaternion v) {return v * -1;}

inline Vector3 operator*(Quaternion lhs, const Vector3 rhs) {
    Vector3 u = Vector3(lhs.x, lhs.y, lhs.z);
    float s = lhs.w;
    return u * (Vector3::Dot(u, rhs) * 2.0f) + rhs * (s * s - Vector3::Dot(u, u)) + Vector3::Cross(u, rhs) * (2.0f * s);
}
[[maybe_unused]] Vector3 Quaternion::Up(Quaternion q) { return q * Vector3::Up(); }
[[maybe_unused]] Vector3 Quaternion::Down(Quaternion q) { return q * Vector3::Down(); }
[[maybe_unused]] Vector3 Quaternion::Left(Quaternion q) { return q * Vector3::Left(); }
[[maybe_unused]] Vector3 Quaternion::Right(Quaternion q) { return q * Vector3::Right(); }
[[maybe_unused]] Vector3 Quaternion::Forward(Quaternion q) { return q * Vector3::Forward(); }
[[maybe_unused]] Vector3 Quaternion::Back(Quaternion q) { return q * Vector3::Back(); }

float Quaternion::Angle(Quaternion a, Quaternion b) {
    float dot = Dot(a, b);
    return acosf(fminf(fabs(dot), 1)) * 2;
}

Quaternion Quaternion::Conjugate(Quaternion rotation) {
    return Quaternion(-rotation.x, -rotation.y, -rotation.z, rotation.w);
}

float Quaternion::Dot(Quaternion lhs, Quaternion rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

[[maybe_unused]] Quaternion Quaternion::FromAngleAxis(float angle, Vector3 axis) {
    Quaternion q;
    float m = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    float s = sinf(angle / 2) / m;
    q.x = axis.x * s;
    q.y = axis.y * s;
    q.z = axis.z * s;
    q.w = cosf(angle / 2);
    return q;
}

[[maybe_unused]] Quaternion Quaternion::FromEuler(Vector3 rotation) {
    return FromEuler(rotation.x, rotation.y, rotation.z);
}

Quaternion Quaternion::FromEuler(float x, float y, float z) {
    (x -= 180) *= Deg2Rad;
    (y -= 180) *= Deg2Rad;
    (z -= 180) *= Deg2Rad;
    float cx = cosf(x * 0.5f);
    float cy = cosf(y * 0.5f);
    float cz = cosf(z * 0.5f);
    float sx = sinf(x * 0.5f);
    float sy = sinf(y * 0.5f);
    float sz = sinf(z * 0.5f);
    Quaternion q;
    q.x = cx * sy * sz + cy * cz * sx;
    q.y = cx * cz * sy - cy * sx * sz;
    q.z = cx * cy * sz - cz * sx * sy;
    q.w = sx * sy * sz + cx * cy * cz;
    return q;
}

Quaternion Quaternion::FromToRotation(Vector3 fromVector, Vector3 toVector) {
    float dot = Vector3::Dot(fromVector, toVector);
    float k = sqrt(fromVector.sqrMagnitude() * toVector.sqrMagnitude());
    if (fabs(dot / k + 1) < 0.00001) {
        Vector3 ortho = fromVector.orthogonal();
        return Quaternion(ortho.normalized(), 0);
    }
    Vector3 cross = Vector3::Cross(fromVector, toVector);
    return Normalize(Quaternion(cross, dot + k));
}

[[maybe_unused]] Quaternion Quaternion::Inverse(Quaternion rotation) {
    float n = Norm(rotation);
    return Conjugate(rotation) / (n * n);
}

[[maybe_unused]] Quaternion Quaternion::Lerp(Quaternion a, Quaternion b, float t) {
    if (t < 0) return a.normalized();
    else if (t > 1) return b.normalized();
    return LerpUnclamped(a, b, t);
}

Quaternion Quaternion::LerpUnclamped(Quaternion a, Quaternion b, float t) {
    Quaternion quaternion;
    if (Dot(a, b) >= 0) quaternion = a * (1 - t) + b * t;
    else quaternion = a * (1 - t) - b * t;
    return quaternion.normalized();
}

[[maybe_unused]] Quaternion Quaternion::LookRotation(Vector3 forward) {
    return LookRotation(forward, Vector3(0, 1, 0));
}

Quaternion Quaternion::LookRotation(Vector3 forward, Vector3 upwards) {
    forward = forward.normalized();
    upwards = upwards.normalized();
    if (forward.sqrMagnitude() < SMALL_FLOAT || upwards.sqrMagnitude() < SMALL_FLOAT)
        return Quaternion::Identity();
    if (1 - fabs(Vector3::Dot(forward, upwards)) < SMALL_FLOAT)
        return FromToRotation(Vector3::Forward(), forward);
    Vector3 right = Vector3::Cross(upwards, forward).normalized();
    upwards = Vector3::Cross(forward, right);
    Quaternion quaternion;
    float radicand = right.x + upwards.y + forward.z;
    if (radicand > 0) {
        quaternion.w = sqrt(1.0f + radicand) * 0.5f;
        float recip = 1.0f / (4.0f * quaternion.w);
        quaternion.x = (upwards.z - forward.y) * recip;
        quaternion.y = (forward.x - right.z) * recip;
        quaternion.z = (right.y - upwards.x) * recip;
    }
    else if (right.x >= upwards.y && right.x >= forward.z) {
        quaternion.x = sqrt(1.0f + right.x - upwards.y - forward.z) * 0.5f;
        float recip = 1.0f / (4.0f * quaternion.x);
        quaternion.w = (upwards.z - forward.y) * recip;
        quaternion.z = (forward.x + right.z) * recip;
        quaternion.y = (right.y + upwards.x) * recip;
    }
    else if (upwards.y > forward.z) {
        quaternion.y = sqrt(1.0f - right.x + upwards.y - forward.z) * 0.5f;
        float recip = 1.0f / (4.0f * quaternion.y);
        quaternion.z = (upwards.z + forward.y) * recip;
        quaternion.w = (forward.x - right.z) * recip;
        quaternion.x = (right.y + upwards.x) * recip;
    }
    else {
        quaternion.z = sqrt(1.0f - right.x - upwards.y + forward.z) * 0.5f;
        float recip = 1.0f / (4.0f * quaternion.z);
        quaternion.y = (upwards.z + forward.y) * recip;
        quaternion.x = (forward.x + right.z) * recip;
        quaternion.w = (right.y - upwards.x) * recip;
    }
    return quaternion;
}

float Quaternion::Norm(Quaternion rotation) {
    return sqrt(rotation.x * rotation.x +
                rotation.y * rotation.y +
                rotation.z * rotation.z +
                rotation.w * rotation.w);
}

Quaternion Quaternion::Normalize(Quaternion rotation) {
    return rotation / Norm(rotation);
}

[[maybe_unused]] Quaternion Quaternion::RotateTowards(Quaternion from, Quaternion to, float maxRadiansDelta) {
    float angle = Quaternion::Angle(from, to);
    if (angle == 0) return to;
    maxRadiansDelta = fmaxf(maxRadiansDelta, angle - (float)M_PI);
    float t = fminf(1, maxRadiansDelta / angle);
    return Quaternion::SlerpUnclamped(from, to, t);
}

[[maybe_unused]] Quaternion Quaternion::Slerp(Quaternion a, Quaternion b, float t) {
    if (t < 0) return a.normalized();
    else if (t > 1) return b.normalized();
    return SlerpUnclamped(a, b, t);
}

Quaternion Quaternion::SlerpUnclamped(Quaternion a, Quaternion b, float t) {
    float n1;
    float n2;
    float n3 = Dot(a, b);
    bool flag = false;
    if (n3 < 0) {
        flag = true;
        n3 = -n3;
    }
    if (n3 > 0.999999) {
        n2 = 1 - t;
        n1 = flag ? -t : t;
    }
    else {
        float n4 = acosf(n3);
        float n5 = 1 / sinf(n4);
        n2 = sinf((1 - t) * n4) * n5;
        n1 = flag ? -sinf(t * n4) * n5 : sinf(t * n4) * n5;
    }
    Quaternion quaternion;
    quaternion.x = (n2 * a.x) + (n1 * b.x);
    quaternion.y = (n2 * a.y) + (n1 * b.y);
    quaternion.z = (n2 * a.z) + (n1 * b.z);
    quaternion.w = (n2 * a.w) + (n1 * b.w);
    return quaternion.normalized();
}

[[maybe_unused]] void Quaternion::ToAngleAxis(Quaternion rotation, float &angle, Vector3 &axis) {
    if (rotation.w > 1) rotation = rotation.normalized();
    angle = 2 * acosf(rotation.w);
    float s = sqrt(1 - rotation.w * rotation.w);
    if (s < 0.00001) {
        axis.x = 1;
        axis.y = 0;
        axis.z = 0;
    } else {
        axis.x = rotation.x / s;
        axis.y = rotation.y / s;
        axis.z = rotation.z / s;
    }
}
Vector3 Quaternion::ToEuler(Quaternion rotation) {
    float sqw = rotation.w * rotation.w;
    float sqx = rotation.x * rotation.x;
    float sqy = rotation.y * rotation.y;
    float sqz = rotation.z * rotation.z;
    float unit = sqx + sqy + sqz + sqw;
    float test = rotation.x * rotation.w - rotation.y * rotation.z;
    Vector3 v;
    if (test > 0.4995f * unit) {
        v.y = 2 * atan2f(rotation.y, rotation.x);
        v.x = (float)M_PI_2;
        v.z = 0;
        return v;
    }
    if (test < -0.4995f * unit) {
        v.y = -2 * atan2f(rotation.y, rotation.x);
        v.x = -(float)M_PI_2;
        v.z = 0;
        return v;
    }
    v.y = atan2f(2 * rotation.w * rotation.y + 2 * rotation.z * rotation.x, 1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y));
    v.x = asinf(2 * (rotation.w * rotation.x - rotation.y * rotation.z));
    v.z = atan2f(2 * rotation.w * rotation.z + 2 * rotation.x * rotation.y, 1 - 2 * (rotation.z * rotation.z + rotation.x * rotation.x));
    return (v * Rad2Deg) + 180;
}
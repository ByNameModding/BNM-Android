#pragma once

namespace BNM::Structures::Unity {
    struct Vector2;
    struct Vector3;

    struct Vector2 {
        union {
            struct { float x, y; };
            float data[2]{0.f, 0.f};
        };
        inline constexpr Vector2() : x(0), y(0) {};
        inline constexpr Vector2(float x, float y) : x(x), y(y) {};

        inline float* GetPtr() { return data; }
        [[nodiscard]] inline const float* GetPtr() const { return data; }

        inline float& operator[](int i) { return data[i]; }
        inline const float& operator[](int i) const { return data[i]; }

        inline static float Angle(Vector2 from, Vector2 to);
        inline static Vector2 ClampMagnitude(Vector2 vector, float maxLength);
        inline static float Component(Vector2 a, Vector2 b) { return Dot(a, b) / Magnitude(b); }
        inline static float Distance(Vector2 a, Vector2 b) { return Magnitude(a - b); }
        inline static float Dot(Vector2 lhs, Vector2 rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }
        inline static Vector2 FromPolar(float rad, float theta);
        inline static Vector2 Lerp(Vector2 from, Vector2 to, float t) { return to * t + from * (1.0f - t); }
        inline static Vector2 LerpUnclamped(Vector2 from, Vector2 to, float t) { return (to - from) * t + from; }
        inline static float Magnitude(Vector2 vector) { return sqrtf(vector.x * vector.x + vector.y * vector.y); }
        inline static Vector2 Max(Vector2 lhs, Vector2 rhs);
        inline static Vector2 Min(Vector2 lhs, Vector2 rhs);
        inline static Vector2 MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta);
        inline void Normalize() { *this = Normalize(*this); }
        inline static Vector2 Normalize(Vector2 vector);
        inline static void OrthoNormalize(Vector2 &normal, Vector2 &tangent);
        inline static Vector2 Perpendicular(Vector2 inDirection) { return {-inDirection.y, inDirection.x}; }
        inline static Vector2 Project(Vector2 a, Vector2 b);
        inline static Vector2 Reflect(Vector2 inDirection, Vector2 inNormal);
        inline static Vector2 Reject(Vector2 a, Vector2 b);
        inline static Vector2 RotateTowards(Vector2 current, Vector2 target, float maxRadiansDelta, float maxMagnitudeDelta);
        inline void Scale(Vector2 scale) { scale * scale; }
        inline static Vector2 Scale(Vector2 a, Vector2 b) { return a * b; }
        inline static Vector2 Slerp(Vector2, Vector2, float);
        inline static Vector2 SlerpUnclamped(Vector2, Vector2, float);
        inline static Vector2 SmoothDamp(Vector2 current, Vector2 target, Vector2 &currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
        inline static float SqrMagnitude(Vector2 vector)  { return vector.x * vector.x + vector.y * vector.y; }
        inline static void ToPolar(Vector2, float &, float &);

        inline float magnitude() { return Magnitude(*this); }
        inline float sqrMagnitude() { return SqrMagnitude(*this); }
        inline Vector2 normalized() { return Normalize(*this); }
        inline std::string str() const { return std::to_string(x) + std::string(BNM_OBFUSCATE(", ")) + std::to_string(y); }

        inline bool operator==(const Vector2 v) { return x == v.x && y == v.y; }
        inline bool operator!=(const Vector2 v) { return x != v.x || y != v.y; }
        inline Vector2& operator+=(float rhs) { x += rhs; y += rhs; return *this; }
        inline Vector2& operator-=(float rhs) { x -= rhs; y -= rhs; return *this; }
        inline Vector2& operator*=(float rhs) { x *= rhs; y *= rhs; return *this; }
        inline Vector2& operator/=(float rhs) { x /= rhs; y /= rhs; return *this; }
        inline Vector2& operator+=(Vector2 rhs) { x += rhs.x; y += rhs.y; return *this; }
        inline Vector2& operator-=(Vector2 rhs) { x -= rhs.x; y -= rhs.y; return *this; }
        inline friend Vector2 operator+(Vector2 lhs, float rhs) { return {lhs.x + rhs, lhs.y + rhs}; }
        inline friend Vector2 operator-(Vector2 lhs, float rhs) { return {lhs.x - rhs, lhs.y - rhs}; }
        inline friend Vector2 operator*(Vector2 lhs, float rhs) { return {lhs.x * rhs, lhs.y * rhs}; }
        inline friend Vector2 operator/(Vector2 lhs, float rhs) { return {lhs.x / rhs, lhs.y / rhs}; }
        inline friend Vector2 operator+(float lhs, Vector2 rhs) { return {lhs + rhs.x, lhs + rhs.y}; }
        inline friend Vector2 operator-(float lhs, Vector2 rhs) { return {lhs - rhs.x, lhs - rhs.y}; }
        inline friend Vector2 operator*(float lhs, Vector2 rhs) { return {lhs * rhs.x, lhs * rhs.y}; }
        inline friend Vector2 operator/(float lhs, Vector2 rhs) { return {lhs / rhs.x, lhs / rhs.y}; }
        inline friend Vector2 operator+(Vector2 lhs, Vector2 rhs) { return {lhs.x + rhs.x, lhs.y + rhs.y}; }
        inline friend Vector2 operator-(Vector2 lhs, Vector2 rhs) { return {lhs.x - rhs.x, lhs.y - rhs.y}; }
        inline friend Vector2 operator*(Vector2 lhs, Vector2 rhs) { return {lhs.x * rhs.x, lhs.y * rhs.y}; }
        inline friend Vector2 operator/(Vector2 lhs, Vector2 rhs) { return {lhs.x / rhs.x, lhs.y / rhs.y}; }
        inline Vector2 operator-() const { return {-x, -y}; }
        operator Vector3() const;

        static const Vector2 positiveInfinity;
        static const Vector2 negativeInfinity;
        static const Vector2 down;
        static const Vector2 left;
        static const Vector2 one;
        static const Vector2 right;
        static const Vector2 up;
        static const Vector2 zero;
    };


    float Vector2::Angle(Vector2 a, Vector2 b) {
        float v = Dot(a, b) / (Magnitude(a) * Magnitude(b));
        v = fmaxf(v, -1.f);
        v = fminf(v, 1.f);
        return acos(v);
    }

    Vector2 Vector2::ClampMagnitude(Vector2 vector, float maxLength) {
        float length = Magnitude(vector);
        if (length > maxLength) vector *= maxLength / length;
        return vector;
    }

    Vector2 Vector2::FromPolar(float rad, float theta) {
        Vector2 v;
        v.x = rad * cosf(theta);
        v.y = rad * sinf(theta);
        return v;
    }

    Vector2 Vector2::Max(Vector2 a, Vector2 b) {
        float x = a.x > b.x ? a.x : b.x;
        float y = a.y > b.y ? a.y : b.y;
        return {x, y};
    }

    Vector2 Vector2::Min(Vector2 a, Vector2 b) {
        float x = a.x > b.x ? b.x : a.x;
        float y = a.y > b.y ? b.y : a.y;
        return {x, y};
    }

    Vector2 Vector2::MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta) {
        Vector2 d = target - current;
        float m = Magnitude(d);
        if (m < maxDistanceDelta || m == 0) return target;
        return current + (d * maxDistanceDelta / m);
    }

    Vector2 Vector2::Normalize(Vector2 vector) {
        float mag = Magnitude(vector);
        if (mag == 0) return Vector2::zero;
        return vector / mag;
    }

    void Vector2::OrthoNormalize(Vector2 &normal, Vector2 &tangent) {
        normal = Normalize(normal);
        tangent = Reject(tangent, normal);
        tangent = Normalize(tangent);
    }

    Vector2 Vector2::Project(Vector2 a, Vector2 b) {
        float m = Magnitude(b);
        return Dot(a, b) / (m * m) * b;
    }

    Vector2 Vector2::Reflect(Vector2 inDirection, Vector2 inNormal) {
        return inDirection - 2 * Project(inDirection, inNormal);
    }

    Vector2 Vector2::Reject(Vector2 a, Vector2 b) {
        return a - Project(a, b);
    }

    Vector2 Vector2::RotateTowards(Vector2 current, Vector2 target, float maxRadiansDelta, float maxMagnitudeDelta) {
        float magCur = Magnitude(current);
        float magTar = Magnitude(target);
        float newMag = magCur + maxMagnitudeDelta * (float) ((magTar > magCur) - (magCur > magTar));
        newMag = fminf(newMag, fmaxf(magCur, magTar));
        newMag = fmaxf(newMag, fminf(magCur, magTar));
        float totalAngle = Angle(current, target) - maxRadiansDelta;
        if (totalAngle <= 0) return Normalize(target) * newMag;
        else if (totalAngle >= M_PI) return -Normalize(target) * newMag;
        float axis = current.x * target.y - current.y * target.x;
        axis = axis / fabsf(axis);
        if (1 - fabsf(axis) >= 0.00001) axis = 1;
        current = Normalize(current);
        Vector2 newVector = current * cosf(maxRadiansDelta) + Vector2(-current.y, current.x) * sinf(maxRadiansDelta) * axis;
        return newVector * newMag;
    }

    Vector2 Vector2::Slerp(Vector2 a, Vector2 b, float t) {
        if (t < 0) return a;
        else if (t > 1) return b;
        return SlerpUnclamped(a, b, t);
    }

    Vector2 Vector2::SlerpUnclamped(Vector2 a, Vector2 b, float t) {
        float magA = Magnitude(a);
        float magB = Magnitude(b);
        a /= magA;
        b /= magB;
        float dot = Dot(a, b);
        dot = fmaxf(dot, -1.f);
        dot = fminf(dot, 1.f);
        float theta = acosf(dot) * t;
        Vector2 relativeVec = Normalize(b - a * dot);
        Vector2 newVec = a * cosf(theta) + relativeVec * sinf(theta);
        return newVec * (magA + (magB - magA) * t);
    }

    Vector2 Vector2::SmoothDamp(Vector2 current, Vector2 target, Vector2 &currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
        smoothTime = std::max(0.0001f, smoothTime);
        float omega = 2.f / smoothTime;
        float x = omega * deltaTime;
        float exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
        Vector2 change = current - target;
        Vector2 originalTo = target;
        float maxChange = maxSpeed * smoothTime;
        float maxChangeSq = maxChange * maxChange;
        float sqrMag = SqrMagnitude(change);
        if (sqrMag > maxChangeSq) change /= sqrtf(sqrMag) * maxChange;
        target = current - change;
        Vector2 temp = (currentVelocity + omega * change) * deltaTime;
        currentVelocity = (currentVelocity - omega * temp) * exp;
        Vector2 output = target + (change + temp) * exp;
        Vector2 origMinusCurrent = originalTo - current;
        Vector2 outMinusOrig = output - originalTo;
        if (origMinusCurrent.x * outMinusOrig.x + origMinusCurrent.y * outMinusOrig.y > 0.f) {
            currentVelocity = (output - originalTo) / deltaTime;
            return originalTo;
        }
        return output;
    }

    void Vector2::ToPolar(Vector2 vector, float &rad, float &theta) {
        rad = Magnitude(vector);
        theta = atan2f(vector.y, vector.x);
    }

    struct Vector2Int {
        union {
            struct { int x, y; };
            int data[2]{0, 0};
        };
    };
}
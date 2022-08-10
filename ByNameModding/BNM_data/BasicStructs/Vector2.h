#pragma once
struct Vector2 {
    union {
        struct { float x, y; };
        float data[2];
    };
    inline Vector2() : x(0), y(0) {};
    inline Vector2(float x, float y) : x(x), y(y) {};
    [[maybe_unused]] inline static Vector2 Down() { return Vector2(0, -1); }
    [[maybe_unused]] inline static Vector2 Left() { return Vector2(-1, 0); }
    [[maybe_unused]] inline static Vector2 One() { return Vector2(1, 1); }
    [[maybe_unused]] inline static Vector2 Right() { return Vector2(1, 0); }
    [[maybe_unused]] inline static Vector2 Up() { return Vector2(0, 1); }
    inline static Vector2 Zero() { return Vector2(0, 0); }
    inline static float Angle(Vector2, Vector2);
    [[maybe_unused]] inline static Vector2 ClampMagnitude(Vector2, float);
    [[maybe_unused]] inline static float Component(Vector2, Vector2);
    [[maybe_unused]] inline static float Distance(Vector2, Vector2);
    inline static float Dot(Vector2, Vector2);
    [[maybe_unused]] inline static Vector2 FromPolar(float, float);
    [[maybe_unused]] inline static Vector2 Lerp(Vector2, Vector2, float) ;
    inline static Vector2 LerpUnclamped(Vector2, Vector2, float);
    inline static float Magnitude(Vector2);
    [[maybe_unused]] inline static Vector2 Max(Vector2, Vector2);
    [[maybe_unused]] inline static Vector2 Min(Vector2, Vector2);
    [[maybe_unused]] inline static Vector2 MoveTowards(Vector2, Vector2, float);
    inline static Vector2 Normalize(Vector2);
    [[maybe_unused]] inline static void OrthoNormalize(Vector2 &, Vector2 &);
    inline static Vector2 Project(Vector2, Vector2);
    [[maybe_unused]] inline static Vector2 Reflect(Vector2, Vector2);
    inline static Vector2 Reject(Vector2, Vector2);
    [[maybe_unused]] inline static Vector2 RotateTowards(Vector2, Vector2, float, float);
    [[maybe_unused]] inline static Vector2 Scale(Vector2, Vector2);
    [[maybe_unused]] inline static Vector2 Slerp(Vector2, Vector2, float);
    inline static Vector2 SlerpUnclamped(Vector2, Vector2, float);
    inline static float SqrMagnitude(Vector2);
    [[maybe_unused]] inline static void ToPolar(Vector2, float &, float &);
    inline float magnitude() { return Magnitude(*this); }
    inline float sqrMagnitude() { return SqrMagnitude(*this); }
    inline Vector2 normalized() { return Normalize(*this); }
    inline std::string str() { return std::to_string(x) + OBFUSCATES_BNM(", ") + std::to_string(y); }
    inline Vector2& operator+=(float rhs) { x += rhs; y += rhs; return *this; }
    inline Vector2& operator-=(float rhs) { x -= rhs; y -= rhs; return *this; }
    inline Vector2& operator*=(float rhs) { x *= rhs; y *= rhs; return *this; }
    inline Vector2& operator/=(float rhs) { x /= rhs; y /= rhs; return *this; }
    inline Vector2& operator+=(Vector2 rhs) { x += rhs.x; y += rhs.y; return *this; }
    inline Vector2& operator-=(Vector2 rhs) { x -= rhs.x; y -= rhs.y; return *this; }
};

inline Vector2 operator+(Vector2 lhs, const float rhs) { return Vector2(lhs.x + rhs, lhs.y + rhs); }
inline Vector2 operator-(Vector2 lhs, const float rhs) { return Vector2(lhs.x - rhs, lhs.y - rhs); }
inline Vector2 operator*(Vector2 lhs, const float rhs) { return Vector2(lhs.x * rhs, lhs.y * rhs); }
inline Vector2 operator/(Vector2 lhs, const float rhs) { return Vector2(lhs.x / rhs, lhs.y / rhs); }
inline Vector2 operator+(const float lhs, Vector2 rhs) { return Vector2(lhs + rhs.x, lhs + rhs.y); }
inline Vector2 operator-(const float lhs, Vector2 rhs) { return Vector2(lhs - rhs.x, lhs - rhs.y); }
inline Vector2 operator*(const float lhs, Vector2 rhs) { return Vector2(lhs * rhs.x, lhs * rhs.y); }
inline Vector2 operator/(const float lhs, Vector2 rhs) { return Vector2(lhs / rhs.x, lhs / rhs.y); }
inline Vector2 operator+(Vector2 lhs, const Vector2 rhs) { return Vector2(lhs.x + rhs.x, lhs.y + rhs.y); }
inline Vector2 operator-(Vector2 lhs, const Vector2 rhs) { return Vector2(lhs.x - rhs.x, lhs.y - rhs.y); }
inline Vector2 operator*(Vector2 lhs, const Vector2 rhs) { return Vector2(lhs.x * rhs.x, lhs.y * rhs.y); }
inline Vector2 operator/(Vector2 lhs, const Vector2 rhs) { return Vector2(lhs.x / rhs.x, lhs.y / rhs.y); }
inline bool operator==(const Vector2 lhs, const Vector2 rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator!=(const Vector2 lhs, const Vector2 rhs) { return lhs.x != rhs.x && lhs.y != rhs.y; }
inline Vector2 operator-(Vector2 v) {return v * -1;}

float Vector2::Angle(Vector2 a, Vector2 b) {
    float v = Dot(a, b) / (a.magnitude() * b.magnitude());
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    return acos(v);
}

[[maybe_unused]] Vector2 Vector2::ClampMagnitude(Vector2 vector, float maxLength) {
    float length = vector.magnitude();
    if (length > maxLength) vector *= maxLength / length;
    return vector;
}

[[maybe_unused]] float Vector2::Component(Vector2 a, Vector2 b) {
    return Dot(a, b) / b.magnitude();
}

[[maybe_unused]] float Vector2::Distance(Vector2 a, Vector2 b) {
    return (a - b).magnitude();
}

float Vector2::Dot(Vector2 lhs, Vector2 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

[[maybe_unused]] Vector2 Vector2::FromPolar(float rad, float theta) {
    Vector2 v;
    v.x = rad * cos(theta);
    v.y = rad * sin(theta);
    return v;
}

[[maybe_unused]] Vector2 Vector2::Lerp(Vector2 a, Vector2 b, float t) {
    if (t < 0) return a;
    else if (t > 1) return b;
    return LerpUnclamped(a, b, t);
}

Vector2 Vector2::LerpUnclamped(Vector2 a, Vector2 b, float t) {
    return (b - a) * t + a;
}

float Vector2::Magnitude(Vector2 v) {
    return sqrt(v.sqrMagnitude());
}

[[maybe_unused]] Vector2 Vector2::Max(Vector2 a, Vector2 b) {
    float x = a.x > b.x ? a.x : b.x;
    float y = a.y > b.y ? a.y : b.y;
    return Vector2(x, y);
}

[[maybe_unused]] Vector2 Vector2::Min(Vector2 a, Vector2 b) {
    float x = a.x > b.x ? b.x : a.x;
    float y = a.y > b.y ? b.y : a.y;
    return Vector2(x, y);
}

[[maybe_unused]] Vector2 Vector2::MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta) {
    Vector2 d = target - current;
    float m = d.magnitude();
    if (m < maxDistanceDelta || m == 0) return target;
    return current + (d * maxDistanceDelta / m);
}

Vector2 Vector2::Normalize(Vector2 v) {
    float mag = v.magnitude();
    if (mag == 0) return Vector2::Zero();
    return v / mag;
}

[[maybe_unused]] void Vector2::OrthoNormalize(Vector2 &normal, Vector2 &tangent) {
    normal = normal.normalized();
    tangent = Reject(tangent, normal);
    tangent = tangent.normalized();
}

Vector2 Vector2::Project(Vector2 a, Vector2 b) {
    float m = b.magnitude();
    return Dot(a, b) / (m * m) * b;
}

[[maybe_unused]] Vector2 Vector2::Reflect(Vector2 vector, Vector2 planeNormal) {
    return vector - 2 * Project(vector, planeNormal);
}

Vector2 Vector2::Reject(Vector2 a, Vector2 b) {
    return a - Project(a, b);
}

[[maybe_unused]] Vector2 Vector2::RotateTowards(Vector2 current, Vector2 target, float maxRadiansDelta, float maxMagnitudeDelta) {
    float magCur = current.magnitude();
    float magTar = target.magnitude();
    float newMag = magCur + maxMagnitudeDelta * ((magTar > magCur) - (magCur > magTar));
    newMag = fmin(newMag, fmax(magCur, magTar));
    newMag = fmax(newMag, fmin(magCur, magTar));
    float totalAngle = Angle(current, target) - maxRadiansDelta;
    if (totalAngle <= 0) return target.normalized() * newMag;
    else if (totalAngle >= M_PI) return -target.normalized() * newMag;
    float axis = current.x * target.y - current.y * target.x;
    axis = axis / fabs(axis);
    if (!(1 - fabs(axis) < 0.00001)) axis = 1;
    current = current.normalized();
    Vector2 newVector = current * cos(maxRadiansDelta) + Vector2(-current.y, current.x) * sin(maxRadiansDelta) * axis;
    return newVector * newMag;
}

[[maybe_unused]] Vector2 Vector2::Scale(Vector2 a, Vector2 b) {
    return Vector2(a.x * b.x, a.y * b.y);
}

[[maybe_unused]] Vector2 Vector2::Slerp(Vector2 a, Vector2 b, float t) {
    if (t < 0) return a;
    else if (t > 1) return b;
    return SlerpUnclamped(a, b, t);
}

Vector2 Vector2::SlerpUnclamped(Vector2 a, Vector2 b, float t) {
    float magA = a.magnitude();
    float magB = b.magnitude();
    a /= magA;
    b /= magB;
    float dot = Dot(a, b);
    dot = fmax(dot, -1.0);
    dot = fmin(dot, 1.0);
    float theta = acos(dot) * t;
    Vector2 relativeVec = (b - a * dot).normalized();
    Vector2 newVec = a * cos(theta) + relativeVec * sin(theta);
    return newVec * (magA + (magB - magA) * t);
}

float Vector2::SqrMagnitude(Vector2 v) {
    return v.x * v.x + v.y * v.y;
}

[[maybe_unused]] void Vector2::ToPolar(Vector2 vector, float &rad, float &theta) {
    rad = vector.magnitude();
    theta = atan2(vector.y, vector.x);
}
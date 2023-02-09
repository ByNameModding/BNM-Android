#pragma once
struct Quaternion;
struct Vector3 {
    union {
        struct { float x{}, y{}, z{}; };
        union {
            struct {
                // Y axis
                float pitch{};
                // Z axis
                float yaw{};
                // X axis
                float roll{};
            } UE4;
            struct {
                // Z axis
                float yaw{};
                // X axis
                float roll{};
                // Y axis
                float pitch{};
            } Unity;
        } Rot;
        float data[3];
    };
    inline Vector3() noexcept : x(0.f), y(0.f), z(0.f) {};
    inline Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {};
    inline static float Angle(Vector3 from, Vector3 to);
    inline static float Dot(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 ClampMagnitude(Vector3, float);
    [[maybe_unused]] inline static float Component(Vector3, Vector3);
    inline static Vector3 Cross(Vector3, Vector3);
    [[maybe_unused]] inline static float Distance(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 FromSpherical(float, float, float);
    [[maybe_unused]] inline static Vector3 Lerp(Vector3, Vector3, float);
    inline static Vector3 LerpUnclamped(Vector3, Vector3, float);
    [[maybe_unused]] inline static Vector3 Max(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 Min(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 MoveTowards(Vector3, Vector3, float);
    inline static Vector3 Orthogonal(Vector3);
    [[maybe_unused]] inline static Vector3 RotateTowards(Vector3, Vector3, float, float);
    [[maybe_unused]] inline static Vector3 Scale(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 Slerp(Vector3, Vector3, float);
    inline static Vector3 SlerpUnclamped(Vector3, Vector3, float);
    [[maybe_unused]] inline static void ToSpherical(Vector3 vector, float &, float &, float &);
    inline static float Magnitude(Vector3);
    inline static float SqrMagnitude(Vector3);
    inline static Vector3 Normalize(Vector3);
    inline static Vector3 NormalizeEuler(Vector3, bool is180 = true);
    inline static float NormalizeAngle(float f, bool is180 = true);
    [[maybe_unused]] inline static Vector3 FromString(std::string);
    inline Vector3 orthogonal() { return Orthogonal(*this); }
    inline float magnitude() { return Magnitude(*this); }
    inline float sqrMagnitude() { return SqrMagnitude(*this); }
    inline Vector3 normalized() { return Normalize(*this); }
    [[maybe_unused]] inline Vector3 normalizedEuler(bool is180 = true) { return NormalizeEuler(*this, is180); }
    inline std::string str() { return std::to_string(x) + OBFUSCATES_BNM(", ") + std::to_string(y) + OBFUSCATES_BNM(", ") + std::to_string(z); }
    bool operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vector3& v) const { return x != v.x || y != v.y || z != v.z; }
    Vector3& operator+=(const Vector3& inV) { x += inV.x; y += inV.y; z += inV.z; return *this; }
    Vector3& operator-=(const Vector3& inV) { x -= inV.x; y -= inV.y; z -= inV.z; return *this; }
    Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    Vector3& operator/=(float s);
    Vector3& operator/=(const Vector3& inV) { x /= inV.x; y /= inV.y; z /= inV.z; return *this; }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }
    [[maybe_unused]] static const Vector3 infinityVec;
    [[maybe_unused]] static const Vector3 zero;
    [[maybe_unused]] static const Vector3 one;
    [[maybe_unused]] static const Vector3 up;
    [[maybe_unused]] static const Vector3 down;
    [[maybe_unused]] static const Vector3 left;
    [[maybe_unused]] static const Vector3 right;
    [[maybe_unused]] static const Vector3 forward;
    [[maybe_unused]] static const Vector3 back;
};
inline Vector3 operator+(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); }
inline Vector3 operator-(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }
inline Vector3 operator*(const Vector3& inV, const float s) { return Vector3(inV.x * s, inV.y * s, inV.z * s); }
inline Vector3 operator*(const float s, const Vector3& inV) { return Vector3(inV.x * s, inV.y * s, inV.z * s); }
inline Vector3 operator*(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); }
inline Vector3 operator/(const Vector3& inV, const float s) { Vector3 temp(inV); temp /= s; return temp; }
inline Vector3 operator/(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); }
inline Vector3& Vector3::operator/=(float s) {
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

float Vector3::Angle(Vector3 from, Vector3 to) {
    float v = Dot(from, to) / (from.magnitude() * to.magnitude());
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    return acos(v);
}

float Vector3::Dot(Vector3 lhs, Vector3 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
[[maybe_unused]] Vector3 Vector3::ClampMagnitude(Vector3 vector, float maxLength) {
    float length = vector.magnitude();
    if (length > maxLength) vector *= maxLength / length;
    return vector;
}

[[maybe_unused]] float Vector3::Component(Vector3 a, Vector3 b) {
    return Dot(a, b) / b.magnitude();
}

Vector3 Vector3::Cross(Vector3 lhs, Vector3 rhs) {
    float x = lhs.y * rhs.z - lhs.z * rhs.y;
    float y = lhs.z * rhs.x - lhs.x * rhs.z;
    float z = lhs.x * rhs.y - lhs.y * rhs.x;
    return Vector3(x, y, z);
}

[[maybe_unused]] float Vector3::Distance(Vector3 a, Vector3 b) {
    return (a - b).magnitude();
}

[[maybe_unused]] Vector3 Vector3::FromSpherical(float rad, float theta, float phi) {
    Vector3 v;
    v.x = rad * sin(theta) * cos(phi);
    v.y = rad * sin(theta) * sin(phi);
    v.z = rad * cos(theta);
    return v;
}

[[maybe_unused]] Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float t) {
    if (t < 0) return a;
    else if (t > 1) return b;
    return LerpUnclamped(a, b, t);
}

Vector3 Vector3::LerpUnclamped(Vector3 a, Vector3 b, float t) {
    return (b - a) * t + a;
}

[[maybe_unused]] Vector3 Vector3::Max(Vector3 a, Vector3 b) {
    float x = a.x > b.x ? a.x : b.x;
    float y = a.y > b.y ? a.y : b.y;
    float z = a.z > b.z ? a.z : b.z;
    return Vector3(x, y, z);
}

[[maybe_unused]] Vector3 Vector3::Min(Vector3 a, Vector3 b) {
    float x = a.x > b.x ? b.x : a.x;
    float y = a.y > b.y ? b.y : a.y;
    float z = a.z > b.z ? b.z : a.z;
    return Vector3(x, y, z);
}

[[maybe_unused]] Vector3 Vector3::MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta) {
    Vector3 d = target - current;
    float m = d.magnitude();
    if (m < maxDistanceDelta || m == 0)
        return target;
    return current + (d * maxDistanceDelta / m);
}

Vector3 Vector3::Orthogonal(Vector3 v) {
    return v.z < v.x ? Vector3(v.y, -v.x, 0) : Vector3(0, -v.z, v.y);
}

[[maybe_unused]] Vector3 Vector3::RotateTowards(Vector3 current, Vector3 target, float maxRadiansDelta, float maxMagnitudeDelta) {
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

[[maybe_unused]] Vector3 Vector3::Scale(Vector3 a, Vector3 b) {
    return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

[[maybe_unused]] Vector3 Vector3::Slerp(Vector3 a, Vector3 b, float t) {
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
    Vector3 relativeVec = Normalize(b - a * dot);
    Vector3 newVec = a * cos(theta) + relativeVec * sin(theta);
    return newVec * (magA + (magB - magA) * t);
}

[[maybe_unused]] void Vector3::ToSpherical(Vector3 vector, float &rad, float &theta, float &phi) {
    rad = vector.magnitude();
    float v = vector.z / rad;
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    theta = acos(v);
    phi = atan2(vector.y, vector.x);
}

float Vector3::Magnitude(Vector3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float Vector3::SqrMagnitude(Vector3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
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

Vector3 Vector3::NormalizeEuler(Vector3 vec, bool is180) {
    vec.x = NormalizeAngle(vec.x, is180);
    vec.y = NormalizeAngle(vec.y, is180);
    vec.z = NormalizeAngle(vec.z, is180);
    return vec;
}

[[maybe_unused]] Vector3 Vector3::FromString(std::string str) {
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

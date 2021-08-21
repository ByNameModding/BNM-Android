#pragma once

using namespace std;
#include "../macros.h"
#include <math.h>

struct Vector3
{
    union {
        struct {
            float x;
            float y;
            float z;
        };
        float data[3];
    };
    inline Vector3();
    inline Vector3(float data[]);
    inline Vector3(float value);
    inline Vector3(float x, float y);
    inline Vector3(float x, float y, float z);


    /**
     * Constructors.
     */


    /**
     * Constants for common vectors.
     */
    static inline Vector3 zero();
    static inline Vector3 One();
    static inline Vector3 Right();
    static inline Vector3 Left();
    static inline Vector3 Up();
    static inline Vector3 Down();
    static inline Vector3 Forward();
    static inline Vector3 Backward();


    /**
     * Returns the angle between two vectors in radians.
     * @param a: The first vector.
     * @param b: The second vector.
     * @return: A scalar value.
     */
    static inline float Angle(Vector3 a, Vector3 b);

    /**
     * Returns a vector with its magnitude clamped to maxLength.
     * @param vector: The target vector.
     * @param maxLength: The maximum length of the return vector.
     * @return: A new vector.
     */
    static inline Vector3 ClampMagnitude(Vector3 vector, float maxLength);

    /**
     * Returns the component of a in the direction of b (scalar projection).
     * @param a: The target vector.
     * @param b: The vector being compared against.
     * @return: A scalar value.
     */
    static inline float Component(Vector3 a, Vector3 b);

    /**
     * Returns the cross product of two vectors.
     * @param lhs: The left side of the multiplication.
     * @param rhs: The right side of the multiplication.
     * @return: A new vector.
     */
    static inline Vector3 Cross(Vector3 lhs, Vector3 rhs);

    /**
     * Returns the distance between a and b.
     * @param a: The first point.
     * @param b: The second point.
     * @return: A scalar value.
     */
    static inline float Distance(Vector3 a, Vector3 b);

    static inline float Distance2(Vector3 a, Vector3 b);

    static inline char ToChar(Vector3 a);

    /**
     * Returns the dot product of two vectors.
     * @param lhs: The left side of the multiplication.
     * @param rhs: The right side of the multiplication.
     * @return: A scalar value.
     */
    static inline float Dot(Vector3 lhs, Vector3 rhs);

    /**
     * Converts a spherical representation of a vector into cartesian
     * coordinates.
     * This uses the ISO convention (radius r, inclination theta, azimuth phi).
     * @param rad: The magnitude of the vector.
     * @param theta: The angle in the xy plane from the x axis.
     * @param phi: The angle from the positive z axis to the vector.
     * @return: A new vector.
     */
    static inline Vector3 FromSpherical(float rad, float theta, float phi);

    /**
     * Returns a vector linearly interpolated between a and b, moving along
     * a straight line. The vector is clamped to never go beyond the end points.
     * @param a: The starting point.
     * @param b: The ending point.
     * @param t: The interpolation value [0-1].
     * @return: A new vector.
     */
    static inline Vector3 Lerp(Vector3 a, Vector3 b, float t);

    /**
     * Returns a vector linearly interpolated between a and b, moving along
     * a straight line.
     * @param a: The starting point.
     * @param b: The ending point.
     * @param t: The interpolation value [0-1] (no actual bounds).
     * @return: A new vector.
     */
    static inline Vector3 LerpUnclamped(Vector3 a, Vector3 b, float t);

    /**
     * Returns the magnitude of a vector.
     * @param v: The vector in question.
     * @return: A scalar value.
     */
    static inline float Magnitude(Vector3 v);

    /**
     * Returns a vector made from the largest components of two other vectors.
     * @param a: The first vector.
     * @param b: The second vector.
     * @return: A new vector.
     */
    static inline Vector3 Max(Vector3 a, Vector3 b);

    /**
     * Returns a vector made from the smallest components of two other vectors.
     * @param a: The first vector.
     * @param b: The second vector.
     * @return: A new vector.
     */
    static inline Vector3 Min(Vector3 a, Vector3 b);

    /**
     * Returns a vector "maxDistanceDelta" units closer to the target. This
     * interpolation is in a straight line, and will not overshoot.
     * @param current: The current position.
     * @param target: The destination position.
     * @param maxDistanceDelta: The maximum distance to move.
     * @return: A new vector.
     */
    static inline Vector3 MoveTowards(Vector3 current, Vector3 target,
                                      float maxDistanceDelta);

    /**
     * Returns a new vector with magnitude of one.
     * @param v: The vector in question.
     * @return: A new vector.
     */
    static inline Vector3 Normalized(Vector3 v);

    /**
     * Returns an arbitrary vector orthogonal to the input.
     * This vector is not normalized.
     * @param v: The input vector.
     * @return: A new vector.
     */
    static inline Vector3 Orthogonal(Vector3 v);

    /**
     * Creates a new coordinate system out of the three vectors.
     * Normalizes "normal", normalizes "tangent" and makes it orthogonal to
     * "normal" and normalizes "binormal" and makes it orthogonal to both
     * "normal" and "tangent".
     * @param normal: A reference to the first axis vector.
     * @param tangent: A reference to the second axis vector.
     * @param binormal: A reference to the third axis vector.
     */
    static inline void OrthoNormalize(Vector3 &normal, Vector3 &tangent,
                                      Vector3 &binormal);

    /**
     * Returns the vector projection of a onto b.
     * @param a: The target vector.
     * @param b: The vector being projected onto.
     * @return: A new vector.
     */
    static inline Vector3 Project(Vector3 a, Vector3 b);

    /**
     * Returns a vector projected onto a plane orthogonal to "planeNormal".
     * This can be visualized as the shadow of the vector onto the plane, if
     * the light source were in the direction of the plane normal.
     * @param vector: The vector to project.
     * @param planeNormal: The normal of the plane onto which to project.
     * @param: A new vector.
     */
    static inline Vector3 ProjectOnPlane(Vector3 vector, Vector3 planeNormal);

    /**
     * Returns a vector reflected off the plane orthogonal to the normal.
     * The input vector is pointed inward, at the plane, and the return vector
     * is pointed outward from the plane, like a beam of light hitting and then
     * reflecting off a mirror.
     * @param vector: The vector traveling inward at the plane.
     * @param planeNormal: The normal of the plane off of which to reflect.
     * @return: A new vector pointing outward from the plane.
     */
    static inline Vector3 Reflect(Vector3 vector, Vector3 planeNormal);

    /**
     * Returns the vector rejection of a on b.
     * @param a: The target vector.
     * @param b: The vector being projected onto.
     * @return: A new vector.
     */
    static inline Vector3 Reject(Vector3 a, Vector3 b);

    /**
     * Rotates vector "current" towards vector "target" by "maxRadiansDelta".
     * This treats the vectors as directions and will linearly interpolate
     * between their magnitudes by "maxMagnitudeDelta". This function does not
     * overshoot. If a negative delta is supplied, it will rotate away from
     * "target" until it is pointing the opposite direction, but will not
     * overshoot that either.
     * @param current: The starting direction.
     * @param target: The destination direction.
     * @param maxRadiansDelta: The maximum number of radians to rotate.
     * @param maxMagnitudeDelta: The maximum delta for magnitude interpolation.
     * @return: A new vector.
     */
    static inline Vector3 RotateTowards(Vector3 current, Vector3 target,
                                        float maxRadiansDelta,
                                        float maxMagnitudeDelta);

    /**
     * Multiplies two vectors element-wise.
     * @param a: The lhs of the multiplication.
     * @param b: The rhs of the multiplication.
     * @return: A new vector.
     */
    static inline Vector3 Scale(Vector3 a, Vector3 b);

    /**
     * Returns a vector rotated towards b from a by the percent t.
     * Since interpolation is done spherically, the vector moves at a constant
     * angular velocity. This rotation is clamped to 0 <= t <= 1.
     * @param a: The starting direction.
     * @param b: The ending direction.
     * @param t: The interpolation value [0-1].
     */
    static inline Vector3 Slerp(Vector3 a, Vector3 b, float t);

    /**
     * Returns a vector rotated towards b from a by the percent t.
     * Since interpolation is done spherically, the vector moves at a constant
     * angular velocity. This rotation is unclamped.
     * @param a: The starting direction.
     * @param b: The ending direction.
     * @param t: The interpolation value [0-1].
     */
    static inline Vector3 SlerpUnclamped(Vector3 a, Vector3 b, float t);

    /**
     * Returns the squared magnitude of a vector.
     * This is useful when comparing relative lengths, where the exact length
     * is not important, and much time can be saved by not calculating the
     * square root.
     * @param v: The vector in question.
     * @return: A scalar value.
     */
    static inline float SqrMagnitude(Vector3 v);

    /**
     * Calculates the spherical coordinate space representation of a vector.
     * This uses the ISO convention (radius r, inclination theta, azimuth phi).
     * @param vector: The vector to convert.
     * @param rad: The magnitude of the vector.
     * @param theta: The angle in the xy plane from the x axis.
     * @param phi: The angle from the positive z axis to the vector.
     */
    static inline void ToSpherical(Vector3 vector, float &rad, float &theta,
                                   float &phi);


    /**
     * Operator overloading.
     */
    inline struct Vector3& operator+=(const float rhs);
    inline struct Vector3& operator-=(const float rhs);
    inline struct Vector3& operator*=(const float rhs);
    inline struct Vector3& operator/=(const float rhs);
    inline struct Vector3& operator+=(const Vector3 rhs);
    inline struct Vector3& operator-=(const Vector3 rhs);
};

inline Vector3 operator-(Vector3 rhs);
inline Vector3 operator+(Vector3 lhs, const float rhs);
inline Vector3 operator-(Vector3 lhs, const float rhs);
inline Vector3 operator*(Vector3 lhs, const float rhs);
inline Vector3 operator/(Vector3 lhs, const float rhs);
inline Vector3 operator+(const float lhs, Vector3 rhs);
inline Vector3 operator-(const float lhs, Vector3 rhs);
inline Vector3 operator*(const float lhs, Vector3 rhs);
inline Vector3 operator/(const float lhs, Vector3 rhs);
inline Vector3 operator+(Vector3 lhs, const Vector3 rhs);
inline Vector3 operator-(Vector3 lhs, const Vector3 rhs);
inline bool operator==(const Vector3 lhs, const Vector3 rhs);
inline bool operator!=(const Vector3 lhs, const Vector3 rhs);



/*******************************************************************************
 * Implementation
 */

Vector3::Vector3() : x(0), y(0), z(0) {}
Vector3::Vector3(float data[]) : x(data[0]), y(data[1]), z(data[2]) {}
Vector3::Vector3(float value) : x(value), y(value), z(value) {}
Vector3::Vector3(float x, float y) : x(x), y(y), z(0) {}
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}


Vector3 Vector3::zero() { return Vector3(0, 0, 0); }
Vector3 Vector3::One() { return Vector3(1, 1, 1); }
Vector3 Vector3::Right() { return Vector3(1, 0, 0); }
Vector3 Vector3::Left() { return Vector3(-1, 0, 0); }
Vector3 Vector3::Up() { return Vector3(0, 1, 0); }
Vector3 Vector3::Down() { return Vector3(0, -1, 0); }
Vector3 Vector3::Forward() { return Vector3(0, 0, 1); }
Vector3 Vector3::Backward() { return Vector3(0, 0, -1); }


float Vector3::Angle(Vector3 a, Vector3 b)
{
    float v = Dot(a, b) / (Magnitude(a) * Magnitude(b));
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    return acos(v);
}

Vector3 Vector3::ClampMagnitude(Vector3 vector, float maxLength)
{
    float length = Magnitude(vector);
    if (length > maxLength)
        vector *= maxLength / length;
    return vector;
}

float Vector3::Component(Vector3 a, Vector3 b)
{
    return Dot(a, b) / Magnitude(b);
}

Vector3 Vector3::Cross(Vector3 lhs, Vector3 rhs)
{
    float x = lhs.y * rhs.z - lhs.z * rhs.y;
    float y = lhs.z * rhs.x - lhs.x * rhs.z;
    float z = lhs.x * rhs.y - lhs.y * rhs.x;
    return Vector3(x, y, z);
}

float Vector3::Distance(Vector3 a, Vector3 b)
{
    return Vector3::Magnitude(a - b);
}
float Vector3::Distance2(Vector3 a, Vector3 b)
{
    return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2) + pow((a.z - b.z), 2));
}

float Vector3::Dot(Vector3 lhs, Vector3 rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector3 Vector3::FromSpherical(float rad, float theta, float phi)
{
    Vector3 v;
    v.x = rad * sin(theta) * cos(phi);
    v.y = rad * sin(theta) * sin(phi);
    v.z = rad * cos(theta);
    return v;
}

Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float t)
{
    if (t < 0) return a;
    else if (t > 1) return b;
    return LerpUnclamped(a, b, t);
}

Vector3 Vector3::LerpUnclamped(Vector3 a, Vector3 b, float t)
{
    return (b - a) * t + a;
}

float Vector3::Magnitude(Vector3 v)
{
    return sqrt(SqrMagnitude(v));
}

Vector3 Vector3::Max(Vector3 a, Vector3 b)
{
    float x = a.x > b.x ? a.x : b.x;
    float y = a.y > b.y ? a.y : b.y;
    float z = a.z > b.z ? a.z : b.z;
    return Vector3(x, y, z);
}

Vector3 Vector3::Min(Vector3 a, Vector3 b)
{
    float x = a.x > b.x ? b.x : a.x;
    float y = a.y > b.y ? b.y : a.y;
    float z = a.z > b.z ? b.z : a.z;
    return Vector3(x, y, z);
}

Vector3 Vector3::MoveTowards(Vector3 current, Vector3 target,
                             float maxDistanceDelta)
{
    Vector3 d = target - current;
    float m = Magnitude(d);
    if (m < maxDistanceDelta || m == 0)
        return target;
    return current + (d * maxDistanceDelta / m);
}

Vector3 Vector3::Normalized(Vector3 v)
{
    float mag = Magnitude(v);
    if (mag == 0)
        return Vector3::zero();
    return v / mag;
}

Vector3 Vector3::Orthogonal(Vector3 v)
{
    return v.z < v.x ? Vector3(v.y, -v.x, 0) : Vector3(0, -v.z, v.y);
}

void Vector3::OrthoNormalize(Vector3 &normal, Vector3 &tangent,
                             Vector3 &binormal)
{
    normal = Normalized(normal);
    tangent = ProjectOnPlane(tangent, normal);
    tangent = Normalized(tangent);
    binormal = ProjectOnPlane(binormal, tangent);
    binormal = ProjectOnPlane(binormal, normal);
    binormal = Normalized(binormal);
}

Vector3 Vector3::Project(Vector3 a, Vector3 b)
{
    float m = Magnitude(b);
    return Dot(a, b) / (m * m) * b;
}

Vector3 Vector3::ProjectOnPlane(Vector3 vector, Vector3 planeNormal)
{
    return Reject(vector, planeNormal);
}

Vector3 Vector3::Reflect(Vector3 vector, Vector3 planeNormal)
{
    return vector - 2 * Project(vector, planeNormal);
}

Vector3 Vector3::Reject(Vector3 a, Vector3 b)
{
    return a - Project(a, b);
}

Vector3 Vector3::RotateTowards(Vector3 current, Vector3 target,
                               float maxRadiansDelta,
                               float maxMagnitudeDelta)
{
    float magCur = Magnitude(current);
    float magTar = Magnitude(target);
    float newMag = magCur + maxMagnitudeDelta *
                            ((magTar > magCur) - (magCur > magTar));
    newMag = fmin(newMag, fmax(magCur, magTar));
    newMag = fmax(newMag, fmin(magCur, magTar));

    float totalAngle = Angle(current, target) - maxRadiansDelta;
    if (totalAngle <= 0)
        return Normalized(target) * newMag;
    else if (totalAngle >= M_PI)
        return Normalized(-target) * newMag;

    Vector3 axis = Cross(current, target);
    float magAxis = Magnitude(axis);
    if (magAxis == 0)
        axis = Normalized(Cross(current, current + Vector3(3.95, 5.32, -4.24)));
    else
        axis /= magAxis;
    current = Normalized(current);
    Vector3 newVector = current * cos(maxRadiansDelta) +
                        Cross(axis, current) * sin(maxRadiansDelta);
    return newVector * newMag;
}

Vector3 Vector3::Scale(Vector3 a, Vector3 b)
{
    return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3 Vector3::Slerp(Vector3 a, Vector3 b, float t)
{
    if (t < 0) return a;
    else if (t > 1) return b;
    return SlerpUnclamped(a, b, t);
}

Vector3 Vector3::SlerpUnclamped(Vector3 a, Vector3 b, float t)
{
    float magA = Magnitude(a);
    float magB = Magnitude(b);
    a /= magA;
    b /= magB;
    float dot = Dot(a, b);
    dot = fmax(dot, -1.0);
    dot = fmin(dot, 1.0);
    float theta = acos(dot) * t;
    Vector3 relativeVec = Normalized(b - a * dot);
    Vector3 newVec = a * cos(theta) + relativeVec * sin(theta);
    return newVec * (magA + (magB - magA) * t);
}

float Vector3::SqrMagnitude(Vector3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

void Vector3::ToSpherical(Vector3 vector, float &rad, float &theta,
                          float &phi)
{
    rad = Magnitude(vector);
    float v = vector.z / rad;
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    theta = acos(v);
    phi = atan2(vector.y, vector.x);
}


struct Vector3& Vector3::operator+=(const float rhs)
{
    x += rhs;
    y += rhs;
    z += rhs;
    return *this;
}

struct Vector3& Vector3::operator-=(const float rhs)
{
    x -= rhs;
    y -= rhs;
    z -= rhs;
    return *this;
}

struct Vector3& Vector3::operator*=(const float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

struct Vector3& Vector3::operator/=(const float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

struct Vector3& Vector3::operator+=(const Vector3 rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

struct Vector3& Vector3::operator-=(const Vector3 rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

char Vector3::ToChar(Vector3 a) {
    const char* x = (const char*)(int)a.x;
    const char* y = (const char*)(int)a.y;
    const char* z = (const char*)(int)a.z;
    char buffer[25];
    strncpy(buffer, x, sizeof(buffer));
    strncpy(buffer, ", ", sizeof(buffer));
    strncpy(buffer, y, sizeof(buffer));
    strncpy(buffer, ", ", sizeof(buffer));
    strncpy(buffer, z, sizeof(buffer));
    strncpy(buffer, ", ", sizeof(buffer));
    return buffer[25];
}

Vector3 operator-(Vector3 rhs) { return rhs * -1; }
Vector3 operator+(Vector3 lhs, const float rhs) { return lhs += rhs; }
Vector3 operator-(Vector3 lhs, const float rhs) { return lhs -= rhs; }
Vector3 operator*(Vector3 lhs, const float rhs) { return lhs *= rhs; }
Vector3 operator/(Vector3 lhs, const float rhs) { return lhs /= rhs; }
Vector3 operator+(const float lhs, Vector3 rhs) { return rhs += lhs; }
Vector3 operator-(const float lhs, Vector3 rhs) { return rhs -= lhs; }
Vector3 operator*(const float lhs, Vector3 rhs) { return rhs *= lhs; }
Vector3 operator/(const float lhs, Vector3 rhs) { return rhs /= lhs; }
Vector3 operator+(Vector3 lhs, const Vector3 rhs) { return lhs += rhs; }
Vector3 operator-(Vector3 lhs, const Vector3 rhs) { return lhs -= rhs; }

bool operator==(const Vector3 lhs, const Vector3 rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

bool operator!=(const Vector3 lhs, const Vector3 rhs)
{
    return !(lhs == rhs);
}

std::string to_string(Vector3 a) {
    return to_string(a.x) + OBFUSCATES_BNM(", ") + to_string(a.y) + OBFUSCATES_BNM(", ") + to_string(a.z);
}
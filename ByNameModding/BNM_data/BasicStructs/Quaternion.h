#pragma once
#define SMALL_FLOAT 0.0000000001
#define Deg2Rad (M_PI / 180.0)
#define Rad2Deg (1.0 / Deg2Rad)
#include <string>
#include "Vector3.h"
namespace BNM::Structures::Unity {
    struct Quaternion {
        union {
            struct { float x, y, z, w; };
            float data[4];
        };
        inline Quaternion() noexcept : x(0), y(0), z(0), w(1) {};
        inline Quaternion(float data[]) noexcept : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}
        inline Quaternion(Vector3 vector, float scalar) noexcept : x(vector.x), y(vector.y), z(vector.z), w(scalar) {};
        inline Quaternion(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {};
        inline Quaternion(float Pitch, float Yaw, float Roll) {
            *this = Quaternion::FromEuler(Pitch, Yaw, Roll);
        };
        inline static Quaternion Identity() { return Quaternion(0, 0, 0, 1); };
        inline static Vector3 Up(Quaternion q);
        inline static Vector3 Down(Quaternion q);
        inline static Vector3 Left(Quaternion q);
        inline static Vector3 Right(Quaternion q);
        inline static Vector3 Forward(Quaternion q);
        inline static Vector3 Back(Quaternion q);
        inline static float Angle(Quaternion a, Quaternion b);
        inline static Quaternion Conjugate(Quaternion rotation);
        inline static float Dot(Quaternion lhs, Quaternion rhs);
        inline static Quaternion FromAngleAxis(float angle, Vector3 axis);
        inline static Quaternion FromEuler(Vector3 rotation);
        inline static Quaternion FromEuler(float roll, float pitch, float yaw, bool fromDeg = true);
        inline static Quaternion FromToRotation(Vector3 fromVector, Vector3 toVector);
        inline static Quaternion Inverse(Quaternion rotation);
        inline static Quaternion Lerp(Quaternion a, Quaternion b, float t);
        inline static Quaternion LerpUnclamped(Quaternion a, Quaternion b, float t);
        inline static Quaternion LookRotation(Vector3 forward);
        inline static Quaternion LookRotation(Vector3 forward, Vector3 upwards);
        inline static float Norm(Quaternion rotation);
        inline static Quaternion Normalize(Quaternion rotation);
        inline static Quaternion RotateTowards(Quaternion from, Quaternion to, float maxRadiansDelta);
        inline static Quaternion Slerp(Quaternion a, Quaternion b, float t);
        inline static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t);
        inline static void ToAngleAxis(Quaternion rotation, float &angle, Vector3 &axis);
        inline static Vector3 ToEuler(Quaternion rotation, bool toDeg = true);
        inline Vector3 euler() { return ToEuler(*this); }
        inline Quaternion normalized() { return Normalize(*this); }
        inline std::string str() {return std::to_string(x) + OBFUSCATES_BNM(", ") + std::to_string(y) + OBFUSCATES_BNM(", ") + std::to_string(z) + OBFUSCATES_BNM(", ") + std::to_string(w); }
        Quaternion& operator+=(const Quaternion& aQuat);
        Quaternion& operator-=(const Quaternion& aQuat);
        Quaternion& operator*=(const float aScalar);
        Quaternion& operator*=(const Quaternion& aQuat);
        Quaternion& operator/=(const Quaternion& aQuat);
        Quaternion& operator/=(const float aScalar);
        friend Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {
            Quaternion q(lhs);
            return q += rhs;
        }

        friend Quaternion  operator-(const Quaternion& lhs, const Quaternion& rhs) {
            Quaternion t(lhs);
            return t -= rhs;
        }

        Quaternion operator-() const {
            return Quaternion(-x, -y, -z, -w);
        }

        Quaternion operator*(const float s) const {
            return Quaternion(x * s, y * s, z * s, w * s);
        }

        friend Quaternion  operator*(const float s, const Quaternion& q) {
            Quaternion t(q);
            return t *= s;
        }

        friend Quaternion  operator/(const Quaternion& q, const float s) {
            Quaternion t(q);
            return t /= s;
        }

        inline friend Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) {
            return {lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
                    lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z,
                    lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x,
                    lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z};
        }
        inline friend Quaternion operator/(const Quaternion& lhs, const Quaternion& rhs) {
            return {lhs.w / rhs.x + lhs.x / rhs.w + lhs.y / rhs.z - lhs.z / rhs.y,
                    lhs.w / rhs.y + lhs.y / rhs.w + lhs.z / rhs.x - lhs.x / rhs.z,
                    lhs.w / rhs.z + lhs.z / rhs.w + lhs.x / rhs.y - lhs.y / rhs.x,
                    lhs.w / rhs.w - lhs.x / rhs.x - lhs.y / rhs.y - lhs.z / rhs.z};
        }
        inline static Vector3 RotateVectorByQuat(const Quaternion& lhs, const Vector3& rhs);
    };

    inline Quaternion& Quaternion::operator+=(const Quaternion& aQuat) {
        x += aQuat.x;
        y += aQuat.y;
        z += aQuat.z;
        w += aQuat.w;
        return *this;
    }

    inline Quaternion& Quaternion::operator-=(const Quaternion& aQuat) {
        x -= aQuat.x;
        y -= aQuat.y;
        z -= aQuat.z;
        w -= aQuat.w;
        return *this;
    }

    inline Quaternion& Quaternion::operator*=(float aScalar) {
        x *= aScalar;
        y *= aScalar;
        z *= aScalar;
        w *= aScalar;
        return *this;
    }

    inline Quaternion& Quaternion::operator/=(const float aScalar) {
        x /= aScalar;
        y /= aScalar;
        z /= aScalar;
        w /= aScalar;
        return *this;
    }

    inline Quaternion& Quaternion::operator*=(const Quaternion& rhs) {
        float tempx = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
        float tempy = w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z;
        float tempz = w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x;
        float tempw = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
        x = tempx; y = tempy; z = tempz; w = tempw;
        return *this;
    }
    inline Quaternion& Quaternion::operator/=(const Quaternion& rhs) {
        float tempx = w / rhs.x + x / rhs.w + y / rhs.z - z / rhs.y;
        float tempy = w / rhs.y + y / rhs.w + z / rhs.x - x / rhs.z;
        float tempz = w / rhs.z + z / rhs.w + x / rhs.y - y / rhs.x;
        float tempw = w / rhs.w - x / rhs.x - y / rhs.y - z / rhs.z;
        x = tempx; y = tempy; z = tempz; w = tempw;
        return *this;
    }
    inline Vector3 operator*(Quaternion lhs, const Vector3 rhs) {
        Vector3 u = Vector3(lhs.x, lhs.y, lhs.z);
        float s = lhs.w;
        return u * (Vector3::Dot(u, rhs) * 2.0f) + rhs * (s * s - Vector3::Dot(u, u)) + Vector3::Cross(u, rhs) * (2.0f * s);
    }
    Vector3 Quaternion::Up(Quaternion q) { return q * Vector3::up; }
    Vector3 Quaternion::Down(Quaternion q) { return q * Vector3::down; }
    Vector3 Quaternion::Left(Quaternion q) { return q * Vector3::left; }
    Vector3 Quaternion::Right(Quaternion q) { return q * Vector3::right; }
    Vector3 Quaternion::Forward(Quaternion q) { return q * Vector3::forward; }
    Vector3 Quaternion::Back(Quaternion q) { return q * Vector3::back; }

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

    Quaternion Quaternion::FromAngleAxis(float angle, Vector3 axis) {
        Quaternion q;
        float m = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
        float s = sinf(angle / 2) / m;
        q.x = axis.x * s;
        q.y = axis.y * s;
        q.z = axis.z * s;
        q.w = cosf(angle / 2);
        return q;
    }

    Quaternion Quaternion::FromEuler(Vector3 rotation) {
        return FromEuler(rotation.x, rotation.y, rotation.z);
    }

    Quaternion Quaternion::FromEuler(float yaw, float pitch, float roll, bool fromDeg) {
        if (fromDeg) {
            roll *= (float) Deg2Rad;
            pitch *= (float) Deg2Rad;
            yaw *= (float) Deg2Rad;
        }
        float cY(cos(yaw / 2.0f));
        float sY(sin(yaw / 2.0f));

        float cP(cos(pitch / 2.0f));
        float sP(sin(pitch / 2.0f));

        float cR(cos(roll / 2.0f));
        float sR(sin(roll / 2.0f));

        /*
        Simplified multiplication of this data:
          Quaternion qX(sY, 0.0F, 0.0F, cY);
          Quaternion qY(0.0F, sP, 0.0F, cP);
          Quaternion qZ(0.0F, 0.0F, sR, cR);
          (qY * qX) * qZ;
        */
        return {cP * sY * cR + sP * cY * sR,
                sP * cY * cR - cP * sY * sR,
                cP * cY * sR - sP * sY * cR,
                cP * cY * cR + sP * sY * sR};
    }

    Vector3 Quaternion::ToEuler(Quaternion q, bool toDeg) {
        Vector3 rot;

        float xy = q.x * q.y, xw = q.x * q.w;
        float yz = q.y * q.z;
        float zw = q.z * q.w;

        float singularity_test = yz - xw;

        rot.x = -1.f * asin(clamp(2.0f * singularity_test, -1.0f, 1.0f));

        if (abs(singularity_test) < 0.499999f) {
            float xx = q.x * q.x, yy = q.y * q.y, yw = q.y * q.w, zz = q.z * q.z, ww = q.w * q.w;
            rot.y = atan2(2.0f * (q.x * q.z + yw), zz - xx - yy + ww);
            rot.z = atan2(2.0f * (xy + zw), yy - zz - xx + ww);
        } else {
            float a = xy + zw;
            float b = -yz + xw;
            float c = xy - zw;
            float e = yz + xw;
            rot.y = atan2(a * e + b * c, b * e - a * c);
            rot.z = 0;
        }

        if (toDeg) rot *= (float) Rad2Deg;

        return rot;
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

    Quaternion Quaternion::Inverse(Quaternion rotation) {
        float n = Norm(rotation);
        return Conjugate(rotation) / (n * n);
    }

    Quaternion Quaternion::Lerp(Quaternion a, Quaternion b, float t) {
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

    Quaternion Quaternion::LookRotation(Vector3 forward) {
        return LookRotation(forward, Vector3(0, 1, 0));
    }

    Quaternion Quaternion::LookRotation(Vector3 forward, Vector3 upwards) {
        forward = forward.normalized();
        upwards = upwards.normalized();
        if (forward.sqrMagnitude() < SMALL_FLOAT || upwards.sqrMagnitude() < SMALL_FLOAT)
            return Quaternion::Identity();
        if (1 - fabs(Vector3::Dot(forward, upwards)) < SMALL_FLOAT)
            return FromToRotation(Vector3::forward, forward);
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

    Quaternion Quaternion::RotateTowards(Quaternion from, Quaternion to, float maxRadiansDelta) {
        float angle = Quaternion::Angle(from, to);
        if (angle == 0) return to;
        maxRadiansDelta = fmaxf(maxRadiansDelta, angle - (float)M_PI);
        float t = fminf(1, maxRadiansDelta / angle);
        return Quaternion::SlerpUnclamped(from, to, t);
    }

    Quaternion Quaternion::Slerp(Quaternion a, Quaternion b, float t) {
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

    void Quaternion::ToAngleAxis(Quaternion rotation, float &angle, Vector3 &axis) {
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
    inline Vector3 Quaternion::RotateVectorByQuat(const Quaternion& lhs, const Vector3& rhs) {
        float x = lhs.x * 2.f;
        float y = lhs.y * 2.f;
        float z = lhs.z * 2.f;
        float xx = lhs.x * x;
        float yy = lhs.y * y;
        float zz = lhs.z * z;
        float xy = lhs.x * y;
        float xz = lhs.x * z;
        float yz = lhs.y * z;
        float wx = lhs.w * x;
        float wy = lhs.w * y;
        float wz = lhs.w * z;

        Vector3 res;
        res.x = (1.0f - (yy + zz)) * rhs.x + (xy - wz) * rhs.y + (xz + wy) * rhs.z;
        res.y = (xy + wz) * rhs.x + (1.0f - (xx + zz)) * rhs.y + (yz - wx) * rhs.z;
        res.z = (xz - wy) * rhs.x + (yz + wx) * rhs.y + (1.0f - (xx + yy)) * rhs.z;

        return res;
    }
}
#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Matrix3x3.hpp"

#define MAT(m, r, c) (m)[(c)*4+(r)]
#define RETURN_ZERO do { for (int i=0;i<16;i++) out[i] = 0.0F; return false; } while(0)

namespace BNM::Structures::Unity {

    inline void QuaternionToMatrix(const Quaternion& q, struct Matrix4x4& m);

    struct FrustumPlanes {
        float left;
        float right;
        float bottom;
        float top;
        float zNear;
        float zFar;
    };

    inline bool InvertMatrix4x4_Full(const float* m, float* out);

    struct Matrix4x4 {
        float m_Data[16]{};
        enum class InitIdentity { kIdentity };

        Matrix4x4() = default;
        inline Matrix4x4(InitIdentity) { SetIdentity(); }
        inline Matrix4x4(const Matrix3x3 &other) {
            m_Data[0] = other.m_Data[0];
            m_Data[1] = other.m_Data[1];
            m_Data[2] = other.m_Data[2];
            m_Data[3] = 0.0F;

            m_Data[4] = other.m_Data[3];
            m_Data[5] = other.m_Data[4];
            m_Data[6] = other.m_Data[5];
            m_Data[7] = 0.0F;

            m_Data[8] = other.m_Data[6];
            m_Data[9] = other.m_Data[7];
            m_Data[10] = other.m_Data[8];
            m_Data[11] = 0.0F;

            m_Data[12] = 0.0F;
            m_Data[13] = 0.0F;
            m_Data[14] = 0.0F;
            m_Data[15] = 1.0F;
        }
        inline Matrix4x4(const float data[16]) {
            for (int i = 0; i < 16; i++)
            m_Data[i] = data[i];
        }

        float& Get(int row, int column) { return m_Data[row + (column * 4)]; }
        [[nodiscard]] const float& Get(int row, int column) const { return m_Data[row + (column * 4)]; }
        float* GetPtr() { return m_Data; }
        [[nodiscard]] const float* GetPtr() const { return m_Data; }

        float operator[](int index) const     { return m_Data[index]; }
        float& operator[](int index)          { return m_Data[index]; }
        inline static void MultiplyMatrices4x4(const Matrix4x4* __restrict lhs, const Matrix4x4* __restrict rhs, Matrix4x4* __restrict res) {
            for (int i = 0; i < 4; i++) {
                res->m_Data[i] = lhs->m_Data[i] * rhs->m_Data[0] + lhs->m_Data[i + 4] * rhs->m_Data[1] + lhs->m_Data[i + 8] * rhs->m_Data[2] + lhs->m_Data[i + 12] * rhs->m_Data[3];
                res->m_Data[i + 4] = lhs->m_Data[i] * rhs->m_Data[4] + lhs->m_Data[i + 4] * rhs->m_Data[5] + lhs->m_Data[i + 8] * rhs->m_Data[6] + lhs->m_Data[i + 12] * rhs->m_Data[7];
                res->m_Data[i + 8] = lhs->m_Data[i] * rhs->m_Data[8] + lhs->m_Data[i + 4] * rhs->m_Data[9] + lhs->m_Data[i + 8] * rhs->m_Data[10] + lhs->m_Data[i + 12] * rhs->m_Data[11];
                res->m_Data[i + 12] = lhs->m_Data[i] * rhs->m_Data[12] + lhs->m_Data[i + 4] * rhs->m_Data[13] + lhs->m_Data[i + 8] * rhs->m_Data[14] + lhs->m_Data[i + 12] * rhs->m_Data[15];
            }
        }
        inline Matrix4x4& operator*=(const Matrix4x4& inM) {
            Matrix4x4 tmp;
            MultiplyMatrices4x4(this, &inM, &tmp);
            *this = tmp;
            return *this;
        }
        inline Matrix4x4& operator=(const Matrix3x3& other) {
            m_Data[0] = other.m_Data[0];
            m_Data[1] = other.m_Data[1];
            m_Data[2] = other.m_Data[2];
            m_Data[3] = 0.0F;

            m_Data[4] = other.m_Data[3];
            m_Data[5] = other.m_Data[4];
            m_Data[6] = other.m_Data[5];
            m_Data[7] = 0.0F;

            m_Data[8] = other.m_Data[6];
            m_Data[9] = other.m_Data[7];
            m_Data[10] = other.m_Data[8];
            m_Data[11] = 0.0F;

            m_Data[12] = 0.0F;
            m_Data[13] = 0.0F;
            m_Data[14] = 0.0F;
            m_Data[15] = 1.0F;
            return *this;
        }
        [[nodiscard]] inline Vector3 MultiplyVector3(const Vector3& inV) const {
            Vector3 res;
            res.x = m_Data[0] * inV.x + m_Data[4] * inV.y + m_Data[8] * inV.z;
            res.y = m_Data[1] * inV.x + m_Data[5] * inV.y + m_Data[9] * inV.z;
            res.z = m_Data[2] * inV.x + m_Data[6] * inV.y + m_Data[10] * inV.z;
            return res;
        }
        inline void MultiplyVector3(const Vector3& inV, Vector3& output) const {
            output.x = m_Data[0] * inV.x + m_Data[4] * inV.y + m_Data[8] * inV.z;
            output.y = m_Data[1] * inV.x + m_Data[5] * inV.y + m_Data[9] * inV.z;
            output.z = m_Data[2] * inV.x + m_Data[6] * inV.y + m_Data[10] * inV.z;
        }
        inline bool PerspectiveMultiplyVector3(const Vector3& inV, Vector3& output) const {
            Vector3 res;
            float w;
            res.x = Get(0, 0) * inV.x + Get(0, 1) * inV.y + Get(0, 2) * inV.z;
            res.y = Get(1, 0) * inV.x + Get(1, 1) * inV.y + Get(1, 2) * inV.z;
            res.z = Get(2, 0) * inV.x + Get(2, 1) * inV.y + Get(2, 2) * inV.z;
            w     = Get(3, 0) * inV.x + Get(3, 1) * inV.y + Get(3, 2) * inV.z;
            if (abs(w) > 1.0e-7f) {
                float invW = 1.0f / w;
                output.x = res.x * invW;
                output.y = res.y * invW;
                output.z = res.z * invW;
                return true;
            }
            output.x = 0.0f;
            output.y = 0.0f;
            output.z = 0.0f;
            return false;
        }
        [[nodiscard]] inline Vector4 MultiplyVector4(const Vector4& inV) const {
            Vector4 res;
            MultiplyVector4(inV, res);
            return res;
        }
        inline void MultiplyVector4(const Vector4& inV, Vector4& output) const {
            output.x = m_Data[0] * inV.x + m_Data[4] * inV.y + m_Data[8] * inV.z + m_Data[12] * inV.w;
            output.y = m_Data[1] * inV.x + m_Data[5] * inV.y + m_Data[9] * inV.z + m_Data[13] * inV.w;
            output.z = m_Data[2] * inV.x + m_Data[6] * inV.y + m_Data[10] * inV.z + m_Data[14] * inV.w;
            output.w = m_Data[3] * inV.x + m_Data[7] * inV.y + m_Data[11] * inV.z + m_Data[15] * inV.w;
        }
        [[nodiscard]] inline Vector3 MultiplyPoint3(const Vector3& inV) const {
            Vector3 res;
            res.x = m_Data[0] * inV.x + m_Data[4] * inV.y + m_Data[8] * inV.z + m_Data[12];
            res.y = m_Data[1] * inV.x + m_Data[5] * inV.y + m_Data[9] * inV.z + m_Data[13];
            res.z = m_Data[2] * inV.x + m_Data[6] * inV.y + m_Data[10] * inV.z + m_Data[14];
            return res;
        }
        inline void MultiplyPoint3(const Vector3& inV, Vector3& output) const {
            output.x = m_Data[0] * inV.x + m_Data[4] * inV.y + m_Data[8] * inV.z + m_Data[12];
            output.y = m_Data[1] * inV.x + m_Data[5] * inV.y + m_Data[9] * inV.z + m_Data[13];
            output.z = m_Data[2] * inV.x + m_Data[6] * inV.y + m_Data[10] * inV.z + m_Data[14];
        }
        [[nodiscard]] inline Vector2 MultiplyPoint2(const Vector2& inV) const {
            Vector2 res;
            res.x = m_Data[0] * inV.x + m_Data[4] * inV.y + m_Data[12];
            res.y = m_Data[1] * inV.x + m_Data[5] * inV.y + m_Data[13];
            return res;
        }
        inline void MultiplyPoint2(const Vector2& inV, Vector2& output) const {
            output.x = m_Data[0] * inV.x + m_Data[4] * inV.y + m_Data[12];
            output.y = m_Data[1] * inV.x + m_Data[5] * inV.y + m_Data[13];
        }
        inline bool PerspectiveMultiplyPoint3(const Vector3& inV, Vector3& output) const {
            Vector3 res;
            float w;
            res.x = Get(0, 0) * inV.x + Get(0, 1) * inV.y + Get(0, 2) * inV.z + Get(0, 3);
            res.y = Get(1, 0) * inV.x + Get(1, 1) * inV.y + Get(1, 2) * inV.z + Get(1, 3);
            res.z = Get(2, 0) * inV.x + Get(2, 1) * inV.y + Get(2, 2) * inV.z + Get(2, 3);
            w     = Get(3, 0) * inV.x + Get(3, 1) * inV.y + Get(3, 2) * inV.z + Get(3, 3);
            if (abs(w) > 1.0e-7f) {
                float invW = 1.0f / w;
                output.x = res.x * invW;
                output.y = res.y * invW;
                output.z = res.z * invW;
                return true;
            }
            output.x = 0.0f;
            output.y = 0.0f;
            output.z = 0.0f;
            return false;
        }
        [[nodiscard]] inline Vector3 InverseMultiplyPoint3Affine(const Vector3& inV) const {
            Vector3 v(inV.x - Get(0, 3), inV.y - Get(1, 3), inV.z - Get(2, 3));
            Vector3 res;
            res.x = Get(0, 0) * v.x + Get(1, 0) * v.y + Get(2, 0) * v.z;
            res.y = Get(0, 1) * v.x + Get(1, 1) * v.y + Get(2, 1) * v.z;
            res.z = Get(0, 2) * v.x + Get(1, 2) * v.y + Get(2, 2) * v.z;
            return res;
        }
        [[nodiscard]] inline Vector3 InverseMultiplyVector3Affine(const Vector3& inV) const {
            Vector3 res;
            res.x = Get(0, 0) * inV.x + Get(1, 0) * inV.y + Get(2, 0) * inV.z;
            res.y = Get(0, 1) * inV.x + Get(1, 1) * inV.y + Get(2, 1) * inV.z;
            res.z = Get(0, 2) * inV.x + Get(1, 2) * inV.y + Get(2, 2) * inV.z;
            return res;
        }

        [[nodiscard]] inline bool IsIdentity(float threshold = Vector3::kEpsilon) const {
            if (CompareApproximately(Get(0, 0), 1.0f, threshold) && CompareApproximately(Get(0, 1), 0.0f, threshold) && CompareApproximately(Get(0, 2), 0.0f, threshold) && CompareApproximately(Get(0, 3), 0.0f, threshold) &&
                CompareApproximately(Get(1, 0), 0.0f, threshold) && CompareApproximately(Get(1, 1), 1.0f, threshold) && CompareApproximately(Get(1, 2), 0.0f, threshold) && CompareApproximately(Get(1, 3), 0.0f, threshold) &&
                CompareApproximately(Get(2, 0), 0.0f, threshold) && CompareApproximately(Get(2, 1), 0.0f, threshold) && CompareApproximately(Get(2, 2), 1.0f, threshold) && CompareApproximately(Get(2, 3), 0.0f, threshold) &&
                CompareApproximately(Get(3, 0), 0.0f, threshold) && CompareApproximately(Get(3, 1), 0.0f, threshold) && CompareApproximately(Get(3, 2), 0.0f, threshold) && CompareApproximately(Get(3, 3), 1.0f, threshold))
                return true;
            return false;
        }
        [[nodiscard]] bool IsPerspective() const { return (m_Data[3] != 0.0f || m_Data[7] != 0.0f || m_Data[11] != 0.0f || m_Data[15] != 1.0f); }
        [[nodiscard]] float MaxabsScale() const {
            float a = Vector3::SqrMagnitude(GetAxisX());
            a = std::max(a, Vector3::SqrMagnitude(GetAxisY()));
            a = std::max(a, Vector3::SqrMagnitude(GetAxisZ()));
            return sqrtf(a);
        }
        inline void RemoveScale() {
            SetAxisX(Vector3::Normalize(GetAxisX()));
            SetAxisY(Vector3::Normalize(GetAxisY()));
            SetAxisZ(Vector3::Normalize(GetAxisZ()));
        }

        [[nodiscard]] inline float GetDeterminant() const {
            double m00 = Get(0, 0);  double m01 = Get(0, 1);  double m02 = Get(0, 2);  double m03 = Get(0, 3);
            double m10 = Get(1, 0);  double m11 = Get(1, 1);  double m12 = Get(1, 2);  double m13 = Get(1, 3);
            double m20 = Get(2, 0);  double m21 = Get(2, 1);  double m22 = Get(2, 2);  double m23 = Get(2, 3);
            double m30 = Get(3, 0);  double m31 = Get(3, 1);  double m32 = Get(3, 2);  double m33 = Get(3, 3);

            double result =
                    m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30 - m03 * m11 * m22 * m30 + m01 * m13 * m22 * m30 +
                    m02 * m11 * m23 * m30 - m01 * m12 * m23 * m30 - m03 * m12 * m20 * m31 + m02 * m13 * m20 * m31 +
                    m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31 - m02 * m10 * m23 * m31 + m00 * m12 * m23 * m31 +
                    m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32 - m03 * m10 * m21 * m32 + m00 * m13 * m21 * m32 +
                    m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32 - m02 * m11 * m20 * m33 + m01 * m12 * m20 * m33 +
                    m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33 - m01 * m10 * m22 * m33 + m00 * m11 * m22 * m33;
            return (float)result;
        }
        [[nodiscard]] inline float GetDeterminant2x2() const {
            return Get(0,0) * Get(1,1) - Get(0,1) * Get(1,0);
        }

        inline Matrix4x4& Invert_Full() {
            InvertMatrix4x4_Full(m_Data, m_Data);
            return *this;
        }

        inline static bool Invert_Full(const Matrix4x4 &inM, Matrix4x4 &outM) {
            return InvertMatrix4x4_Full(inM.m_Data, outM.m_Data);
        }

        inline static bool Invert_General3D(const Matrix4x4 &in, Matrix4x4 &out) {
            float pos, neg, t;
            float det;

            pos = neg = 0.0;
            t = MAT(in, 0, 0) * MAT(in, 1, 1) * MAT(in, 2, 2);
            if (t >= 0.0) pos += t;
            else neg += t;

            t = MAT(in, 1, 0) * MAT(in, 2, 1) * MAT(in, 0, 2);
            if (t >= 0.0) pos += t;
            else neg += t;

            t = MAT(in, 2, 0) * MAT(in, 0, 1) * MAT(in, 1, 2);
            if (t >= 0.0) pos += t;
            else neg += t;

            t = -MAT(in, 2, 0) * MAT(in, 1, 1) * MAT(in, 0, 2);
            if (t >= 0.0) pos += t;
            else neg += t;

            t = -MAT(in, 1, 0) * MAT(in, 0, 1) * MAT(in, 2, 2);
            if (t >= 0.0) pos += t;
            else neg += t;

            t = -MAT(in, 0, 0) * MAT(in, 2, 1) * MAT(in, 1, 2);
            if (t >= 0.0) pos += t;
            else neg += t;

            det = pos + neg;

            if (det * det < 1e-25)
            RETURN_ZERO;

            det = 1.0F / det;
            MAT(out, 0, 0) = ((MAT(in, 1, 1) * MAT(in, 2, 2) - MAT(in, 2, 1) * MAT(in, 1, 2)) * det);
            MAT(out, 0, 1) = (-(MAT(in, 0, 1) * MAT(in, 2, 2) - MAT(in, 2, 1) * MAT(in, 0, 2)) * det);
            MAT(out, 0, 2) = ((MAT(in, 0, 1) * MAT(in, 1, 2) - MAT(in, 1, 1) * MAT(in, 0, 2)) * det);
            MAT(out, 1, 0) = (-(MAT(in, 1, 0) * MAT(in, 2, 2) - MAT(in, 2, 0) * MAT(in, 1, 2)) * det);
            MAT(out, 1, 1) = ((MAT(in, 0, 0) * MAT(in, 2, 2) - MAT(in, 2, 0) * MAT(in, 0, 2)) * det);
            MAT(out, 1, 2) = (-(MAT(in, 0, 0) * MAT(in, 1, 2) - MAT(in, 1, 0) * MAT(in, 0, 2)) * det);
            MAT(out, 2, 0) = ((MAT(in, 1, 0) * MAT(in, 2, 1) - MAT(in, 2, 0) * MAT(in, 1, 1)) * det);
            MAT(out, 2, 1) = (-(MAT(in, 0, 0) * MAT(in, 2, 1) - MAT(in, 2, 0) * MAT(in, 0, 1)) * det);
            MAT(out, 2, 2) = ((MAT(in, 0, 0) * MAT(in, 1, 1) - MAT(in, 1, 0) * MAT(in, 0, 1)) * det);

            MAT(out, 0, 3) = -(MAT(in, 0, 3) * MAT(out, 0, 0) +
                MAT(in, 1, 3) * MAT(out, 0, 1) +
                MAT(in, 2, 3) * MAT(out, 0, 2));
            MAT(out, 1, 3) = -(MAT(in, 0, 3) * MAT(out, 1, 0) +
                MAT(in, 1, 3) * MAT(out, 1, 1) +
                MAT(in, 2, 3) * MAT(out, 1, 2));
            MAT(out, 2, 3) = -(MAT(in, 0, 3) * MAT(out, 2, 0) +
                MAT(in, 1, 3) * MAT(out, 2, 1) +
                MAT(in, 2, 3) * MAT(out, 2, 2));

            MAT(out, 3, 0) = 0.0f;
            MAT(out, 3, 1) = 0.0f;
            MAT(out, 3, 2) = 0.0f;
            MAT(out, 3, 3) = 1.0f;

            return true;
        }

        inline Matrix4x4& Transpose() {
            std::swap(Get(0, 1), Get(1, 0));
            std::swap(Get(0, 2), Get(2, 0));
            std::swap(Get(0, 3), Get(3, 0));
            std::swap(Get(1, 2), Get(2, 1));
            std::swap(Get(1, 3), Get(3, 1));
            std::swap(Get(2, 3), Get(3, 2));
            return *this;
        }

        inline Matrix4x4& SetIdentity() {
            Get(0, 0) = 1.0;   Get(0, 1) = 0.0;   Get(0, 2) = 0.0;   Get(0, 3) = 0.0;
            Get(1, 0) = 0.0;   Get(1, 1) = 1.0;   Get(1, 2) = 0.0;   Get(1, 3) = 0.0;
            Get(2, 0) = 0.0;   Get(2, 1) = 0.0;   Get(2, 2) = 1.0;   Get(2, 3) = 0.0;
            Get(3, 0) = 0.0;   Get(3, 1) = 0.0;   Get(3, 2) = 0.0;   Get(3, 3) = 1.0;
            return *this;
        }
        inline Matrix4x4& SetZero() {
            Get(0, 0) = 0.0;   Get(0, 1) = 0.0;   Get(0, 2) = 0.0;   Get(0, 3) = 0.0;
            Get(1, 0) = 0.0;   Get(1, 1) = 0.0;   Get(1, 2) = 0.0;   Get(1, 3) = 0.0;
            Get(2, 0) = 0.0;   Get(2, 1) = 0.0;   Get(2, 2) = 0.0;   Get(2, 3) = 0.0;
            Get(3, 0) = 0.0;   Get(3, 1) = 0.0;   Get(3, 2) = 0.0;   Get(3, 3) = 0.0;
            return *this;
        }
        inline Matrix4x4& SetPerspective(float fovy, float aspect, float zNear, float zFar) {
            constexpr float deg2Rad = M_PI / 180.f;

            float cotangent, deltaZ;
            float radians = deg2Rad * (fovy / 2.0f);
            cotangent = cos(radians) / sin(radians);
            deltaZ = zNear - zFar;

            Get(0, 0) = cotangent / aspect; Get(0, 1) = 0.0F;      Get(0, 2) = 0.0F;                    Get(0, 3) = 0.0F;
            Get(1, 0) = 0.0F;               Get(1, 1) = cotangent; Get(1, 2) = 0.0F;                    Get(1, 3) = 0.0F;
            Get(2, 0) = 0.0F;               Get(2, 1) = 0.0F;      Get(2, 2) = (zFar + zNear) / deltaZ; Get(2, 3) = 2.0F * zNear * zFar / deltaZ;
            Get(3, 0) = 0.0F;               Get(3, 1) = 0.0F;      Get(3, 2) = -1.0F;                   Get(3, 3) = 0.0F;

            return *this;
        }
        inline Matrix4x4& SetPerspectiveCotan(float cotanHalfFOV, float zNear, float zFar) {
            float deltaZ = zNear - zFar;

            Get(0, 0) = cotanHalfFOV;          Get(0, 1) = 0.0F;      Get(0, 2) = 0.0F;                    Get(0, 3) = 0.0F;
            Get(1, 0) = 0.0F;               Get(1, 1) = cotanHalfFOV; Get(1, 2) = 0.0F;                    Get(1, 3) = 0.0F;
            Get(2, 0) = 0.0F;               Get(2, 1) = 0.0F;      Get(2, 2) = (zFar + zNear) / deltaZ; Get(2, 3) = 2.0F * zNear * zFar / deltaZ;
            Get(3, 0) = 0.0F;               Get(3, 1) = 0.0F;      Get(3, 2) = -1.0F;                   Get(3, 3) = 0.0F;

            return *this;
        }
        inline Matrix4x4& SetOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
            SetIdentity();

            float deltax = right - left;
            float deltay = top - bottom;
            float deltaz = zFar - zNear;

            Get(0, 0) = 2.0F / deltax;
            Get(0, 3) = -(right + left) / deltax;
            Get(1, 1) = 2.0F / deltay;
            Get(1, 3) = -(top + bottom) / deltay;
            Get(2, 2) = -2.0F / deltaz;
            Get(2, 3) = -(zFar + zNear) / deltaz;
            return *this;
        }
        inline Matrix4x4& SetFrustum(float left, float right, float bottom, float top, float nearval, float farval) {
            float x, y, a, b, c, d, e;

            x =  (2.0F * nearval)       / (right - left);
            y =  (2.0F * nearval)       / (top - bottom);
            a =  (right + left)         / (right - left);
            b =  (top + bottom)         / (top - bottom);
            c = -(farval + nearval)        / (farval - nearval);
            d = -(2.0f * farval * nearval) / (farval - nearval);
            e = -1.0f;

            Get(0, 0) = x;    Get(0, 1) = 0.0;  Get(0, 2) = a;   Get(0, 3) = 0.0;
            Get(1, 0) = 0.0;  Get(1, 1) = y;    Get(1, 2) = b;   Get(1, 3) = 0.0;
            Get(2, 0) = 0.0;  Get(2, 1) = 0.0;  Get(2, 2) = c;   Get(2, 3) = d;
            Get(3, 0) = 0.0;  Get(3, 1) = 0.0;  Get(3, 2) = e;  Get(3, 3) = 0.0;
            return *this;
        }
        inline Matrix4x4& AdjustDepthRange(float origNear, float newNear, float newFar) {
            if (IsPerspective()) {
                float x = Get(0, 0);
                float y = Get(1, 1);
                float w = Get(0, 2);
                float z = Get(1, 2);

                float r = ((2.0f * origNear) / x) * ((w + 1) * 0.5f);
                float t = ((2.0f * origNear) / y) * ((z + 1) * 0.5f);
                float l = ((2.0f * origNear) / x) * (((w + 1) * 0.5f) - 1.0f);
                float b = ((2.0f * origNear) / y) * (((z + 1) * 0.5f) - 1.0f);

                float ratio = (newNear / origNear);

                r *= ratio;
                t *= ratio;
                l *= ratio;
                b *= ratio;

                return SetFrustum(l, r, b, t, newNear, newFar);
            }
            float deltaz = newFar - newNear;
            Get(2, 2) = -2.0F / deltaz;
            Get(2, 3) = -(newFar + newNear) / deltaz;
            return *this;
        }
        [[nodiscard]] inline Vector3 GetAxisX() const { return {Get(0, 0), Get(1, 0), Get(2, 0)}; }
        [[nodiscard]] inline Vector3 GetAxisY() const { return {Get(0, 1), Get(1, 1), Get(2, 1)}; }
        [[nodiscard]] inline Vector3 GetAxisZ() const { return {Get(0, 2), Get(1, 2), Get(2, 2)}; }
        [[nodiscard]] inline Vector3 GetAxis(int axis) const { return {Get(0, axis), Get(1, axis), Get(2, axis)}; }
        [[nodiscard]] inline Vector3 GetPosition() const { return {Get(0, 3), Get(1, 3), Get(2, 3)}; }
        [[nodiscard]] inline Vector3 GetLossyScale() const {
            Vector3 result;
            result.x = Vector3::Magnitude(GetAxisX());
            result.y = Vector3::Magnitude(GetAxisY());
            result.z = Vector3::Magnitude(GetAxisZ());
            float determinant = Matrix3x3(*this).GetDeterminant();
            if (determinant < 0) result.x *= -1;
            return result;
        }
        [[nodiscard]] inline Vector4 GetRow(int row) const { return {Get(row, 0), Get(row, 1), Get(row, 2), Get(row, 3)}; }
        [[nodiscard]] inline Vector4 GetColumn(int col) const { return {Get(0, col), Get(1, col), Get(2, col), Get(3, col)}; }
        inline void SetAxisX(const Vector3& v) { Get(0, 0) = v.x; Get(1, 0) = v.y; Get(2, 0) = v.z; }
        inline void SetAxisY(const Vector3& v) { Get(0, 1) = v.x; Get(1, 1) = v.y; Get(2, 1) = v.z; }
        inline void SetAxisZ(const Vector3& v) { Get(0, 2) = v.x; Get(1, 2) = v.y; Get(2, 2) = v.z; }
        inline void SetAxis(int axis, const Vector3& v) { Get(0, axis) = v.x; Get(1, axis) = v.y; Get(2, axis) = v.z; }
        inline void SetPosition(const Vector3& v) { Get(0, 3) = v.x; Get(1, 3) = v.y; Get(2, 3) = v.z; }
        inline void SetRow(int row, const Vector4& v) { Get(row, 0) = v.x; Get(row, 1) = v.y; Get(row, 2) = v.z; Get(row, 3) = v.w; }
        inline void SetColumn(int col, const Vector4& v) { Get(0, col) = v.x; Get(1, col) = v.y; Get(2, col) = v.z; Get(3, col) = v.w; }
        inline Matrix4x4& SetTranslate(const Vector3& inTrans) {
            Get(0, 0) = 1.0;   Get(0, 1) = 0.0;   Get(0, 2) = 0.0;   Get(0, 3) = inTrans.x;
            Get(1, 0) = 0.0;   Get(1, 1) = 1.0;   Get(1, 2) = 0.0;   Get(1, 3) = inTrans.y;
            Get(2, 0) = 0.0;   Get(2, 1) = 0.0;   Get(2, 2) = 1.0;   Get(2, 3) = inTrans.z;
            Get(3, 0) = 0.0;   Get(3, 1) = 0.0;   Get(3, 2) = 0.0;   Get(3, 3) = 1.0;
            return *this;
        }
        inline Matrix4x4& SetBasis(const Vector3& inX, const Vector3& inY, const Vector3& inZ) {
            Get(0, 0) = inX[0];    Get(0, 1) = inY[0];    Get(0, 2) = inZ[0];    Get(0, 3) = 0.0;
            Get(1, 0) = inX[1];    Get(1, 1) = inY[1];    Get(1, 2) = inZ[1];    Get(1, 3) = 0.0;
            Get(2, 0) = inX[2];    Get(2, 1) = inY[2];    Get(2, 2) = inZ[2];    Get(2, 3) = 0.0;
            Get(3, 0) = 0.0;       Get(3, 1) = 0.0;       Get(3, 2) = 0.0;       Get(3, 3) = 1.0;
            return *this;
        }
        inline Matrix4x4& SetBasisTransposed(const Vector3& inX, const Vector3& inY, const Vector3& inZ) {
            Get(0, 0) = inX[0];    Get(1, 0) = inY[0];    Get(2, 0) = inZ[0];    Get(3, 0) = 0.0;
            Get(0, 1) = inX[1];    Get(1, 1) = inY[1];    Get(2, 1) = inZ[1];    Get(3, 1) = 0.0;
            Get(0, 2) = inX[2];    Get(1, 2) = inY[2];    Get(2, 2) = inZ[2];    Get(3, 2) = 0.0;
            Get(0, 3) = 0.0;       Get(1, 3) = 0.0;       Get(2, 3) = 0.0;       Get(3, 3) = 1.0;
            return *this;
        }
        inline Matrix4x4& SetScale(const Vector3& inScale){
            Get(0, 0) = inScale.x;    Get(0, 1) = 0.0;       Get(0, 2) = 0.0;       Get(0, 3) = 0.0;
            Get(1, 0) = 0.0;       Get(1, 1) = inScale.y;    Get(1, 2) = 0.0;       Get(1, 3) = 0.0;
            Get(2, 0) = 0.0;       Get(2, 1) = 0.0;       Get(2, 2) = inScale.z;    Get(2, 3) = 0.0;
            Get(3, 0) = 0.0;       Get(3, 1) = 0.0;       Get(3, 2) = 0.0;       Get(3, 3) = 1.0;
            return *this;
        }
        inline Matrix4x4& SetScaleAndPosition(const Vector3& inScale, const Vector3& inPosition) {
            Get(0, 0) = inScale[0];    Get(0, 1) = 0.0;           Get(0, 2) = 0.0;           Get(0, 3) = inPosition[0];
            Get(1, 0) = 0.0;           Get(1, 1) = inScale[1];    Get(1, 2) = 0.0;           Get(1, 3) = inPosition[1];
            Get(2, 0) = 0.0;           Get(2, 1) = 0.0;           Get(2, 2) = inScale[2];    Get(2, 3) = inPosition[2];
            Get(3, 0) = 0.0;           Get(3, 1) = 0.0;           Get(3, 2) = 0.0;           Get(3, 3) = 1.0;
            return *this;
        }
        inline Matrix4x4& SetPositionAndOrthoNormalBasis(const Vector3& inPosition, const Vector3& inX, const Vector3& inY, const Vector3& inZ) {
            Get(0, 0) = inX[0];    Get(0, 1) = inY[0];    Get(0, 2) = inZ[0];    Get(0, 3) = inPosition[0];
            Get(1, 0) = inX[1];    Get(1, 1) = inY[1];    Get(1, 2) = inZ[1];    Get(1, 3) = inPosition[1];
            Get(2, 0) = inX[2];    Get(2, 1) = inY[2];    Get(2, 2) = inZ[2];    Get(2, 3) = inPosition[2];
            Get(3, 0) = 0.0;       Get(3, 1) = 0.0;       Get(3, 2) = 0.0;       Get(3, 3) = 1.0;
            return *this;
        }
        inline Matrix4x4& Translate(const Vector3& inTrans) {
            Get(0, 3) = Get(0, 0) * inTrans[0] + Get(0, 1) * inTrans[1] + Get(0, 2) * inTrans[2] + Get(0, 3);
            Get(1, 3) = Get(1, 0) * inTrans[0] + Get(1, 1) * inTrans[1] + Get(1, 2) * inTrans[2] + Get(1, 3);
            Get(2, 3) = Get(2, 0) * inTrans[0] + Get(2, 1) * inTrans[1] + Get(2, 2) * inTrans[2] + Get(2, 3);
            Get(3, 3) = Get(3, 0) * inTrans[0] + Get(3, 1) * inTrans[1] + Get(3, 2) * inTrans[2] + Get(3, 3);
            return *this;
        }
        inline Matrix4x4& Scale(const Vector3& inScale) {
            Get(0, 0) *= inScale[0];
            Get(1, 0) *= inScale[0];
            Get(2, 0) *= inScale[0];
            Get(3, 0) *= inScale[0];

            Get(0, 1) *= inScale[1];
            Get(1, 1) *= inScale[1];
            Get(2, 1) *= inScale[1];
            Get(3, 1) *= inScale[1];

            Get(0, 2) *= inScale[2];
            Get(1, 2) *= inScale[2];
            Get(2, 2) *= inScale[2];
            Get(3, 2) *= inScale[2];
            return *this;
        }

        inline Matrix4x4& SetFromToRotation(const Vector3& from, const Vector3& to) {
            Matrix3x3 mat;
            mat.SetFromToRotation(from, to);
            *this = mat;
            return *this;
        }

        inline void SetTR(const Vector3& pos, const Quaternion& q) {
            QuaternionToMatrix(q, *this);
            m_Data[12] = pos[0];
            m_Data[13] = pos[1];
            m_Data[14] = pos[2];
        }
        inline void SetTRS(const Vector3& pos, const Quaternion& q, const Vector3& s) {
            QuaternionToMatrix(q, *this);

            m_Data[0] *= s[0];
            m_Data[1] *= s[0];
            m_Data[2] *= s[0];

            m_Data[4] *= s[1];
            m_Data[5] *= s[1];
            m_Data[6] *= s[1];

            m_Data[8] *= s[2];
            m_Data[9] *= s[2];
            m_Data[10] *= s[2];

            m_Data[12] = pos[0];
            m_Data[13] = pos[1];
            m_Data[14] = pos[2];
        }
        inline void SetTRInverse(const Vector3& pos, const Quaternion& q) {
            QuaternionToMatrix(Quaternion::Inverse(q), *this);
            Translate(Vector3(-pos[0], -pos[1], -pos[2]));
        }
        [[nodiscard]] FrustumPlanes DecomposeProjection() const {
            FrustumPlanes planes{};
            if (IsPerspective()) {
                planes.zNear = Get(2, 3) / (Get(2, 2) - 1.0f);
                planes.zFar = Get(2, 3) / (Get(2, 2) + 1.0f);
                planes.right = planes.zNear * (1.0f + Get(0, 2)) / Get(0, 0);
                planes.left = planes.zNear * (-1.0f + Get(0, 2)) / Get(0, 0);
                planes.top = planes.zNear  * (1.0f + Get(1, 2)) / Get(1, 1);
                planes.bottom = planes.zNear  * (-1.0f + Get(1, 2)) / Get(1, 1);
                return planes;
            }
            planes.zNear = (Get(2, 3) + 1.0f) / Get(2, 2);
            planes.zFar =  (Get(2, 3) - 1.0f) / Get(2, 2);
            planes.right = (1.0f - Get(0, 3)) / Get(0, 0);
            planes.left = (-1.0f - Get(0, 3)) / Get(0, 0);
            planes.top = (1.0f - Get(1, 3)) / Get(1, 1);
            planes.bottom = (-1.0f - Get(1, 3)) / Get(1, 1);
            return planes;
        }
        static const Matrix4x4 identity;
        [[nodiscard]] inline bool ValidTRS() const {
            return Get(3, 0) == 0 && Get(3, 1) == 0 && Get(3, 2) == 0 && fabs(Get(3, 3)) == 1;
        }
        inline static Matrix4x4 Rotate(Quaternion q) { // Из C#
            float x = q.x * 2.0f;
            float y = q.y * 2.0f;
            float z = q.z * 2.0f;
            float xx = q.x * x;
            float yy = q.y * y;
            float zz = q.z * z;
            float xy = q.x * y;
            float xz = q.x * z;
            float yz = q.y * z;
            float wx = q.w * x;
            float wy = q.w * y;
            float wz = q.w * z;

            Matrix4x4 m;
            m.m_Data[0] = 1.0f - (yy + zz); m.m_Data[1] = xy + wz; m.m_Data[2] = xz - wy; m.m_Data[3] = 0.0f;
            m.m_Data[4] = xy - wz; m.m_Data[5] = 1.0f - (xx + zz); m.m_Data[6] = yz + wx; m.m_Data[7] = 0.0f;
            m.m_Data[8] = xz + wy; m.m_Data[9] = yz - wx; m.m_Data[10] = 1.0f - (xx + yy); m.m_Data[11] = 0.0f;
            m.m_Data[12] = 0.0f; m.m_Data[13] = 0.0f; m.m_Data[14] = 0.0F; m.m_Data[15] = 1.0f;
            return m;
        }
    };
    inline Matrix4x4 operator *(const Matrix4x4 &lhs, const Matrix4x4 &rhs) {
        Matrix4x4 res;
        res.m_Data[0] = lhs.m_Data[0] * rhs.m_Data[0] + lhs.m_Data[4] * rhs.m_Data[1] + lhs.m_Data[8] * rhs.m_Data[2] + lhs.m_Data[12] * rhs.m_Data[3];
        res.m_Data[4] = lhs.m_Data[0] * rhs.m_Data[4] + lhs.m_Data[4] * rhs.m_Data[5] + lhs.m_Data[8] * rhs.m_Data[6] + lhs.m_Data[12] * rhs.m_Data[7];
        res.m_Data[8] = lhs.m_Data[0] * rhs.m_Data[8] + lhs.m_Data[4] * rhs.m_Data[9] + lhs.m_Data[8] * rhs.m_Data[10] + lhs.m_Data[12] * rhs.m_Data[11];
        res.m_Data[12] = lhs.m_Data[0] * rhs.m_Data[12] + lhs.m_Data[4] * rhs.m_Data[13] + lhs.m_Data[8] * rhs.m_Data[14] + lhs.m_Data[12] * rhs.m_Data[15];
        res.m_Data[1] = lhs.m_Data[1] * rhs.m_Data[0] + lhs.m_Data[5] * rhs.m_Data[1] + lhs.m_Data[9] * rhs.m_Data[2] + lhs.m_Data[13] * rhs.m_Data[3];
        res.m_Data[5] = lhs.m_Data[1] * rhs.m_Data[4] + lhs.m_Data[5] * rhs.m_Data[5] + lhs.m_Data[9] * rhs.m_Data[6] + lhs.m_Data[13] * rhs.m_Data[7];
        res.m_Data[9] = lhs.m_Data[1] * rhs.m_Data[8] + lhs.m_Data[5] * rhs.m_Data[9] + lhs.m_Data[9] * rhs.m_Data[10] + lhs.m_Data[13] * rhs.m_Data[11];
        res.m_Data[13] = lhs.m_Data[1] * rhs.m_Data[12] + lhs.m_Data[5] * rhs.m_Data[13] + lhs.m_Data[9] * rhs.m_Data[14] + lhs.m_Data[13] * rhs.m_Data[15];
        res.m_Data[2] = lhs.m_Data[2] * rhs.m_Data[0] + lhs.m_Data[6] * rhs.m_Data[1] + lhs.m_Data[10] * rhs.m_Data[2] + lhs.m_Data[14] * rhs.m_Data[3];
        res.m_Data[6] = lhs.m_Data[2] * rhs.m_Data[4] + lhs.m_Data[6] * rhs.m_Data[5] + lhs.m_Data[10] * rhs.m_Data[6] + lhs.m_Data[14] * rhs.m_Data[7];
        res.m_Data[10] = lhs.m_Data[2] * rhs.m_Data[8] + lhs.m_Data[6] * rhs.m_Data[9] + lhs.m_Data[10] * rhs.m_Data[10] + lhs.m_Data[14] * rhs.m_Data[11];
        res.m_Data[14] = lhs.m_Data[2] * rhs.m_Data[12] + lhs.m_Data[6] * rhs.m_Data[13] + lhs.m_Data[10] * rhs.m_Data[14] + lhs.m_Data[14] * rhs.m_Data[15];
        res.m_Data[3] = lhs.m_Data[3] * rhs.m_Data[0] + lhs.m_Data[7] * rhs.m_Data[1] + lhs.m_Data[11] * rhs.m_Data[2] + lhs.m_Data[15] * rhs.m_Data[3];
        res.m_Data[7] = lhs.m_Data[3] * rhs.m_Data[4] + lhs.m_Data[7] * rhs.m_Data[5] + lhs.m_Data[11] * rhs.m_Data[6] + lhs.m_Data[15] * rhs.m_Data[7];
        res.m_Data[11] = lhs.m_Data[3] * rhs.m_Data[8] + lhs.m_Data[7] * rhs.m_Data[9] + lhs.m_Data[11] * rhs.m_Data[10] + lhs.m_Data[15] * rhs.m_Data[11];
        res.m_Data[15] = lhs.m_Data[3] * rhs.m_Data[12] + lhs.m_Data[7] * rhs.m_Data[13] + lhs.m_Data[11] * rhs.m_Data[14] + lhs.m_Data[15] * rhs.m_Data[15];
        return res;
    }
    inline void QuaternionToMatrix(const Quaternion& q, Matrix4x4& m) {
        float x = q.x * 2.0F;
        float y = q.y * 2.0F;
        float z = q.z * 2.0F;
        float xx = q.x * x;
        float yy = q.y * y;
        float zz = q.z * z;
        float xy = q.x * y;
        float xz = q.x * z;
        float yz = q.y * z;
        float wx = q.w * x;
        float wy = q.w * y;
        float wz = q.w * z;

        m.m_Data[0] = 1.0f - (yy + zz);
        m.m_Data[1] = xy + wz;
        m.m_Data[2] = xz - wy;
        m.m_Data[3] = 0.0F;

        m.m_Data[4] = xy - wz;
        m.m_Data[5] = 1.0f - (xx + zz);
        m.m_Data[6] = yz + wx;
        m.m_Data[7] = 0.0F;

        m.m_Data[8]  = xz + wy;
        m.m_Data[9]  = yz - wx;
        m.m_Data[10] = 1.0f - (xx + yy);
        m.m_Data[11] = 0.0F;

        m.m_Data[12] = 0.0F;
        m.m_Data[13] = 0.0F;
        m.m_Data[14] = 0.0F;
        m.m_Data[15] = 1.0F;
    }

    inline void MultiplyMatrices3x4(const Matrix4x4& lhs, const Matrix4x4& rhs, Matrix4x4& res) {
        for (int i = 0; i < 3; i++) {
            res.m_Data[i]    = lhs.m_Data[i] * rhs.m_Data[0]  + lhs.m_Data[i + 4] * rhs.m_Data[1]  + lhs.m_Data[i + 8] * rhs.m_Data[2];//  + lhs.m_Data[i+12] * rhs.m_Data[3];
            res.m_Data[i + 4]  = lhs.m_Data[i] * rhs.m_Data[4]  + lhs.m_Data[i + 4] * rhs.m_Data[5]  + lhs.m_Data[i + 8] * rhs.m_Data[6];//  + lhs.m_Data[i+12] * rhs.m_Data[7];
            res.m_Data[i + 8]  = lhs.m_Data[i] * rhs.m_Data[8]  + lhs.m_Data[i + 4] * rhs.m_Data[9]  + lhs.m_Data[i + 8] * rhs.m_Data[10];// + lhs.m_Data[i+12] * rhs.m_Data[11];
            res.m_Data[i + 12] = lhs.m_Data[i] * rhs.m_Data[12] + lhs.m_Data[i + 4] * rhs.m_Data[13] + lhs.m_Data[i + 8] * rhs.m_Data[14] + lhs.m_Data[i + 12];// * rhs.m_Data[15];
        }

        res.m_Data[3]  = 0.0f;
        res.m_Data[7]  = 0.0f;
        res.m_Data[11] = 0.0f;
        res.m_Data[15] = 1.0f;
    }

    inline void MultiplyMatrices2D(const Matrix4x4& lhs, const Matrix4x4& rhs, Matrix4x4& res) {
        res.m_Data[0] = lhs.m_Data[0] * rhs.m_Data[0]  + lhs.m_Data[0 + 4] * rhs.m_Data[1]  + lhs.m_Data[0 + 8] * rhs.m_Data[2];
        res.m_Data[4] = lhs.m_Data[0] * rhs.m_Data[4]  + lhs.m_Data[0 + 4] * rhs.m_Data[5]  + lhs.m_Data[0 + 8] * rhs.m_Data[6];
        res.m_Data[12] = lhs.m_Data[0] * rhs.m_Data[12] + lhs.m_Data[0 + 4] * rhs.m_Data[13] + lhs.m_Data[0 + 8] * rhs.m_Data[14] + lhs.m_Data[0 + 12];
        res.m_Data[1]    = lhs.m_Data[1] * rhs.m_Data[0]  + lhs.m_Data[1 + 4] * rhs.m_Data[1]  + lhs.m_Data[1 + 8] * rhs.m_Data[2];
        res.m_Data[5]  = lhs.m_Data[1] * rhs.m_Data[4]  + lhs.m_Data[1 + 4] * rhs.m_Data[5]  + lhs.m_Data[1 + 8] * rhs.m_Data[6];
        res.m_Data[13] = lhs.m_Data[1] * rhs.m_Data[12] + lhs.m_Data[1 + 4] * rhs.m_Data[13] + lhs.m_Data[1 + 8] * rhs.m_Data[14] + lhs.m_Data[1 + 12];
        res.m_Data[2]    = lhs.m_Data[2] * rhs.m_Data[0]  + lhs.m_Data[2 + 4] * rhs.m_Data[1]  + lhs.m_Data[2 + 8] * rhs.m_Data[2];
        res.m_Data[14] = lhs.m_Data[2] * rhs.m_Data[12] + lhs.m_Data[2 + 4] * rhs.m_Data[13] + lhs.m_Data[2 + 8] * rhs.m_Data[14] + lhs.m_Data[2 + 12];
        res.m_Data[3]  = 0;
        res.m_Data[6]  = 0;
        res.m_Data[7]  = 0;
        res.m_Data[8]  = 0;
        res.m_Data[9]  = 0;
        res.m_Data[10]  = 1.0f;
        res.m_Data[11] = 0;
        res.m_Data[15] = 1.0f;
    }

   inline float ComputeUniformScale(const Matrix4x4& matrix) { return Vector3::Magnitude(matrix.GetAxisX()); }

#define SWAP_ROWS(a, b) do { float *_tmp = a; (a)=(b); (b)=_tmp; } while(false)
   inline bool InvertMatrix4x4_Full(const float* m, float* out) {
        float wtmp[4][8];
        float m0, m1, m2, m3, s;
        float *r0, *r1, *r2, *r3;

        r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

        r0[0] = MAT(m, 0, 0); r0[1] = MAT(m, 0, 1);
        r0[2] = MAT(m, 0, 2); r0[3] = MAT(m, 0, 3);
        r0[4] = 1.0; r0[5] = r0[6] = r0[7] = 0.0;

        r1[0] = MAT(m, 1, 0); r1[1] = MAT(m, 1, 1);
        r1[2] = MAT(m, 1, 2); r1[3] = MAT(m, 1, 3);
        r1[5] = 1.0; r1[4] = r1[6] = r1[7] = 0.0;

        r2[0] = MAT(m, 2, 0); r2[1] = MAT(m, 2, 1);
        r2[2] = MAT(m, 2, 2); r2[3] = MAT(m, 2, 3);
        r2[6] = 1.0; r2[4] = r2[5] = r2[7] = 0.0;

        r3[0] = MAT(m, 3, 0); r3[1] = MAT(m, 3, 1);
        r3[2] = MAT(m, 3, 2); r3[3] = MAT(m, 3, 3);
        r3[7] = 1.0; r3[4] = r3[5] = r3[6] = 0.0;

        if (abs(r3[0]) > abs(r2[0])) SWAP_ROWS(r3, r2);
        if (abs(r2[0]) > abs(r1[0])) SWAP_ROWS(r2, r1);
        if (abs(r1[0]) > abs(r0[0])) SWAP_ROWS(r1, r0);
        if (0.0F == r0[0]) RETURN_ZERO;

        m1 = r1[0] / r0[0]; m2 = r2[0] / r0[0]; m3 = r3[0] / r0[0];
        s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
        s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
        s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
        s = r0[4]; if (s != 0.0F) r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s;
        s = r0[5]; if (s != 0.0F) r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s;
        s = r0[6]; if (s != 0.0F) r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s;
        s = r0[7]; if (s != 0.0F) r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s;
        if (abs(r3[1]) > abs(r2[1])) SWAP_ROWS(r3, r2);
        if (abs(r2[1]) > abs(r1[1])) SWAP_ROWS(r2, r1);
        if (0.0F == r1[1]) RETURN_ZERO;
        m2 = r2[1] / r1[1]; m3 = r3[1] / r1[1];
        r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
        r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
        s = r1[4]; if (0.0F != s) r2[4] -= m2 * s; r3[4] -= m3 * s;
        s = r1[5]; if (0.0F != s) r2[5] -= m2 * s; r3[5] -= m3 * s;
        s = r1[6]; if (0.0F != s) r2[6] -= m2 * s; r3[6] -= m3 * s;
        s = r1[7]; if (0.0F != s) r2[7] -= m2 * s; r3[7] -= m3 * s;
        if (abs(r3[2]) > abs(r2[2])) SWAP_ROWS(r3, r2);
        if (0.0F == r2[2]) RETURN_ZERO;
        m3 = r3[2] / r2[2];
        r3[3] -= m3 * r2[3]; r3[4] -= m3 * r2[4];
        r3[5] -= m3 * r2[5]; r3[6] -= m3 * r2[6];
        r3[7] -= m3 * r2[7];
        if (0.0F == r3[3]) RETURN_ZERO;
        s = 1.0F / r3[3];
        r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;
        m2 = r2[3];
        s  = 1.0F / r2[2];
        r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
        r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
        m1 = r1[3];
        r1[4] -= r3[4] * m1; r1[5] -= r3[5] * m1, r1[6] -= r3[6] * m1; r1[7] -= r3[7] * m1;
        m0 = r0[3];
        r0[4] -= r3[4] * m0; r0[5] -= r3[5] * m0, r0[6] -= r3[6] * m0; r0[7] -= r3[7] * m0;
        m1 = r1[2];
        s  = 1.0F / r1[1];
        r1[4] = s * (r1[4] - r2[4] * m1); r1[5] = s * (r1[5] - r2[5] * m1), r1[6] = s * (r1[6] - r2[6] * m1); r1[7] = s * (r1[7] - r2[7] * m1);
        m0 = r0[2];
        r0[4] -= r2[4] * m0; r0[5] -= r2[5] * m0, r0[6] -= r2[6] * m0; r0[7] -= r2[7] * m0;
        m0 = r0[1];
        s  = 1.0F / r0[0];
        r0[4] = s * (r0[4] - r1[4] * m0); r0[5] = s * (r0[5] - r1[5] * m0), r0[6] = s * (r0[6] - r1[6] * m0); r0[7] = s * (r0[7] - r1[7] * m0);
        MAT(out, 0, 0) = r0[4]; MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6]; MAT(out, 0, 3) = r0[7];
        MAT(out, 1, 0) = r1[4]; MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6]; MAT(out, 1, 3) = r1[7];
        MAT(out, 2, 0) = r2[4]; MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6]; MAT(out, 2, 3) = r2[7];
        MAT(out, 3, 0) = r3[4]; MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6]; MAT(out, 3, 3) = r3[7];
        return true;
    }

#undef SWAP_ROWS
#undef MAT
#undef RETURN_ZERO

    template<class T>
    inline T* Stride(T* p, size_t offset) { return reinterpret_cast<T*>((char*)p + offset); }

    inline bool CompareApproximately(const Matrix4x4& lhs, const Matrix4x4& rhs, float dist) {
        for (int i = 0; i < 16; i++) if (!CompareApproximately(lhs[i], rhs[i], dist)) return false;
        return true;
    }
    inline void TransformPoints3x3(const Matrix4x4& matrix, const Vector3* in, Vector3* out, int count) {
        auto m = Matrix3x3(matrix);
        for (int i = 0; i < count; i++) out[i] = m.MultiplyPoint3(in[i]);
    }
    inline void TransformPoints3x4(const Matrix4x4& matrix, const Vector3* in, Vector3* out, int count) {
        for (int i = 0; i < count; i++) out[i] = matrix.MultiplyPoint3(in[i]);
    }
    inline void TransformPoints3x3(const Matrix4x4& matrix, const Vector3* in, size_t inStride, Vector3* out, size_t outStride, int count) {
        auto m = Matrix3x3(matrix);
        for (int i = 0; i < count; ++i, in = Stride(in, inStride), out = Stride(out, outStride)) *out = m.MultiplyPoint3(*in);
    }
    inline void TransformPoints3x4(const Matrix4x4& matrix, const Vector3* in, size_t inStride, Vector3* out, size_t outStride, int count) {
        for (int i = 0; i < count; ++i, in = Stride(in, inStride), out = Stride(out, outStride)) *out = matrix.MultiplyPoint3(*in);
    }
}
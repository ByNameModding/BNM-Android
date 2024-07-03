#pragma once

namespace BNM::Structures::Unity {
    struct Matrix4x4;

    inline static bool CompareApproximately(float f0, float f1, float epsilon = 0.000001f) {
        float dist = (f0 - f1);
        dist = abs(dist);
        return dist <= epsilon;
    }

    struct Matrix3x3 {
        float m_Data[9]{};
        inline Matrix3x3() = default;
        inline Matrix3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) { Get(0, 0) = m00; Get(1, 0) = m10; Get(2, 0) = m20; Get(0, 1) = m01; Get(1, 1) = m11; Get(2, 1) = m21; Get(0, 2) = m02; Get(1, 2) = m12; Get(2, 2) = m22; }
        Matrix3x3(const class Matrix4x4& m);

        inline float& Get(int row, int column) { return m_Data[row + (column * 3)]; }
        [[nodiscard]] inline const float& Get(int row, int column) const { return m_Data[row + (column * 3)]; }

        inline float& operator[](int row) { return m_Data[row]; }
        inline float operator[](int row) const { return m_Data[row]; }

        inline float* GetPtr() { return m_Data; }
        [[nodiscard]] inline const float* GetPtr() const { return m_Data; }

        [[nodiscard]] inline Vector3 GetColumn(int col) const { return {Get(0, col), Get(1, col), Get(2, col)}; }

        inline static void MultiplyMatrices3x3(const Matrix3x3* __restrict lhs, const Matrix3x3* __restrict rhs, Matrix3x3* __restrict res) {
            for (int i = 0; i < 3; ++i) {
                res->m_Data[i]    = lhs->m_Data[i] * rhs->m_Data[0]  + lhs->m_Data[i + 3] * rhs->m_Data[1]  + lhs->m_Data[i + 6] * rhs->m_Data[2];
                res->m_Data[i + 3]  = lhs->m_Data[i] * rhs->m_Data[3]  + lhs->m_Data[i + 3] * rhs->m_Data[4]  + lhs->m_Data[i + 6] * rhs->m_Data[5];
                res->m_Data[i + 6]  = lhs->m_Data[i] * rhs->m_Data[6]  + lhs->m_Data[i + 3] * rhs->m_Data[7]  + lhs->m_Data[i + 6] * rhs->m_Data[8];
            }
        }
        inline friend Matrix3x3 operator*(const Matrix3x3& lhs, const Matrix3x3& rhs) {
            Matrix3x3 temp;
            MultiplyMatrices3x3(&lhs, &rhs, &temp);
            return temp;
        }

        [[nodiscard]] inline Vector3 MultiplyVector3(const Vector3& inV) const {
            Vector3 res;
            res.x = m_Data[0] * inV.x + m_Data[3] * inV.y + m_Data[6] * inV.z;
            res.y = m_Data[1] * inV.x + m_Data[4] * inV.y + m_Data[7] * inV.z;
            res.z = m_Data[2] * inV.x + m_Data[5] * inV.y + m_Data[8] * inV.z;
            return res;
        }
        inline void MultiplyVector3(const Vector3& inV, Vector3& output) const {
            output.x = m_Data[0] * inV.x + m_Data[3] * inV.y + m_Data[6] * inV.z;
            output.y = m_Data[1] * inV.x + m_Data[4] * inV.y + m_Data[7] * inV.z;
            output.z = m_Data[2] * inV.x + m_Data[5] * inV.y + m_Data[8] * inV.z;
        }
        [[nodiscard]] inline Vector3 MultiplyPoint3(const Vector3& inV) const { return MultiplyVector3(inV); }
        [[nodiscard]] inline Vector3 MultiplyVector3Transpose(const Vector3& inV)  const {
            Vector3 res;
            res.x = Get(0, 0) * inV.x + Get(1, 0) * inV.y + Get(2, 0) * inV.z;
            res.y = Get(0, 1) * inV.x + Get(1, 1) * inV.y + Get(2, 1) * inV.z;
            res.z = Get(0, 2) * inV.x + Get(1, 2) * inV.y + Get(2, 2) * inV.z;
            return res;
        }
        [[nodiscard]] inline Vector3 MultiplyPoint3Transpose(const Vector3& inV) const { return MultiplyVector3Transpose(inV); }
        Matrix3x3& operator=(const class Matrix4x4& m);
        Matrix3x3& operator*=(const class Matrix4x4& inM);
        Matrix3x3& operator*=(const Matrix3x3& inM) {
            int i;
            for (i = 0; i < 3; i++) {
                float v[3] = {Get(i, 0), Get(i, 1), Get(i, 2)};
                Get(i, 0) = v[0] * inM.Get(0, 0) + v[1] * inM.Get(1, 0) + v[2] * inM.Get(2, 0);
                Get(i, 1) = v[0] * inM.Get(0, 1) + v[1] * inM.Get(1, 1) + v[2] * inM.Get(2, 1);
                Get(i, 2) = v[0] * inM.Get(0, 2) + v[1] * inM.Get(1, 2) + v[2] * inM.Get(2, 2);
            }
            return *this;
        }
        inline Matrix3x3& operator*=(float f) { for (float & i : m_Data) i *= f; return *this; }
        inline Matrix3x3& operator/=(float f) { return *this *= (1.0F / f); }
        bool Invert();
        [[nodiscard]] inline float GetDeterminant() const {
            float fCofactor0 = Get(0, 0) * Get(1, 1) * Get(2, 2);
            float fCofactor1 = Get(0, 1) * Get(1, 2) * Get(2, 0);
            float fCofactor2 = Get(0, 2) * Get(1, 0) * Get(2, 1);

            float fCofactor3 = Get(0, 2) * Get(1, 1) * Get(2, 0);
            float fCofactor4 = Get(0, 1) * Get(1, 0) * Get(2, 2);
            float fCofactor5 = Get(0, 0) * Get(1, 2) * Get(2, 1);

            return fCofactor0 + fCofactor1 + fCofactor2 - fCofactor3 - fCofactor4 - fCofactor5;
        }
        inline Matrix3x3& Transpose() {
            std::swap(Get(0, 1), Get(1, 0));
            std::swap(Get(0, 2), Get(2, 0));
            std::swap(Get(2, 1), Get(1, 2));
            return *this;
        }
        inline void InvertTranspose() {
            Invert();
            Transpose();
        }
        inline Matrix3x3& SetIdentity() {
            Get(0, 0) = 1.0F;  Get(0, 1) = 0.0F;  Get(0, 2) = 0.0F;
            Get(1, 0) = 0.0F;  Get(1, 1) = 1.0F;  Get(1, 2) = 0.0F;
            Get(2, 0) = 0.0F;  Get(2, 1) = 0.0F;  Get(2, 2) = 1.0F;
            return *this;
        }
        inline Matrix3x3& SetZero() {
            Get(0, 0) = 0.0F;  Get(0, 1) = 0.0F;  Get(0, 2) = 0.0F;
            Get(1, 0) = 0.0F;  Get(1, 1) = 0.0F;  Get(1, 2) = 0.0F;
            Get(2, 0) = 0.0F;  Get(2, 1) = 0.0F;  Get(2, 2) = 0.0F;
            return *this;
        }
        inline Matrix3x3& SetFromToRotation(const Vector3& from, const Vector3& to) {
            Vector3 v = Vector3::Cross(from, to);
            float e = Vector3::Dot(from, to);
            const float kEpsilon = 0.000001f;
            if (e > 1.0 - kEpsilon) {
                Get(0, 0) = 1.0; Get(0, 1) = 0.0; Get(0, 2) = 0.0;
                Get(1, 0) = 0.0; Get(1, 1) = 1.0; Get(1, 2) = 0.0;
                Get(2, 0) = 0.0; Get(2, 1) = 0.0; Get(2, 2) = 1.0;
            } else if (e < -1.0 + kEpsilon) {
                float invlen;
                float fxx, fyy, fzz, fxy, fxz, fyz;
                float uxx, uyy, uzz, uxy, uxz, uyz;
                float lxx, lyy, lzz, lxy, lxz, lyz;
                Vector3 left(0.0f, from[2], -from[1]);
                if (Vector3::Dot(left, left) < kEpsilon) left[0] = -from[2]; left[1] = 0.0; left[2] = from[0];

                invlen = 1.0f / sqrt(Vector3::Dot(left, left));
                left[0] *= invlen;
                left[1] *= invlen;
                left[2] *= invlen;
                Vector3 up = Vector3::Cross(left, from);
                fxx = -from[0] * from[0]; fyy = -from[1] * from[1]; fzz = -from[2] * from[2];
                fxy = -from[0] * from[1]; fxz = -from[0] * from[2]; fyz = -from[1] * from[2];

                uxx = up[0] * up[0]; uyy = up[1] * up[1]; uzz = up[2] * up[2];
                uxy = up[0] * up[1]; uxz = up[0] * up[2]; uyz = up[1] * up[2];

                lxx = -left[0] * left[0]; lyy = -left[1] * left[1]; lzz = -left[2] * left[2];
                lxy = -left[0] * left[1]; lxz = -left[0] * left[2]; lyz = -left[1] * left[2];

                Get(0, 0) = fxx + uxx + lxx; Get(0, 1) = fxy + uxy + lxy; Get(0, 2) = fxz + uxz + lxz;
                Get(1, 0) = Get(0, 1);   Get(1, 1) = fyy + uyy + lyy; Get(1, 2) = fyz + uyz + lyz;
                Get(2, 0) = Get(0, 2);   Get(2, 1) = Get(1, 2);   Get(2, 2) = fzz + uzz + lzz;
            } else {
                float hvx, hvz, hvxy, hvxz, hvyz;
                float h = (1.0f - e) / Vector3::Dot(v, v);
                hvx = h * v[0];
                hvz = h * v[2];
                hvxy = hvx * v[1];
                hvxz = hvx * v[2];
                hvyz = hvz * v[1];
                Get(0, 0) = e + hvx * v[0]; Get(0, 1) = hvxy - v[2];     Get(0, 2) = hvxz + v[1];
                Get(1, 0) = hvxy + v[2];  Get(1, 1) = e + h * v[1] * v[1]; Get(1, 2) = hvyz - v[0];
                Get(2, 0) = hvxz - v[1];  Get(2, 1) = hvyz + v[0];     Get(2, 2) = e + hvz * v[2];
            }
            return *this;
        }
        inline Matrix3x3& SetAxisAngle(const Vector3& rotationAxis, float radians) {
            GetRotMatrixNormVec(m_Data, rotationAxis.GetPtr(), radians);
            return *this;
        }
        inline Matrix3x3& SetBasis(const Vector3& inX, const Vector3& inY, const Vector3& inZ) {
            Get(0, 0) = inX[0];    Get(0, 1) = inY[0];    Get(0, 2) = inZ[0];
            Get(1, 0) = inX[1];    Get(1, 1) = inY[1];    Get(1, 2) = inZ[1];
            Get(2, 0) = inX[2];    Get(2, 1) = inY[2];    Get(2, 2) = inZ[2];
            return *this;
        }
        inline Matrix3x3& SetBasisTransposed(const Vector3& inX, const Vector3& inY, const Vector3& inZ) {
            Get(0, 0) = inX[0];    Get(1, 0) = inY[0];    Get(2, 0) = inZ[0];
            Get(0, 1) = inX[1];    Get(1, 1) = inY[1];    Get(2, 1) = inZ[1];
            Get(0, 2) = inX[2];    Get(1, 2) = inY[2];    Get(2, 2) = inZ[2];
            return *this;
        }
        inline Matrix3x3& SetScale(const Vector3& inScale) {
            Get(0, 0) = inScale[0];    Get(0, 1) = 0.0F;          Get(0, 2) = 0.0F;
            Get(1, 0) = 0.0F;          Get(1, 1) = inScale[1];    Get(1, 2) = 0.0F;
            Get(2, 0) = 0.0F;          Get(2, 1) = 0.0F;          Get(2, 2) = inScale[2];
            return *this;
        }
        inline Matrix3x3& Scale(const Vector3& inScale) {
            Get(0, 0) *= inScale[0];
            Get(1, 0) *= inScale[0];
            Get(2, 0) *= inScale[0];

            Get(0, 1) *= inScale[1];
            Get(1, 1) *= inScale[1];
            Get(2, 1) *= inScale[1];

            Get(0, 2) *= inScale[2];
            Get(1, 2) *= inScale[2];
            Get(2, 2) *= inScale[2];
            return *this;
        }
        inline bool IsIdentity(float threshold = Vector3::kEpsilon) {
            if (CompareApproximately(Get(0, 0), 1.0f, threshold) && CompareApproximately(Get(0, 1), 0.0f, threshold) && CompareApproximately(Get(0, 2), 0.0f, threshold) &&
                CompareApproximately(Get(1, 0), 0.0f, threshold) && CompareApproximately(Get(1, 1), 1.0f, threshold) && CompareApproximately(Get(1, 2), 0.0f, threshold) &&
                CompareApproximately(Get(2, 0), 0.0f, threshold) && CompareApproximately(Get(2, 1), 0.0f, threshold) && CompareApproximately(Get(2, 2), 1.0f, threshold))
                return true;
            return false;
        }
        inline static bool LookRotationToMatrix(const Vector3& viewVec, const Vector3& upVec, Matrix3x3* m) {
            Vector3 z = viewVec;
            float mag = Vector3::Magnitude(z);
            if (mag < Vector3::kEpsilon) {
                m->SetIdentity();
                return false;
            }
            z /= mag;

            Vector3 x = Vector3::Cross(upVec, z);
            mag = Vector3::Magnitude(x);
            if (mag < Vector3::kEpsilon) {
                m->SetIdentity();
                return false;
            }
            x /= mag;
            
            Vector3 y(Vector3::Cross(z, x));
            if (!CompareApproximately(Vector3::SqrMagnitude(y), 1.0F)) return false;

            m->SetBasis(x, y, z);
            return true;
        }
        inline static void EulerToMatrix(const Vector3& inV, Matrix3x3& matrix) {
            float cx = cos(inV.x);
            float sx = sin(inV.x);
            float cy = cos(inV.y);
            float sy = sin(inV.y);
            float cz = cos(inV.z);
            float sz = sin(inV.z);

            matrix.Get(0, 0) = cy * cz + sx * sy * sz;
            matrix.Get(0, 1) = cz * sx * sy - cy * sz;
            matrix.Get(0, 2) = cx * sy;

            matrix.Get(1, 0) = cx * sz;
            matrix.Get(1, 1) = cx * cz;
            matrix.Get(1, 2) = -sx;

            matrix.Get(2, 0) = -cz * sy + cy * sx * sz;
            matrix.Get(2, 1) = cy * cz * sx + sy * sz;
            matrix.Get(2, 2) = cx * cy;
        }
        inline static void GetRotMatrixNormVec(float* out, const float* inVec, float radians) {
            float s, c;
            float vx, vy, vz, xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

            s = sin(radians);
            c = cos(radians);

            vx = inVec[0];
            vy = inVec[1];
            vz = inVec[2];

#define M(row, col)  out[(row)*3 + col]
            xx = vx * vx;
            yy = vy * vy;
            zz = vz * vz;
            xy = vx * vy;
            yz = vy * vz;
            zx = vz * vx;
            xs = vx * s;
            ys = vy * s;
            zs = vz * s;
            one_c = 1.0F - c;

            M(0, 0) = (one_c * xx) + c;
            M(1, 0) = (one_c * xy) - zs;
            M(2, 0) = (one_c * zx) + ys;

            M(0, 1) = (one_c * xy) + zs;
            M(1, 1) = (one_c * yy) + c;
            M(2, 1) = (one_c * yz) - xs;

            M(0, 2) = (one_c * zx) - ys;
            M(1, 2) = (one_c * yz) + xs;
            M(2, 2) = (one_c * zz) + c;
#undef M
        }
        static void OrthoNormalize(Matrix3x3& matrix) {
            Vector3* c0 = (Vector3*)matrix.GetPtr() + 0;
            Vector3* c1 = (Vector3*)matrix.GetPtr() + 3;
            Vector3* c2 = (Vector3*)matrix.GetPtr() + 6;
            Vector3::OrthoNormalize(*c0, *c1, *c2);
        }

        static const Matrix3x3 zero;
        static const Matrix3x3 identity;
    };
}
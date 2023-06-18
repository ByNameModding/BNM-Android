#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3x3.h"
#define UNITY_USE_COPYMATRIX_4X4 (0)

#define MAT(m, r, c) (m)[(c)*4+(r)]
#define PP_WRAP_CODE(code) do { code; } while(0)
#define RETURN_ZERO PP_WRAP_CODE(\
    for (int i=0;i<16;i++) \
        out[i] = 0.0F; \
    return false;\
)
namespace BNM {
    namespace UNITY_STRUCTS {
        template<class T>
        inline T* Stride(T* p, size_t offset) { return reinterpret_cast<T*>((char*)p + offset); }

        struct FrustumPlanes
        {
            float left;
            float right;
            float bottom;
            float top;
            float zNear;
            float zFar;
        };
        bool InvertMatrix4x4_Full(const float* m, float* out);
        struct Matrix4x4 {
            float m_Data[16]{};
            enum InitIdentity { kIdentity };

            Matrix4x4() {}  // Default ctor is intentionally empty for performance reasons
            explicit Matrix4x4(InitIdentity) { SetIdentity(); }
            Matrix4x4(const Matrix3x3 &other);
            explicit Matrix4x4(const float data[16]) {
                for (int i = 0; i < 16; i++)
                    m_Data[i] = data[i];
            }

            float& Get(int row, int column)            { return m_Data[row + (column * 4)]; }
            const float& Get(int row, int column) const { return m_Data[row + (column * 4)]; }
            float* GetPtr()                            { return m_Data; }
            const float* GetPtr() const                 { return m_Data; }

            float operator[](int index) const         { return m_Data[index]; }
            float& operator[](int index)              { return m_Data[index]; }

            inline Matrix4x4& operator*=(const Matrix4x4& inM);
            inline Matrix4x4& operator=(const Matrix3x3& other);
            inline Vector3 MultiplyVector3(const Vector3& inV) const;
            inline void MultiplyVector3(const Vector3& inV, Vector3& output) const;
            inline bool PerspectiveMultiplyVector3(const Vector3& inV, Vector3& output) const;
            inline Vector4 MultiplyVector4(const Vector4& inV) const;
            inline void MultiplyVector4(const Vector4& inV, Vector4& output) const;
            inline Vector3 MultiplyPoint3(const Vector3& inV) const;
            inline void MultiplyPoint3(const Vector3& inV, Vector3& output) const;
            inline Vector2 MultiplyPoint2(const Vector2& inV) const;
            inline void MultiplyPoint2(const Vector2& inV, Vector2& output) const;
            inline bool PerspectiveMultiplyPoint3(const Vector3& inV, Vector3& output) const;
            inline Vector3 InverseMultiplyPoint3Affine(const Vector3& inV) const;
            inline Vector3 InverseMultiplyVector3Affine(const Vector3& inV) const;

            inline bool IsIdentity(float epsilon = Vector3::epsilon) const;
            // Returns whether a matrix is a perspective projection transform (i.e. doesn't have 0,0,0,1 in the last column).
            bool IsPerspective() const { return (m_Data[3] != 0.0f || m_Data[7] != 0.0f || m_Data[11] != 0.0f || m_Data[15] != 1.0f); }
            // return maximum absolute scale, ~1 for no scale
            float MaxabsScale() const {
                float a = Vector3::SqrMagnitude(GetAxisX());
                a = std::max(a, Vector3::SqrMagnitude(GetAxisY()));
                a = std::max(a, Vector3::SqrMagnitude(GetAxisZ()));
                return sqrtf(a);
            }
            inline void RemoveScale();

            inline float GetDeterminant() const;
            inline float GetDeterminant2x2() const;

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

                // Calculate the determinant of upper left 3x3 sub-matrix and
                // determine if the matrix is singular.
                pos = neg = 0.0;
                t = MAT(in, 0, 0) * MAT(in, 1, 1) * MAT(in, 2, 2);
                if (t >= 0.0)
                    pos += t;
                else
                    neg += t;

                t = MAT(in, 1, 0) * MAT(in, 2, 1) * MAT(in, 0, 2);
                if (t >= 0.0)
                    pos += t;
                else
                    neg += t;

                t = MAT(in, 2, 0) * MAT(in, 0, 1) * MAT(in, 1, 2);
                if (t >= 0.0)
                    pos += t;
                else
                    neg += t;

                t = -MAT(in, 2, 0) * MAT(in, 1, 1) * MAT(in, 0, 2);
                if (t >= 0.0)
                    pos += t;
                else
                    neg += t;

                t = -MAT(in, 1, 0) * MAT(in, 0, 1) * MAT(in, 2, 2);
                if (t >= 0.0)
                    pos += t;
                else
                    neg += t;

                t = -MAT(in, 0, 0) * MAT(in, 2, 1) * MAT(in, 1, 2);
                if (t >= 0.0)
                    pos += t;
                else
                    neg += t;

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

                // Do the translation part
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

            inline Matrix4x4& Transpose();

            inline Matrix4x4& SetIdentity();
            inline Matrix4x4& SetZero();
            inline Matrix4x4& SetPerspective(float fovy, float aspect, float zNear, float zFar);
            // rad = Deg2Rad(fovy/2), contanHalfFOV = cos(rad)/sin(rad)
            inline Matrix4x4& SetPerspectiveCotan(float cotanHalfFOV, float zNear, float zFar);
            inline Matrix4x4& SetOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
            inline Matrix4x4& SetFrustum(float left, float right, float bottom, float top, float nearval, float farval);
            inline Matrix4x4& AdjustDepthRange(float origNear, float newNear, float newFar);

            inline Vector3 GetAxisX() const;
            inline Vector3 GetAxisY() const;
            inline Vector3 GetAxisZ() const;
            inline Vector3 GetAxis(int axis) const;
            inline Vector3 GetPosition() const;
            inline Vector3 GetLossyScale() const;
            inline Vector4 GetRow(int row) const;
            inline Vector4 GetColumn(int col) const;
            // these set only these components of the matrix, everything else is untouched!
            inline void SetAxisX(const Vector3& v);
            inline void SetAxisY(const Vector3& v);
            inline void SetAxisZ(const Vector3& v);
            inline void SetAxis(int axis, const Vector3& v);
            inline void SetPosition(const Vector3& v);
            inline void SetRow(int row, const Vector4& v);
            inline void SetColumn(int col, const Vector4& v);

            inline Matrix4x4& SetTranslate(const Vector3& inTrans) {
                Get(0, 0) = 1.0;   Get(0, 1) = 0.0;   Get(0, 2) = 0.0;   Get(0, 3) = inTrans.x;
                Get(1, 0) = 0.0;   Get(1, 1) = 1.0;   Get(1, 2) = 0.0;   Get(1, 3) = inTrans.y;
                Get(2, 0) = 0.0;   Get(2, 1) = 0.0;   Get(2, 2) = 1.0;   Get(2, 3) = inTrans.z;
                Get(3, 0) = 0.0;   Get(3, 1) = 0.0;   Get(3, 2) = 0.0;   Get(3, 3) = 1.0;
                return *this;
            }
            inline Matrix4x4& SetBasis(const Vector3& inX, const Vector3& inY, const Vector3& inZ);
            inline Matrix4x4& SetBasisTransposed(const Vector3& inX, const Vector3& inY, const Vector3& inZ);

            inline Matrix4x4& SetScale(const Vector3& inScale){
                Get(0, 0) = inScale.x;    Get(0, 1) = 0.0;           Get(0, 2) = 0.0;           Get(0, 3) = 0.0;
                Get(1, 0) = 0.0;           Get(1, 1) = inScale.y;    Get(1, 2) = 0.0;           Get(1, 3) = 0.0;
                Get(2, 0) = 0.0;           Get(2, 1) = 0.0;           Get(2, 2) = inScale.z;    Get(2, 3) = 0.0;
                Get(3, 0) = 0.0;           Get(3, 1) = 0.0;           Get(3, 2) = 0.0;           Get(3, 3) = 1.0;
                return *this;
            }
            inline Matrix4x4& SetScaleAndPosition(const Vector3& inScale, const Vector3& inPosition);
            inline Matrix4x4& SetPositionAndOrthoNormalBasis(const Vector3& inPosition, const Vector3& inX, const Vector3& inY, const Vector3& inZ);

            inline Matrix4x4& Translate(const Vector3& inTrans);
            inline Matrix4x4& Scale(const Vector3& inScale);

            inline Matrix4x4& SetFromToRotation(const Vector3& from, const Vector3& to);

            inline void SetTR(const Vector3& pos, const Quaternion& q);
            inline void SetTRS(const Vector3& pos, const Quaternion& q, const Vector3& s);
            inline void SetTRInverse(const Vector3& pos, const Quaternion& q);
            FrustumPlanes DecomposeProjection() const;
            static const Matrix4x4 identity;
            inline bool ValidTRS() const;
            inline static Matrix4x4 Rotate(Quaternion q);
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
    }
}
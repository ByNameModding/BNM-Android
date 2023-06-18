#pragma once
namespace BNM {
    namespace UNITY_STRUCTS {
        struct Matrix3x3 {
            float m_Data[9]{};
            inline Matrix3x3() {}  // Default ctor is intentionally empty for performance reasons
            inline Matrix3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) { Get(0, 0) = m00; Get(1, 0) = m10; Get(2, 0) = m20; Get(0, 1) = m01; Get(1, 1) = m11; Get(2, 1) = m21; Get(0, 2) = m02; Get(1, 2) = m12; Get(2, 2) = m22; }
            inline explicit Matrix3x3(const class Matrix4x4& m);
            // The Get function accesses the matrix in std math convention
            // m0,0 m0,1 m0,2
            // m1,0 m1,1 m1,2
            // m2,0 m2,1 m2,2

            // The floats are laid out:
            // m0   m3   m6
            // m1   m4   m7
            // m2   m5   m8


            inline float& Get(int row, int column)                { return m_Data[row + (column * 3)]; }
            inline const float& Get(int row, int column) const     { return m_Data[row + (column * 3)]; }

            inline float& operator[](int row)                { return m_Data[row]; }
            inline float operator[](int row) const               { return m_Data[row]; }

            inline float* GetPtr()                                { return m_Data; }
            inline const float* GetPtr() const                { return m_Data; }

            inline Vector3 GetColumn(int col) const { return Vector3(Get(0, col), Get(1, col), Get(2, col)); }
            inline Matrix3x3& operator=(const class Matrix4x4& m);

            inline Matrix3x3& operator*=(const Matrix3x3& inM);
            inline Matrix3x3& operator*=(const class Matrix4x4& inM);
            inline friend Matrix3x3 operator*(const Matrix3x3& lhs, const Matrix3x3& rhs);
            inline Vector3 MultiplyVector3(const Vector3& inV) const;
            inline void MultiplyVector3(const Vector3& inV, Vector3& output) const;

            inline Vector3 MultiplyPoint3(const Vector3& inV) const                 { return MultiplyVector3(inV); }
            inline Vector3 MultiplyVector3Transpose(const Vector3& inV) const;
            inline Vector3 MultiplyPoint3Transpose(const Vector3& inV) const        { return MultiplyVector3Transpose(inV); }

            inline Matrix3x3& operator*=(float f);
            inline Matrix3x3& operator/=(float f) { return *this *= (1.0F / f); }

            inline float GetDeterminant() const;

            //  Matrix3x3& Transpose (const Matrix3x3& inM);
            inline Matrix3x3& Transpose();
            //  Matrix3x3& Invert (const Matrix3x3& inM)                                              { return Transpose (inM); }
            inline bool Invert();
            inline void InvertTranspose();

            inline Matrix3x3& SetIdentity();
            inline Matrix3x3& SetZero();
            inline Matrix3x3& SetFromToRotation(const Vector3& from, const Vector3& to);
            inline Matrix3x3& SetAxisAngle(const Vector3& rotationAxis, float radians);
            inline Matrix3x3& SetBasis(const Vector3& inX, const Vector3& inY, const Vector3& inZ);
            inline Matrix3x3& SetBasisTransposed(const Vector3& inX, const Vector3& inY, const Vector3& inZ);
            inline Matrix3x3& SetScale(const Vector3& inScale);
            inline Matrix3x3& Scale(const Vector3& inScale);

            inline bool IsIdentity(float threshold = Vector3::epsilon);

            static const Matrix3x3 zero;
            static const Matrix3x3 identity;
            static bool LookRotationToMatrix(const Vector3& viewVec, const Vector3& upVec, Matrix3x3* m);
            static void EulerToMatrix(const Vector3& v, Matrix3x3& matrix);
            static void GetRotMatrixNormVec(float* out, const float* inVec, float radians);
            static void OrthoNormalize(Matrix3x3& matrix);
        };

// Generates an orthornormal basis from a look at rotation, returns if it was successful
// (Righthanded)


    }
}
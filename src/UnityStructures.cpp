#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/UnityStructures.hpp>


namespace BNM::Structures::Unity {
    void *RaycastHit::GetCollider() const {
        if (!m_Collider || (BNM_PTR) m_Collider < 0) return {};
#if UNITY_VER > 174
        static void * (*FromId)(int);
        if (!FromId) InitFunc(FromId, GetExternMethod(OBFUSCATE_BNM("UnityEngine.Object::FindObjectFromInstanceID")));
        return FromId(m_Collider);
#else
        return m_Collider;
#endif
    }
    const float Vector4::infinity = std::numeric_limits<float>::infinity();
    const Vector4 Vector4::infinityVec = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
    const Vector4 Vector4::zero = {0, 0, 0, 0};
    const Vector4 Vector4::one = {1, 1, 1, 1};
    const Vector3 Vector3::back = {0.f, 0.f, -1.f};
    const Vector3 Vector3::down = {0.f, -1.f, 0.f};
    const Vector3 Vector3::forward = {0.f, 0.f, 1.f};
    const float Vector3::kEpsilon = 1E-05f;
    const float Vector3::kEpsilonNormalSqrt = 1E-15f;
    const Vector3 Vector3::left = {-1.f, 0.f, 0.f};
    const Vector3 Vector3::negativeInfinity = {-INFINITY, -INFINITY, -INFINITY};
    const Vector3 Vector3::one = {1.f, 1.f, 1.f};
    const Vector3 Vector3::positiveInfinity = {INFINITY, INFINITY, INFINITY};
    const Vector3 Vector3::right = {1.f, 0.f, 0.f};
    const Vector3 Vector3::up = {0.f, 1.f, 0.f};
    const Vector3 Vector3::zero = {0.f, 0.f, 0.f};
    const Matrix4x4 Matrix4x4::identity(kIdentity);

    Matrix3x3& Matrix3x3::operator=(const Matrix4x4& other) {
        m_Data[0] = other.m_Data[0];
        m_Data[1] = other.m_Data[1];
        m_Data[2] = other.m_Data[2];
        m_Data[3] = other.m_Data[4];
        m_Data[4] = other.m_Data[5];
        m_Data[5] = other.m_Data[6];
        m_Data[6] = other.m_Data[8];
        m_Data[7] = other.m_Data[9];
        m_Data[8] = other.m_Data[10];
        return *this;
    }
    Matrix3x3::Matrix3x3(const Matrix4x4& other) {
        m_Data[0] = other.m_Data[0];
        m_Data[1] = other.m_Data[1];
        m_Data[2] = other.m_Data[2];
        m_Data[3] = other.m_Data[4];
        m_Data[4] = other.m_Data[5];
        m_Data[5] = other.m_Data[6];
        m_Data[6] = other.m_Data[8];
        m_Data[7] = other.m_Data[9];
        m_Data[8] = other.m_Data[10];
    }
    Matrix3x3& Matrix3x3::operator*=(const Matrix4x4& inM) {
        for (int i = 0; i < 3; i++) {
            float v[3] = {Get(i, 0), Get(i, 1), Get(i, 2)};
            Get(i, 0) = v[0] * inM.Get(0, 0) + v[1] * inM.Get(1, 0) + v[2] * inM.Get(2, 0);
            Get(i, 1) = v[0] * inM.Get(0, 1) + v[1] * inM.Get(1, 1) + v[2] * inM.Get(2, 1);
            Get(i, 2) = v[0] * inM.Get(0, 2) + v[1] * inM.Get(1, 2) + v[2] * inM.Get(2, 2);
        }
        return *this;
    }
    bool Matrix3x3::Invert() {
        Matrix4x4 m = *this;
        bool success = InvertMatrix4x4_Full(m.GetPtr(), m.GetPtr());
        *this = m;
        return success;
    }
    Vector2::operator Vector3() const {
        return {x, y, 0};
    }
}
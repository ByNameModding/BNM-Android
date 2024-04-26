#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/UnityStructures.hpp>
#include "BNM/UnityStructures/Color.h"
#include "BNM/UnityStructures/Vector4.h"
#include "Internals.hpp"


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

    const Color Color::black = {0.f, 0.f, 0.f};
    const Color Color::red = {1.f, 0.f, 0.f};
    const Color Color::green = {0.f, 1.f, 0.f};
    const Color Color::blue = {0.f, 0.f, 1.f};
    const Color Color::white = {1.f, 1.f, 1.f};
    const Color Color::orange = {1.f, 0.55f, 0.f};
    const Color Color::yellow = {1.f, 0.92156863f, 0.015686275f};
    const Color Color::cyan = {0.f, 1.f, 1.f};
    const Color Color::magenta = {1.f, 0.f, 1.f};

    Color::Color(Vector4 v) : r(v.x), g(v.y), b(v.z), a(v.w) {}

    constexpr float floatInf = std::numeric_limits<float>::infinity();

    const Vector2 Vector2::positiveInfinity = {floatInf, floatInf};
    const Vector2 Vector2::negativeInfinity = {-floatInf, -floatInf};
    const Vector2 Vector2::down = {0.f, -1.f};
    const Vector2 Vector2::left = {-1.f, 0.f};
    const Vector2 Vector2::one = {1.f, 1.f};
    const Vector2 Vector2::right = {1.f, 0.f};
    const Vector2 Vector2::up = {0.f, 1.f};
    const Vector2 Vector2::zero = {0.f, 0.f};

    Vector2::operator Vector3() const { return {x, y, 0}; }

    const Vector3 Vector3::positiveInfinity = {floatInf, floatInf, floatInf};
    const Vector3 Vector3::negativeInfinity = {-floatInf, -floatInf, -floatInf};
    const Vector3 Vector3::back = {0.f, 0.f, -1.f};
    const Vector3 Vector3::down = {0.f, -1.f, 0.f};
    const Vector3 Vector3::forward = {0.f, 0.f, 1.f};
    const Vector3 Vector3::left = {-1.f, 0.f, 0.f};
    const Vector3 Vector3::one = {1.f, 1.f, 1.f};
    const Vector3 Vector3::right = {1.f, 0.f, 0.f};
    const Vector3 Vector3::up = {0.f, 1.f, 0.f};
    const Vector3 Vector3::zero = {0.f, 0.f, 0.f};

    const Vector4 Vector4::positiveinfinity = {floatInf, floatInf, floatInf, floatInf};
    const Vector4 Vector4::negativeInfinity = {-floatInf, -floatInf, -floatInf, -floatInf};
    const Vector4 Vector4::zero = {0.f, 0.f, 0.f, 0.f};
    const Vector4 Vector4::one = {1.f, 1.f, 1.f, 1.f};

    Vector4::Vector4(Color c) : x(c.r), y(c.g), z(c.b), w(c.a) {}

    const Quaternion Quaternion::identity = {0.f, 0.f, 0.f, 1.f};

    const Matrix4x4 Matrix4x4::identity(InitIdentity::kIdentity);

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
}



namespace BNM::UnityEngine {
    BNM::Class UnityEventBase::GetArgumentType(PersistentCall *call) {
        auto type = Internal::vmData.UnityEngine$$Object;
        auto typeName = call->m_Arguments->m_ObjectArgumentAssemblyTypeName;
        if (!typeName->IsNullOrEmpty()) if (auto t = Internal::vmData.Type$$GetType(typeName); t != nullptr) type = t;

        return type;
    }
    BNM::Class UnityEventBase::GetTargetType(PersistentCall *call) {
        BNM::Class targetType;
        if (call->m_Target != nullptr) targetType = call->m_Target;
        else targetType = Internal::vmData.Type$$GetType(call->m_TargetAssemblyTypeName);

        return targetType;
    }
}
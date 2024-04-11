#pragma once
#include "Vector3.h"
#include "Vector2.h"
namespace BNM {
#if defined(__LP64__)
    typedef long BNM_INT_PTR;
    typedef unsigned long BNM_PTR;
#else
    typedef int BNM_INT_PTR;
    typedef unsigned int BNM_PTR;
#endif
    namespace Structures::Unity {
        struct RaycastHit {
            Vector3 point{}, normal{};
            unsigned int faceID{};
            float distance{};
            Vector2 UV{};
        #if UNITY_VER > 174
            int m_Collider{};
        #else
            void *m_Collider{};
        #endif
            void *GetCollider() const;
            inline std::string str() { return OBFUSCATE_BNM("Point: ") + point.str() + OBFUSCATE_BNM(", Normal: ") + normal.str() + OBFUSCATE_BNM(", FaceID: ") + std::to_string(faceID) + OBFUSCATE_BNM(", Distance: ") + std::to_string(distance) + OBFUSCATE_BNM(", UV: ") + UV.str() + OBFUSCATE_BNM(", m_Collider: ") + std::to_string((BNM::BNM_PTR)m_Collider); }
        };
    }
}
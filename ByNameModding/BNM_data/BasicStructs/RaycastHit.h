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
            std::string str() { return OBFUSCATES_BNM("Point: ") + point.str() + OBFUSCATES_BNM(", Normal: ") + normal.str() + OBFUSCATES_BNM(", FaceID: ") + std::to_string(faceID) + OBFUSCATES_BNM(", Distance: ") + std::to_string(distance) + OBFUSCATES_BNM(", UV: ") + UV.str() + OBFUSCATES_BNM(", m_Collider: ") + std::to_string((BNM::BNM_PTR)m_Collider); }
        };
    }
}
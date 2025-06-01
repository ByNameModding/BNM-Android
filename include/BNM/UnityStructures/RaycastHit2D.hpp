#pragma once

#include <string>

#include "Vector2.hpp"

namespace BNM::Structures::Unity {
    struct RaycastHit2D {
        Vector2 centroid, point, normal;
        float distance, fraction;
#if UNITY_VER > 174
        int m_Collider{};
#else
        void *m_Collider{};
#endif
        void *GetCollider() const;

        inline std::string str() { return std::string(BNM_OBFUSCATE("centroid: ")) + centroid.str() + std::string(BNM_OBFUSCATE(", point: ")) + point.str() + std::string(BNM_OBFUSCATE(", normal: ")) + normal.str() + std::string(BNM_OBFUSCATE(", Distance: ")) + std::to_string(distance) + std::string(BNM_OBFUSCATE(", fraction: ")) + std::to_string(fraction) + std::string(BNM_OBFUSCATE(", m_Collider: ")) + std::to_string((BNM::BNM_PTR)m_Collider); }
    };
}
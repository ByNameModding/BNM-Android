#pragma once
#include "Vector3.h"
#include "Vector2.h"
struct RaycastHit {
    Vector3 Point{}, Normal{};
    unsigned int FaceID{};
    float Distance{};
    Vector2 UV{};
#if UNITY_VER > 174
    int m_Collider{};
#else
    void *m_Collider{};
#endif
    void *get_Collider() const;
    std::string str() { return OBFUSCATES_BNM("Point: ") + Point.str() + OBFUSCATES_BNM(", Normal: ") + Normal.str() + OBFUSCATES_BNM(", FaceID: ") + std::to_string(FaceID) + OBFUSCATES_BNM(", Distance: ") + std::to_string(Distance) + OBFUSCATES_BNM(", UV: ") + UV.str() + OBFUSCATES_BNM(", m_Collider: ") + std::to_string(m_Collider); }
};
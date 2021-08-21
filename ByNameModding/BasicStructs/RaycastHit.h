#pragma once

#include "vector2.h"

struct RaycastHit {
    Vector3 Point, Normal;
    std::uint32_t FaceID;
    float Distance;
    Vector2 UV;
    int32_t m_Collider;
};

std::string to_string(RaycastHit a) {
    return OBFUSCATES_BNM("Point: ") + to_string(a.Point) + OBFUSCATES_BNM(", Normal: ") +
           to_string(a.Normal) + OBFUSCATES_BNM(", FaceID: ") + to_string(a.FaceID) +
           OBFUSCATES_BNM(", Distance: ") + to_string(a.Distance) + OBFUSCATES_BNM(", UV: ") +
           to_string(a.UV) + OBFUSCATES_BNM(", m_Collider: ") +
           to_string(a.m_Collider);
}
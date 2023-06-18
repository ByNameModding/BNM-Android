#pragma once
#include "Vector3.h"
namespace BNM {
    namespace UNITY_STRUCTS {
        struct Ray {
            Vector3 m_Origin, m_Direction;
            std::string str() { return OBFUSCATES_BNM("m_Origin: ") + m_Origin.str() + OBFUSCATES_BNM(", m_Direction: ") + m_Direction.str(); }
        };
    }
}
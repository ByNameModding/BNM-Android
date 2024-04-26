#pragma once

#include "Vector3.h"

namespace BNM::Structures::Unity {
    struct Ray {
        Vector3 m_Origin{}, m_Direction{};

        inline std::string str() { return OBFUSCATE_BNM("m_Origin: ") + m_Origin.str() + OBFUSCATE_BNM(", m_Direction: ") + m_Direction.str(); }
    };
}
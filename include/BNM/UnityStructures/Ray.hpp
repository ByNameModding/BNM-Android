#pragma once

#include "Vector3.hpp"

namespace BNM::Structures::Unity {
    struct Ray {
        Vector3 m_Origin{}, m_Direction{};

        inline std::string str() { return std::string(BNM_OBFUSCATE("m_Origin: ")) + m_Origin.str() + std::string(BNM_OBFUSCATE(", m_Direction: ")) + m_Direction.str(); }
    };
}
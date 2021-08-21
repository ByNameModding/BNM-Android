#pragma once

using namespace std;
#include "../macros.h"
struct Ray {
    union
    {
        struct
        {
            Vector3 m_Origin, m_Direction;
        };
        Vector3 data[2];
    };
};
std::string to_string(Ray a) {
    return OBFUSCATES_BNM("m_Origin: ") + to_string(a.m_Origin) + OBFUSCATES_BNM(", m_Direction: ") + to_string(a.m_Direction);
}
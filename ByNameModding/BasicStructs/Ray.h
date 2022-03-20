#pragma once

using namespace std;
struct Ray {
    Vector3 m_Origin, m_Direction;
};
string to_string(Ray a) {
    return OBFUSCATES_BNM("m_Origin: ") + to_string(a.m_Origin) + OBFUSCATES_BNM(", m_Direction: ") + to_string(a.m_Direction);
}
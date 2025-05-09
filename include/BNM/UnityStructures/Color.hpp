#pragma once

#include "BNM/UserSettings/GlobalSettings.hpp"

#include <string>
#include <algorithm>

namespace BNM::Structures::Unity {
    struct Vector4;

    struct Color {
        union {
            struct { float r, g, b, a; };
            float data[4]{0.f, 0.f, 0.f, 1.f};
        };
        inline constexpr Color() = default;
        inline constexpr Color(float r, float g, float b) : r(r), g(g), b(b), a(1.f) {}
        inline constexpr Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
        inline Color(Vector4 v);

        [[nodiscard]] inline std::string str() const { return std::string(BNM_OBFUSCATE("r: ")) + std::to_string(r) + std::string(BNM_OBFUSCATE(", g: ")) + std::to_string(g) + std::string(BNM_OBFUSCATE(", b: ")) + std::to_string(b) + std::string(BNM_OBFUSCATE(", a: ")) + std::to_string(a); };

        inline static Color HSVToRGB(float H, float S, float V, bool hdr = true) {
            if (S == 0) return {V, V, V};
            else if (V == 0) return {0.f, 0.f, 0.f};

            float h_to_floor = H * 6.0f;

            int tmp = (int) floor(h_to_floor);
            float F = h_to_floor - (float) tmp;

            float P = (V) * (1 - S);
            float Q = V * (1 - S * F);
            float T = V * (1 - S * (1 - F));

            Color retVal = {0.f, 0.f, 0.f};
            switch (tmp) {
                case 0: retVal = {V, T, P}; break;
                case 1: retVal = {Q, V, P}; break;
                case 2: retVal = {P, V, T}; break;
                case 3: retVal = {P, Q, V}; break;
                case 4: retVal = {T, P, V}; break;
                case 5: retVal = {V, P, Q}; break;
                case 6: retVal = {V, T, P}; break;
                case -1: retVal = {V, P, Q}; break;
            }
            if (hdr) return retVal;

            return {std::clamp(retVal.r, 0.0f, 1.0f), std::clamp(retVal.g, 0.0f, 1.0f), std::clamp(retVal.b, 0.0f, 1.0f)};
        }

        inline static Color Lerp(Color a, Color b, float t) {
            t = t < 0.f ? 0.f : t > 1.f ? 1.f : t;
            return {
                a.r + (b.r - a.r) * t,
                a.g + (b.g - a.g) * t,
                a.b + (b.b - a.b) * t,
                a.a + (b.a - a.a) * t
            };
        }

        inline Color RGBMultiplied(float m) { return {r * m, g * m, b * m, a}; }
        inline Color RGBMultiplied(Color m) { return {r * m.r, g * m.g, b * m.b, a}; }

        inline friend bool operator ==(const Color& lhs, const Color& rhs) { return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a; }
        inline friend bool operator !=(const Color& lhs, const Color& rhs) { return !(lhs == rhs); }

        static const Color black, red, green, blue, white, orange, yellow, cyan, magenta;
    };

    struct Color32 {
        union {
            struct { uint8_t r{}, g{}, b{}, a{}; };
            int rgba;
        };
    };
}
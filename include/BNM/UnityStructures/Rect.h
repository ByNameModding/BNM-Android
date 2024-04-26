#pragma once

namespace BNM::Structures::Unity {
    struct Rect {
        union {
            struct { float x, y, w, h; };
            float data[4]{0.f, 0.f, 0.f, 0.f};
        };
        inline constexpr Rect() = default;
        inline constexpr Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

        inline std::string str() { return OBFUSCATE_BNM("x: ") + std::to_string(x) + OBFUSCATE_BNM(", y: ") + std::to_string(y) + OBFUSCATE_BNM(", w: ") + std::to_string(w) + OBFUSCATE_BNM(", h: ") + std::to_string(h); }

        inline bool operator ==(const Rect& other) const { return x == other.x && y == other.y && w == other.w && h == other.h; }
        inline bool operator !=(const Rect& other) const = default;
    };
}
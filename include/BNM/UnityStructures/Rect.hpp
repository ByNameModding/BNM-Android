#pragma once

namespace BNM::Structures::Unity {
    struct Rect {
        union {
            struct { float x, y, w, h; };
            float data[4]{0.f, 0.f, 0.f, 0.f};
        };
        inline constexpr Rect() = default;
        inline constexpr Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

        inline std::string str() { return std::string(BNM_OBFUSCATE("x: ")) + std::to_string(x) + std::string(BNM_OBFUSCATE(", y: ")) + std::to_string(y) + std::string(BNM_OBFUSCATE(", w: ")) + std::to_string(w) + std::string(BNM_OBFUSCATE(", h: ")) + std::to_string(h); }

        inline bool operator ==(const Rect& other) const { return x == other.x && y == other.y && w == other.w && h == other.h; }
        inline bool operator !=(const Rect& other) const = default;
    };
}
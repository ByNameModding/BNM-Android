#pragma once
namespace BNM::Structures::Unity {
    struct Rect {
        union {
            struct { float x, y, w, h; };
            float data[4];
        };
        Rect() noexcept : x(0), y(0), w(0), h(0) {}
        Rect(float x, float y, float w, float h) noexcept : x(x), y(y), w(w), h(h) {}
        std::string str() { return OBFUSCATES_BNM("x: ") +  std::to_string(x) + OBFUSCATES_BNM(", y: ") + std::to_string(y) + OBFUSCATES_BNM(", w: ") + std::to_string(w) + OBFUSCATES_BNM(", h: ") + std::to_string(h); }
    };
}
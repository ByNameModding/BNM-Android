#pragma once
struct Rect {
    union {
        struct { float x, y, w, h; };
        float data[4];
    };
    Rect(): x(0), y(0), w(0), h(0) {}
    Rect(float x1, float y1, float w1, float h1): x(x1), y(y1), w(w1), h(h1) {}
    std::string str() { return OBFUSCATES_BNM("x: ") +  std::to_string(x) + OBFUSCATES_BNM(", y: ") + std::to_string(y) + OBFUSCATES_BNM(", w: ") + std::to_string(w) + OBFUSCATES_BNM(", h: ") + std::to_string(h); }
};
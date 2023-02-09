#pragma once
#include <string>
struct Color {
    union {
        struct { float r, g, b, a; };
        float data[4];
    };
    inline Color() noexcept : r(0), g(0), b(0), a(255) {}
    inline Color(float r, float g, float b) noexcept : r(r), g(g), b(b), a(255) {}
    inline Color(float r, float g, float b, float a) noexcept : r(r), g(g), b(b), a(a) {}
    [[maybe_unused]] inline static Color Black(float a = 255) { return {0, 0, 0, a}; }
    [[maybe_unused]] inline static Color Red(float a = 255) { return {255, 0, 0, a}; }
    [[maybe_unused]] inline static Color Green(float a = 255) { return {0, 255, 0, a}; }
    [[maybe_unused]] inline static Color Blue(float a = 255) { return {0, 0, 255, a}; }
    [[maybe_unused]] inline static Color White(float a = 255) { return {255, 255, 255, a}; }
    [[maybe_unused]] inline static Color Orange(float a = 255) { return {255, 153, 0, a}; }
    [[maybe_unused]] inline static Color Yellow(float a = 255) { return {255, 255, 0, a}; }
    [[maybe_unused]] inline static Color Cyan(float a = 255) { return {0, 255, 255, a}; }
    [[maybe_unused]] inline static Color Magenta(float a = 255) { return {255, 0, 255, a}; }
    [[maybe_unused]] inline static Color MonoBlack(float a = 1) { return {0, 0, 0, a}; }
    [[maybe_unused]] inline static Color MonoRed(float a = 1) { return {1, 0, 0, a}; }
    [[maybe_unused]] inline static Color MonoGreen(float a = 1) { return {0, 1, 0, a}; }
    [[maybe_unused]] inline static Color MonoBlue(float a = 1) { return {0, 0, 1, a}; }
    [[maybe_unused]] inline static Color MonoWhite(float a = 1) { return {1, 1, 1, a}; }
    [[maybe_unused]] inline static Color MonoOrange(float a = 1) { return {1, 0.55, 0, a}; }
    [[maybe_unused]] inline static Color MonoYellow(float a = 1) { return {1, 1, 0, a}; }
    [[maybe_unused]] inline static Color MonoCyan(float a = 1) { return {0, 1, 1, a}; }
    [[maybe_unused]] inline static Color MonoMagenta(float a = 1) { return {1, 0, 1, a}; }
    [[maybe_unused]] inline static Color random(float a = 255) {
        float r = static_cast<float>(rand()) / static_cast<float>(255);
        float g = static_cast<float>(rand()) / static_cast<float>(255);
        float b = static_cast<float>(rand()) / static_cast<float>(255);
        return {r, g, b, a};
    }
    inline Color ToMono() { return {r / 255, g / 255, b / 255, a / 255 }; }
    inline std::string str() { return OBFUSCATES_BNM("r: ") +  std::to_string(r) + OBFUSCATES_BNM(", g: ") + std::to_string(g) + OBFUSCATES_BNM(", b: ") + std::to_string(b) + OBFUSCATES_BNM(", a: ") + std::to_string(a); };
};
inline bool operator ==(const Color& lhs, const Color& rhs) { return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a; }
inline bool operator !=(const Color& lhs, const Color& rhs) { return !(lhs == rhs); }
#pragma once
#include <string>
namespace BNM::Structures::Unity {
    template<typename T>
    inline T clamp(T value, T min, T max) {
        return value < min ? min : value > max ? max : value;
    }
    struct Color {
        union {
            struct { float r, g, b, a; };
            float data[4];
        };
        inline Color() noexcept : r(0), g(0), b(0), a(255) {}
        inline Color(float r, float g, float b) noexcept : r(r), g(g), b(b), a(255) {}
        inline Color(float r, float g, float b, float a) noexcept : r(r), g(g), b(b), a(a) {}
        inline static Color Black(float a = 255) { return {0, 0, 0, a}; }
        inline static Color Red(float a = 255) { return {255, 0, 0, a}; }
        inline static Color Green(float a = 255) { return {0, 255, 0, a}; }
        inline static Color Blue(float a = 255) { return {0, 0, 255, a}; }
        inline static Color White(float a = 255) { return {255, 255, 255, a}; }
        inline static Color Orange(float a = 255) { return {255, 153, 0, a}; }
        inline static Color Yellow(float a = 255) { return {255, 255, 0, a}; }
        inline static Color Cyan(float a = 255) { return {0, 255, 255, a}; }
        inline static Color Magenta(float a = 255) { return {255, 0, 255, a}; }
        inline static Color MonoBlack(float a = 1) { return {0, 0, 0, a}; }
        inline static Color MonoRed(float a = 1) { return {1, 0, 0, a}; }
        inline static Color MonoGreen(float a = 1) { return {0, 1, 0, a}; }
        inline static Color MonoBlue(float a = 1) { return {0, 0, 1, a}; }
        inline static Color MonoWhite(float a = 1) { return {1, 1, 1, a}; }
        inline static Color MonoOrange(float a = 1) { return {1, 0.55, 0, a}; }
        inline static Color MonoYellow(float a = 1) { return {1, 1, 0, a}; }
        inline static Color MonoCyan(float a = 1) { return {0, 1, 1, a}; }
        inline static Color MonoMagenta(float a = 1) { return {1, 0, 1, a}; }
        inline static Color random(float a = 255) {
            float r = static_cast<float>(rand()) / static_cast<float>(255);
            float g = static_cast<float>(rand()) / static_cast<float>(255);
            float b = static_cast<float>(rand()) / static_cast<float>(255);
            return {r, g, b, a};
        }
        inline Color ToMono() { return {r / 255, g / 255, b / 255, a / 255 }; }
        inline std::string str() { return OBFUSCATES_BNM("r: ") +  std::to_string(r) + OBFUSCATES_BNM(", g: ") + std::to_string(g) + OBFUSCATES_BNM(", b: ") + std::to_string(b) + OBFUSCATES_BNM(", a: ") + std::to_string(a); };
        inline static Color HSVToRGB(float H, float S, float V, bool hdr = true) {
            Color retval = Color::White();
            if (S == 0) {
                retval.r = V;
                retval.g = V;
                retval.b = V;
            } else if (V == 0) {
                retval.r = 0;
                retval.g = 0;
                retval.b = 0;
            } else {
                retval.r = 0;
                retval.g = 0;
                retval.b = 0;

                float t_S, t_V, h_to_floor;

                t_S = S;
                t_V = V;
                h_to_floor = H * 6.0f;

                int temp = (int)floor(h_to_floor);
                float t = h_to_floor - ((float)temp);
                float var_1 = (t_V) * (1 - t_S);
                float var_2 = t_V * (1 - t_S *  t);
                float var_3 = t_V * (1 - t_S * (1 - t));

                switch (temp) {
                    case 0:
                        retval.r = t_V;
                        retval.g = var_3;
                        retval.b = var_1;
                        break;

                    case 1:
                        retval.r = var_2;
                        retval.g = t_V;
                        retval.b = var_1;
                        break;

                    case 2:
                        retval.r = var_1;
                        retval.g = t_V;
                        retval.b = var_3;
                        break;

                    case 3:
                        retval.r = var_1;
                        retval.g = var_2;
                        retval.b = t_V;
                        break;

                    case 4:
                        retval.r = var_3;
                        retval.g = var_1;
                        retval.b = t_V;
                        break;

                    case 5:
                        retval.r = t_V;
                        retval.g = var_1;
                        retval.b = var_2;
                        break;

                    case 6:
                        retval.r = t_V;
                        retval.g = var_3;
                        retval.b = var_1;
                        break;

                    case -1:
                        retval.r = t_V;
                        retval.g = var_1;
                        retval.b = var_2;
                        break;
                }

                if (!hdr) {
                    retval.r = clamp(retval.r, 0.0f, 1.0f);
                    retval.g = clamp(retval.g, 0.0f, 1.0f);
                    retval.b = clamp(retval.b, 0.0f, 1.0f);
                }
            }
            return retval;
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
    };
    inline bool operator ==(const Color& lhs, const Color& rhs) { return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a; }
    inline bool operator !=(const Color& lhs, const Color& rhs) { return !(lhs == rhs); }
    struct Color32 {
        union {
            struct { uint8_t r, g, b, a; };
            int rgba;
        };
    };
}
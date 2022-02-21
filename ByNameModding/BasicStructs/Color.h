#pragma once
using namespace std;
#include "../macros.h"
struct Color {
    union {
        struct {
            float r, b, g, a;
        };
        float data[4];
    };
    Color() {
        SetColor(0, 0, 0, 255);
    }
    Color(float r, float g, float b) {
        SetColor(r, g, b, 255);
    }
    Color(float r, float g, float b, float a) {
        SetColor(r, g, b, a);
    }
    void SetColor(float r1, float g1, float b1, float a1 = 255) {
        r = r1;
        g = g1;
        b = b1;
        a = a1;
    }
    static Color Black(float a = 255) { return Color(0, 0, 0, a); }
    static Color Red(float a = 255) { return Color(255, 0, 0, a); }
    static Color Green(float a = 255) { return Color(0, 255, 0, a); }
    static Color Blue(float a = 255) { return Color(0, 0, 255, a); }
    static Color White(float a = 255) { return Color(255, 255, 255, a); }
    static Color Orange(float a = 255) { return Color(255, 153, 0, a); }
    static Color Yellow(float a = 255) { return Color(255, 255, 0, a); }
    static Color Cyan(float a = 255) { return Color(0, 255, 255, a); }
    static Color Magenta(float a = 255) { return Color(255, 0, 255, a); }
    static Color MonoBlack(float a = 1){ return Color(0, 0, 0, a); }
    static Color MonoRed(float a = 1){ return Color(1, 0, 0, a); }
    static Color MonoGreen(float a = 1){ return Color(0, 1, 0, a); }
    static Color MonoBlue(float a = 1){ return Color(0, 0, 1, a); }
    static Color MonoWhite(float a = 1){ return Color(1, 1, 1, a); }
    static Color MonoOrange(float a = 1){ return Color(1, 0.55, 0, a); }
    static Color MonoYellow(float a = 1){ return Color(1, 1, 0, a); }
    static Color MonoCyan(float a = 1){ return Color(0, 1, 1, a); }
    static Color MonoMagenta(float a = 1){ return Color(1, 0, 1, a); }
    static Color random(float a = 255) {
        float r = static_cast <float> (rand()) / static_cast <float> (255);
        float g = static_cast <float> (rand()) / static_cast <float> (255);
        float b = static_cast <float> (rand()) / static_cast <float> (255);
        return Color(r, g, b, a);
    }
    Color invert(int a = 255){
        return Color(255 - r, 255 - g, 255 - b, a);
    }
};

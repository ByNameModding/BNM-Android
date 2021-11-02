#pragma once
class Rect {
public:
    union {
        struct {
            float x;
            float y;
            float w;
            float h;
        };
        float data[4];
    };
    Rect()
            : x(0)
            , y(0)
            , w(0)
            , h(0)
    {
    }
    Rect(float x1, float y1, float w1, float h1)
            : x(x1)
            , y(y1)
            , w(w1)
            , h(h1)
    {
    }
    Rect(const Rect& v);
    ~Rect();
};
Rect::Rect(const Rect& v)
        : x(v.x)
        , y(v.y)
        , w(v.w)
        , h(v.h)
{
}
Rect::~Rect() {}

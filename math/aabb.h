#ifndef OC_AABB_H
#define OC_AABB_H

#include "vector2.h"

struct AABB
{
public:
    AABB()
    : Min(Vector2::Zero)
    , Max(Vector2::Zero)
    {}

    AABB(const Vector2& InMin, const Vector2& InMax)
    : Min(InMin)
    , Max(InMax)
    {}

    const AABB& operator+=(const Vector2& V)
    {
        Min.X = V.X < Min.X ? V.X : Min.X;
        Min.Y = V.Y < Min.Y ? V.Y : Min.Y;
        Max.X = V.X > Max.X ? V.X : Max.X;
        Max.Y = V.Y > Max.Y ? V.Y : Max.Y;

        return *this;
    }

    Vector2 GetCenter() const
    {
        return (Min + Max) * 0.5f;
    }

public:
    Vector2 Min;
    Vector2 Max;
};
#endif

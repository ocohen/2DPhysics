#include "circle.h"

Circle::Circle()
{
}

void Circle::SetPosition(const Vector2& InPosition)
{
    Position = InPosition;
}

void Circle::SetRadius(const float InRadius)
{
    Radius = InRadius;
}

bool Circle::OverlapTest(const Circle& OtherCircle) const
{
    const float Distance2 = (OtherCircle.Position - Position).Length2();
    const float Radii = (Radius + OtherCircle.Radius);
    return Distance2 <= Radii * Radii;
}

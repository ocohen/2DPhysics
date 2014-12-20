#include "circle.h"

Circle::Circle() : BaseShape(Shape::Circle)
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

Shape::Type Circle::ShapeType = Shape::Circle;

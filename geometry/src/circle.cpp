#include "circle.h"

Circle::Circle() : BaseShape(Shape::Circle)
{
}

Circle::Circle(const Vector2& InPosition, const float InRadius)
: BaseShape(Shape::Circle)
,Position(InPosition)
, Radius(InRadius)
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

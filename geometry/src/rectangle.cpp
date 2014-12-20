#include "rectangle.h"

Rectangle::Rectangle() : BaseShape(Shape::Rectangle)
{
}

void Rectangle::SetTransform(const Transform& InTransform)
{
    TM = InTransform;
}

void Rectangle::SetExtents(const Vector2& InExtents)
{
    Extents = InExtents;
}

Shape::Type Rectangle::ShapeType = Shape::Rectangle;

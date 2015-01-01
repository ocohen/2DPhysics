#include "rectangle.h"

Rectangle::Rectangle() : ConvexPolygon(Shape::Rectangle)
{
    UpdateVertices();
}

Rectangle::Rectangle(const Vector2& InExtents)
: ConvexPolygon(Shape::Rectangle)
,Extents(InExtents)
{
    UpdateVertices();
}

void Rectangle::SetExtents(const Vector2& InExtents)
{
    Extents = InExtents;
    UpdateVertices();
}

void Rectangle::UpdateVertices()
{
    ClearVertices();
    AddVertex(Vector2(Extents.X, -Extents.Y));
    AddVertex(Extents);
    AddVertex(Vector2(-Extents.X, Extents.Y));
    AddVertex(-Extents);
}

void Rectangle::GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& TM) const
{
    const Vector2 TopRight    = TM.TransformPosition(Extents);
    const Vector2 BottomRight = TM.TransformPosition(Vector2(Extents.X, -Extents.Y));
    const Vector2 BottomLeft  = TM.TransformPosition(-Extents);
    const Vector2 TopLeft     = TM.TransformPosition(Vector2(-Extents.X, Extents.Y));

    //CCW
    OutVertices.push_back(TopRight);
    OutVertices.push_back(TopLeft);
    OutVertices.push_back(BottomLeft);
    OutVertices.push_back(BottomRight);

}
Shape::Type Rectangle::ShapeType = Shape::Rectangle;

#include "RectangleShape.h"

RectangleShape::RectangleShape() : ConvexPolygon(Shape::Rectangle)
{
    UpdateVertices();
}

RectangleShape::RectangleShape(const Vector2& InExtents)
: ConvexPolygon(Shape::Rectangle)
,Extents(InExtents)
{
    UpdateVertices();
}

void RectangleShape::SetExtents(const Vector2& InExtents)
{
    Extents = InExtents;
    UpdateVertices();
}

void RectangleShape::UpdateVertices()
{
    ClearVertices();
    AddVertex(Vector2(Extents.X, -Extents.Y));
    AddVertex(Extents);
    AddVertex(Vector2(-Extents.X, Extents.Y));
    AddVertex(-Extents);
}

void RectangleShape::GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& TM) const
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
Shape::Type RectangleShape::ShapeType = Shape::Rectangle;

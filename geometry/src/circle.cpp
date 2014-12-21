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

void Circle::GenerateRenderVertices(std::vector<Vector2>& OutVertices, unsigned NumVertices) const
{
    const float DeltaRad = PI*2.f / NumVertices;
    for(unsigned Count = 0; Count < NumVertices; ++Count)
    {
        const float Rad = DeltaRad * Count;
        const Vector2 Dir = Vector2(cos(Rad), sin(Rad));
        const Vector2 NewVertex = Dir * Radius + Position;
        OutVertices.push_back(NewVertex);

    }
}

Shape::Type Circle::ShapeType = Shape::Circle;

#include "circle.h"

Circle::Circle() : BaseShape(Shape::Circle)
{
}

Circle::Circle(const float InRadius)
: BaseShape(Shape::Circle)
, Radius(InRadius)
{
}

void Circle::SetRadius(const float InRadius)
{
    Radius = InRadius;
    std::vector<int> tests;
}

void Circle::GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& WorldTM) const
{
    const int NumVertices = 32;
    const Vector2& Position = WorldTM.Position;
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

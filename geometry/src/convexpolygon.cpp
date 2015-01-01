#include "convexpolygon.h"

ConvexPolygon::ConvexPolygon() : BaseShape(Shape::ConvexPolygon)
{
}

ConvexPolygon::ConvexPolygon(Shape::Type ShapeType) : BaseShape(ShapeType)
{
}

void ConvexPolygon::AddVertex(const Vector2& V)
{
    Vertices.push_back(V);
}

void ConvexPolygon::ComputeWorldPositionsAndNormals(std::vector<Vector2>& OutPositions, std::vector<Vector2>& OutNormals, const Transform& WorldTM) const
{
    ComputeWorldPositions(OutPositions, WorldTM);
    ComputeNormals(OutNormals, WorldTM);
}

void ConvexPolygon::ComputeWorldPositions(std::vector<Vector2>& OutPositions, const Transform& WorldTM) const
{
    for(const Vector2& V : Vertices)
    {
        OutPositions.push_back(WorldTM.TransformPosition(V));
    }   
}

void ConvexPolygon::ComputeNormals(std::vector<Vector2>& OutNormals, const Transform& WorldTM) const
{
    const size_t NumVertices = Vertices.size();
    if(NumVertices >= 2)
    {
        for(size_t VertexIdx = 1; VertexIdx < NumVertices; ++VertexIdx)
        {
            const Vector2 Edge = Vertices[VertexIdx] - Vertices[VertexIdx-1];
            const Vector2 Normal = -Edge.GetPerp().GetSafeNormal();
            const Vector2 WorldNormal = WorldTM.TransformVector(Normal);
            OutNormals.push_back(WorldNormal);
        }

        const Vector2 Edge = Vertices[0] - Vertices[NumVertices-1];
        const Vector2 Normal = -Edge.GetPerp().GetSafeNormal();
        const Vector2 WorldNormal = WorldTM.TransformVector(Normal);
        OutNormals.push_back(WorldNormal);
    }
}

void ConvexPolygon::GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& TM) const
{
    for(const Vector2& V : Vertices)
    {
        OutVertices.push_back(TM.TransformPosition(V));
    }
}
Shape::Type ConvexPolygon::ShapeType = Shape::ConvexPolygon;

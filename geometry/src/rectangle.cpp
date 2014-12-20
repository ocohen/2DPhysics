#include "rectangle.h"

Rectangle::Rectangle()
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

void ProjectionSwap(const float Projection, float& Max, float &Min)
{
    if(Projection > Max)
    {
        Max = Projection;
    }

    if(Projection < Min)
    {
        Min = Projection;
    }
}

void MinMaxForNormal(const Vector2& Normal, const Vector2 Pts[8], float& Min1, float& Max1, float& Min2, float& Max2)
{
    Max1 = Vector2::Dot(Pts[0],Normal);
    Min1 = Max1;
    Max2 = Vector2::Dot(Pts[4],Normal);
    Min2 = Max2;

    for(int Count= 1; Count< 4; ++Count)
    {
        const int PtIdx1 = Count;
        const float Projection1 = Vector2::Dot(Pts[PtIdx1],Normal);
        Max1 = Projection1 > Max1 ? Projection1 : Max1;
        Min1 = Projection1 < Min1 ? Projection1 : Min1;

        const int PtIdx2 = Count+4;
        const float Projection2 = Vector2::Dot(Pts[PtIdx2],Normal);
        Max2 = Projection2 > Max2 ? Projection2 : Max2;
        Min2 = Projection2 < Min2 ? Projection2 : Min2;
    }
}

bool Rectangle::OverlapTest(const Rectangle& Other) const
{
    //use SAT
    Vector2 Normals[4];
    Normals[0] = TM.TransformVector(Vector2(1,0));
    Normals[1] = Normals[0].GetPerp();
    Normals[2] = Other.TM.TransformVector(Vector2(1,0));
    Normals[3] = Normals[2].GetPerp();

    Vector2 Pts[8];
    Pts[0] = TM.TransformPosition(Vector2(-Extents.X, -Extents.Y));
    Pts[1] = TM.TransformPosition(Vector2(Extents.X, -Extents.Y));
    Pts[2] = TM.TransformPosition(Vector2(Extents.X, Extents.Y));
    Pts[3] = TM.TransformPosition(Vector2(-Extents.X, Extents.Y));
    Pts[4] = Other.TM.TransformPosition(Vector2(-Other.Extents.X, -Other.Extents.Y));
    Pts[5] = Other.TM.TransformPosition(Vector2(Other.Extents.X, -Other.Extents.Y));
    Pts[6] = Other.TM.TransformPosition(Vector2(Other.Extents.X, Other.Extents.Y));
    Pts[7] = Other.TM.TransformPosition(Vector2(-Other.Extents.X, Other.Extents.Y));

    for(const Vector2& N : Normals)
    {
        float Min1,Max1,Min2,Max2;
        MinMaxForNormal(N, Pts, Min1, Max1, Min2, Max2);

        if(Min2 > Max1 || Min1 > Max2)
        {
            return false;
        }
    }

    return true;
}

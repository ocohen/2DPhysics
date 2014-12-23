#include "baseshape.h"
#include "circle.h"
#include "rectangle.h"
#include "shapeoverlap.h"
#include <cassert>

BaseShape::BaseShape(const Shape::Type InType)
: ShapeType(InType)
{
}


bool CircleCircleOverlapTest(const Circle& A, const Transform& ATM, const Circle& B, const Transform& BTM, ShapeOverlap* Overlap)
{
    const Vector2 AToB = BTM.Position - ATM.Position;
    const float Distance2 = AToB.Length2();
    const float Radii = (A.Radius + B.Radius);
    const bool bOverlapping = Distance2 <= Radii * Radii;

    if(bOverlapping && Overlap)
    {
        Overlap->A = &A;
        Overlap->B = &B;
        Overlap->MTD = AToB.GetSafeNormal();
        Overlap->PenetrationDepth = std::max(Radii - sqrtf(Distance2), 0.f);
    }

    return bOverlapping;

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

bool RectangleRectangleOverlapTest(const Rectangle& A, const Transform& ATM, const Rectangle& B, const Transform& BTM)
{
    //use SAT
    Vector2 Normals[4];
    Normals[0] = ATM.TransformVector(Vector2(1,0));
    Normals[1] = Normals[0].GetPerp();
    Normals[2] = BTM.TransformVector(Vector2(1,0));
    Normals[3] = Normals[2].GetPerp();


    Vector2 Pts[8];
    Pts[0] = ATM.TransformPosition(Vector2(-A.Extents.X, -A.Extents.Y));
    Pts[1] = ATM.TransformPosition(Vector2(A.Extents.X, -A.Extents.Y));
    Pts[2] = ATM.TransformPosition(Vector2(A.Extents.X, A.Extents.Y));
    Pts[3] = ATM.TransformPosition(Vector2(-A.Extents.X, A.Extents.Y));
    Pts[4] = BTM.TransformPosition(Vector2(-B.Extents.X, -B.Extents.Y));
    Pts[5] = BTM.TransformPosition(Vector2(B.Extents.X, -B.Extents.Y));
    Pts[6] = BTM.TransformPosition(Vector2(B.Extents.X, B.Extents.Y));
    Pts[7] = BTM.TransformPosition(Vector2(-B.Extents.X, B.Extents.Y));

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

float AABBPointDistance2(const Vector2& Min, const Vector2& Max, const Vector2& P)
{
    /** Real time collision detection pg 131*/
    float Distance2 = 0.f;
    if(P.X < Min.X) Distance2 += (P.X - Min.X) * (P.X - Min.X);
    if(P.Y < Min.Y) Distance2 += (P.Y - Min.Y) * (P.Y - Min.Y);
    if(P.X > Max.X) Distance2 += (P.X - Max.X) * (P.X - Max.X);
    if(P.Y > Max.Y) Distance2 += (P.Y - Max.Y) * (P.Y - Max.Y);

    return Distance2;
}

bool RectangleCircleOverlapTest(const Rectangle& Rect, const Transform& RectTM, const Circle& Circ, const Transform& CircTM)
{
    //we convert the circle into the rectangle's local space. This allows for a cheap AABB distance check
    const Transform RectInv = RectTM.GetInverse();
    const Vector2 LocalOffset = CircTM.Position - RectTM.Position;
    const Vector2 CircLocalP = RectInv.TransformVector(LocalOffset);
    const float Distance2 = AABBPointDistance2(-Rect.Extents, Rect.Extents, CircLocalP);
    return Distance2 <= Circ.Radius*Circ.Radius;
}

bool BaseShape::OverlapTest(const BaseShape&A, const Transform& ATM, const BaseShape& B, const Transform& BTM, ShapeOverlap* Overlap)
{
    const Shape::Type AType = A.GetType();
    const Shape::Type BType = B.GetType();

    if(AType == BType && AType == Shape::Circle)
    {
        return CircleCircleOverlapTest(*A.Get<Circle>(), ATM, *B.Get<Circle>(), BTM, Overlap);
    }

    if(AType == BType && AType == Shape::Rectangle)
    {
        return RectangleRectangleOverlapTest(*A.Get<Rectangle>(), ATM, *B.Get<Rectangle>(), BTM);
    }

    if(AType == Shape::Circle && BType == Shape::Rectangle)
    {
        return RectangleCircleOverlapTest(*B.Get<Rectangle>(), BTM, *A.Get<Circle>(), ATM);
    }

    if(BType == Shape::Circle && AType == Shape::Rectangle)
    {
        return RectangleCircleOverlapTest(*A.Get<Rectangle>(), ATM, *B.Get<Circle>(), BTM);
    }
    
    assert(false && "Unsupported overlap test between two shapes");
}

#include "baseshape.h"
#include "circle.h"
#include "rectangle.h"
#include "shapeoverlap.h"
#include "contactmanifold.h"
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

bool RectangleRectangleOverlapTest(const Rectangle& A, const Transform& ATM, const Rectangle& B, const Transform& BTM, ShapeOverlap* OverlapResult)
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

    float MinOverlap = std::numeric_limits<float>::max();
    int MinIdx = -1;
    for(int NormalIdx = 0; NormalIdx < 4; ++NormalIdx)
    {
        const Vector2& N = Normals[NormalIdx].GetPerp(); 
        float Min1,Max1,Min2,Max2;
        MinMaxForNormal(N, Pts, Min1, Max1, Min2, Max2);

        if(Min2 > Max1 || Min1 > Max2)
        {
            return false;
        }

        const float Overlap = std::min((Max2 - Min1), (Max1 - Min2));
        if(Overlap < MinOverlap)
        {
            MinOverlap = Overlap;
            MinIdx = NormalIdx;
        }
    }

    if(OverlapResult)
    {
        OverlapResult->A = &A;
        OverlapResult->B = &B;

        const Vector2& MTD = Normals[MinIdx].GetPerp();

        OverlapResult->MTD = Vector2::Dot(MTD, BTM.Position - ATM.Position) > 0.f ? MTD : -MTD;
        OverlapResult->PenetrationDepth = MinOverlap;
    }

    return true;
}

Vector2 AABBClosestPointOnBorder(const Vector2& Min, const Vector2& Max, const Vector2& P)
{
    const float X = Clamp(P.X, Min.X, Max.X);
    const float Y = Clamp(P.Y, Min.Y, Max.Y);

    if(InRange(P.X, Min.X, Max.X) && InRange(P.Y, Min.Y, Max.Y))
    {
        //we never want internal points
        const float MinX = std::min(P.X - Min.X, Max.X - P.X);
        const float MinY = std::min(P.Y - Min.Y, Max.Y - P.Y);
        const bool bMinX = (P.X - Min.X) < (Max.X - P.X);
        const bool bMinY = (P.Y - Min.Y) < (Max.Y - P.Y);
        const bool bClampX = MinX > MinY;

        return Vector2( bClampX ? X : (bMinX ? Min.X : Max.X), bClampX ? (bMinY ? Min.Y : Max.Y) : Y);
    }

    return Vector2(X,Y);
}

bool RectangleCircleOverlapTest(const Rectangle& Rect, const Transform& RectTM, const Circle& Circ, const Transform& CircTM, ShapeOverlap* OverlapResult)
{
    //we convert the circle into the rectangle's local space. This allows for a cheap AABB distance check
    const Transform RectInv = RectTM.GetInverse();
    const Vector2 LocalOffset = CircTM.Position - RectTM.Position;
    const Vector2 CircLocalP = RectInv.TransformVector(LocalOffset);
    const Vector2 ClosestPoint = AABBClosestPointOnBorder(-Rect.Extents, Rect.Extents, CircLocalP);
    const Vector2 ClosestPointToCirc= CircLocalP - ClosestPoint;
    const float Distance2 = ClosestPointToCirc.Length2();
    const bool bOverlap = Distance2 <= Circ.Radius*Circ.Radius;

    if(bOverlap && OverlapResult)
    {
        OverlapResult->A = &Rect;
        OverlapResult->B = &Circ;

        Vector2 MTD = RectTM.TransformVector(ClosestPointToCirc);
        MTD.SafeNormalize();
        const bool bFlipMTD = Vector2::Dot(MTD, CircTM.Position - RectTM.Position) < 0.f;
        
        OverlapResult->MTD = bFlipMTD ? -MTD : MTD;
        OverlapResult->PenetrationDepth = bFlipMTD ? Circ.Radius + sqrtf(Distance2) : Circ.Radius - sqrtf(Distance2);
    }

    return bOverlap;
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
        return RectangleRectangleOverlapTest(*A.Get<Rectangle>(), ATM, *B.Get<Rectangle>(), BTM, Overlap);
    }

    if(AType == Shape::Circle && BType == Shape::Rectangle)
    {
        return RectangleCircleOverlapTest(*B.Get<Rectangle>(), BTM, *A.Get<Circle>(), ATM, Overlap);
    }

    if(BType == Shape::Circle && AType == Shape::Rectangle)
    {
        return RectangleCircleOverlapTest(*A.Get<Rectangle>(), ATM, *B.Get<Circle>(), BTM, Overlap);
    }
    
    assert(false && "Unsupported overlap test between two shapes");
}

void CircleCircleContactInfo(const Circle&,const Transform&, const Circle& B, const Transform& BTM, const Vector2& MTD, const float PenetrationDepth, Contact& ContactOut)
{
    ContactOut.Normal = MTD;
    ContactOut.PenetrationDepth = PenetrationDepth;
    ContactOut.Position = BTM.Position - MTD * B.Radius;
}

void BaseShape::GenerateContactInfo(const BaseShape& A, const Transform& ATM, const BaseShape&B, const Transform& BTM, const Vector2& MTD, const float PenetrationDepth, Contact& ContactOut)
{
    const Shape::Type AType = A.GetType();
    const Shape::Type BType = B.GetType();

    if(AType == BType && AType == Shape::Circle)
    {
        CircleCircleContactInfo(*A.Get<Circle>(), ATM, *B.Get<Circle>(), BTM, MTD, PenetrationDepth, ContactOut);
        return;
    }

    assert(false && "Unsupported overlap test between two shapes");
}

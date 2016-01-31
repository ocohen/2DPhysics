#include "baseshape.h"
#include "circleshape.h"
#include "rectangleshape.h"
#include "shapeoverlap.h"
#include "contactmanifold.h"
#include <cassert>

BaseShape::BaseShape(const Shape::Type InType)
: ShapeType(InType)
{
}


bool CircleShapeCircleShapeOverlapTest(const CircleShape& A, const Transform& ATM, const CircleShape& B, const Transform& BTM, ShapeOverlap* Overlap)
{
    const Vector2 AToB = BTM.Position - ATM.Position;
    const float Distance2 = AToB.Length2();
    const float Radii = (A.Radius + B.Radius);
    const bool bOverlapping = Distance2 <= Radii * Radii;

    if(bOverlapping && Overlap)
    {
        Overlap->MTD = AToB.GetSafeNormal();
        Overlap->PenetrationDepth = std::max(Radii - sqrtf(Distance2), 0.f);
    }

    return bOverlapping;

}

void MinMaxForNormal(const Vector2& Normal, const Vector2 Pts[], int NumPts, float& Min, float& Max)
{
    Max = Vector2::Dot(Pts[0],Normal);
    Min = Max;

    for(int PtIdx= 1; PtIdx< NumPts; ++PtIdx)
    {
        const float Projection = Vector2::Dot(Pts[PtIdx],Normal);
        Max = std::max(Max, Projection);
        Min = std::min(Min, Projection);
    }
}

bool ConvexConvexOverlapTest(const ConvexPolygon& A, const Transform& ATM, const ConvexPolygon& B, const Transform& BTM, ShapeOverlap* OverlapResult)
{
    //use SAT
    std::vector<Vector2> Positions;
    std::vector<Vector2> Normals;
    A.ComputeWorldPositionsAndNormals(Positions, Normals, ATM);
    B.ComputeWorldPositionsAndNormals(Positions, Normals, BTM);

    const int NumAVertices = A.GetNumVertices();
    const int NumBVertices = B.GetNumVertices();
    const int NumVertices = NumAVertices + NumBVertices;

    float MinOverlap = std::numeric_limits<float>::max();
    int MinIdx = -1;
    for(int NormalIdx = 0; NormalIdx < NumVertices; ++NormalIdx)
    {
        const Vector2& N = Normals[NormalIdx]; 
        float Min1,Max1,Min2,Max2;
        MinMaxForNormal(N, &Positions[0], NumAVertices, Min1, Max1);
        MinMaxForNormal(N, &Positions[NumAVertices], NumBVertices, Min2, Max2);

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
        const Vector2& MTD = Normals[MinIdx];

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

template <bool FlipAB = false>
bool RectangleShapeCircleShapeOverlapTest(const RectangleShape& Rect, const Transform& RectTM, const CircleShape& Circ, const Transform& CircTM, ShapeOverlap* OverlapResult)
{
    //we convert the CircleShape into the RectangleShape's local space. This allows for a cheap AABB distance check
    const Transform RectInv = RectTM.GetInverse();
    const Vector2 LocalOffset = CircTM.Position - RectTM.Position;
    const Vector2 CircLocalP = RectInv.TransformVector(LocalOffset);
    const Vector2 ClosestPoint = AABBClosestPointOnBorder(-Rect.Extents, Rect.Extents, CircLocalP);
    const Vector2 ClosestPointToCirc= CircLocalP - ClosestPoint;
    const float Distance2 = ClosestPointToCirc.Length2();
    const bool bOverlap = Distance2 <= Circ.Radius*Circ.Radius;

    if(bOverlap && OverlapResult)
    {
        Vector2 MTD = RectTM.TransformVector(ClosestPointToCirc);
        MTD.SafeNormalize();
        const bool bFlipMTD = Vector2::Dot(MTD, CircTM.Position - RectTM.Position) < 0.f;
        
        OverlapResult->MTD = bFlipMTD ? -MTD : MTD;
        OverlapResult->PenetrationDepth = bFlipMTD ? Circ.Radius + sqrtf(Distance2) : Circ.Radius - sqrtf(Distance2);

        if(FlipAB)
        {   //This is to account for when the CircleShape is actually A and the RectangleShape is actually B
            OverlapResult->MTD *= -1;
        }
    }

    return bOverlap;
}

bool BaseShape::OverlapTest(const BaseShape&A, const Transform& ATM, const BaseShape& B, const Transform& BTM, ShapeOverlap* Overlap)
{
    const Shape::Type AType = A.GetType();
    const Shape::Type BType = B.GetType();

    if(Overlap)
    {
        Overlap->A = &A;
        Overlap->B = &B;
        Overlap->AWorldTM = ATM;
        Overlap->BWorldTM = BTM;
    }

    if(AType == BType && AType == Shape::Circle)
    {
        return CircleShapeCircleShapeOverlapTest(*A.Get<CircleShape>(), ATM, *B.Get<CircleShape>(), BTM, Overlap);
    }

    if(AType == BType && (AType == Shape::Rectangle))
    {
        return ConvexConvexOverlapTest(*A.Get<RectangleShape>(), ATM, *B.Get<RectangleShape>(), BTM, Overlap);
    }

    if(AType == Shape::Circle && BType == Shape::Rectangle)
    {
        return RectangleShapeCircleShapeOverlapTest<true>(*B.Get<RectangleShape>(), BTM, *A.Get<CircleShape>(), ATM, Overlap);
    }

    if(BType == Shape::Circle && AType == Shape::Rectangle)
    {
        return RectangleShapeCircleShapeOverlapTest(*A.Get<RectangleShape>(), ATM, *B.Get<CircleShape>(), BTM, Overlap);
    }
    
    assert(false && "Unsupported overlap test between two shapes");

	return false;
}

void GenerateCircleShapeCircleShapeManifold(const CircleShape&,const Transform&, const CircleShape& B, const Transform& BTM, const Vector2& MTD, const float PenetrationDepth, ContactManifold& ManifoldOut)
{
    ManifoldOut.NumContacts = 1;
    Contact& ConctactPoint = ManifoldOut.ContactPoints[0];
    ConctactPoint.Normal = MTD;
    ConctactPoint.PenetrationDepth = PenetrationDepth;
    ConctactPoint.Position = BTM.Position - MTD * B.Radius;
}

/* Given a Dir and a set of vertices, find the closest two vertices */
void GetTwoClosest(const Vector2& Dir, const Vector2 Vertices[], const int NumVertices, int& Closest, int& Second)
{
    float MaxProjection = -std::numeric_limits<float>::max();
    float SecondMaxProjection = -std::numeric_limits<float>::max();
    int MaxIdx = -1;
    int SecondMaxIdx = -1;
    for(int VertIdx = 0; VertIdx < NumVertices; ++VertIdx)
    {
        const Vector2& V = Vertices[VertIdx];
        const float Projection = Vector2::Dot(V, Dir);
        if(Projection > SecondMaxProjection)
        {
            SecondMaxProjection = Projection;
            SecondMaxIdx = VertIdx;
        }
           
        if(Projection > MaxProjection)
        {
            SecondMaxProjection = MaxProjection;
            SecondMaxIdx = MaxIdx;
            MaxProjection = Projection;
            MaxIdx = VertIdx;
        }
    }

    Closest = MaxIdx;
    Second = SecondMaxIdx;
}

/** Will ensure B1 and B2 are within the halfplane defined by the A1A2 edge and will clip B1 or B2 if needed.*/
void Clip(const Vector2& A1, const Vector2& A2, Vector2& B1, Vector2& B2)
{
    const Vector2 A1A2Dir = (A2 - A1).GetSafeNormal();
    const Vector2 A1B1 = B1 - A1;
    const Vector2 A1B2 = B2 - A1;

    const float A1B1Proj = Vector2::Dot(A1B1, A1A2Dir);
    const float A1B2Proj = Vector2::Dot(A1B2, A1A2Dir);

    if(A1B1Proj < 0)
    {
        const Vector2 A1B1Projected = (A1 + (A1A2Dir * A1B1Proj));
        const Vector2 A1ToA1B1Projected = A1B1Projected - A1;
        const Vector2 A1B1ProjectedToB1 = A1B1 - A1ToA1B1Projected;
        B1 = A1 + A1B1ProjectedToB1;
    }

    if(A1B2Proj < 0)
    {
        const Vector2 A1B2Projected = (A1 + (A1A2Dir * A1B2Proj));
        const Vector2 A1ToA1B2Projected = A1B2Projected - A1;
        const Vector2 A1B2ProjectedToB2 = A1B2 - A1ToA1B2Projected;
        B2 = A1 + A1B2ProjectedToB2;
    }
}

void GenerateConvexConvexManifold(const ConvexPolygon& A,const Transform& ATM, const ConvexPolygon& B, const Transform& BTM, const Vector2& MTD, const float PenetrationDepth, ContactManifold& OutManifold)
{
    //We are going to clip the closest incident vertices (B) so that they are within the halfplane defined by the closest vertices of A

    std::vector<Vector2> Positions;
    A.ComputeWorldPositions(Positions, ATM);
    B.ComputeWorldPositions(Positions, BTM);

    const int NumAVertices = A.GetNumVertices();
    const int NumBVertices = B.GetNumVertices();

    int AV1Idx, AV2Idx;
    GetTwoClosest(MTD, &Positions[0], NumAVertices, AV1Idx, AV2Idx);
    assert(AV1Idx >= 0 && AV2Idx >= 0);
    const Vector2& AV1 = Positions[AV1Idx];
    const Vector2& AV2 = Positions[AV2Idx];

    int BV1Idx, BV2Idx;
    GetTwoClosest(-MTD, &Positions[NumAVertices], NumBVertices, BV1Idx, BV2Idx);
    assert(BV1Idx >= 0 && BV2Idx >= 0);
    Vector2 BV1 = Positions[BV1Idx + NumAVertices];
    Vector2 BV2 = Positions[BV2Idx + NumAVertices];

    Clip(AV1, AV2, BV1, BV2);
    Clip(AV2, AV1, BV1, BV2);

    //now we cull out anything that's on the wrong end of the MTD
    const bool bBV1Valid = Vector2::Dot(BV1 - AV1, -MTD) >= 0;
    const bool bBV2Valid = Vector2::Dot(BV2 - AV1, -MTD) >= 0;

    OutManifold.NumContacts = 0;
    if(bBV1Valid)
    {
        Contact& NewContact = OutManifold.ContactPoints[OutManifold.NumContacts++];
        NewContact.Position = BV1;
        NewContact.PenetrationDepth = PenetrationDepth;
        NewContact.Normal = MTD;
    }

    if(bBV2Valid)
    {
        Contact& NewContact = OutManifold.ContactPoints[OutManifold.NumContacts++];
        NewContact.Position = BV2;
        NewContact.PenetrationDepth = PenetrationDepth;
        NewContact.Normal = MTD;
    }

    

}

template<bool FlipAB = false>
void GenerateConvexCircleShapeManifold(const ConvexPolygon&, const Transform&, const CircleShape& B, const Transform& BTM, const Vector2& MTD, const float PenetrationDepth, ContactManifold& OutManifold)
{
    OutManifold.NumContacts = 1;
    Contact& ContactPoint = OutManifold.ContactPoints[0];
    ContactPoint.Normal = MTD;
    ContactPoint.PenetrationDepth = PenetrationDepth;
    if(FlipAB == false)
    {
        ContactPoint.Position = BTM.Position - (MTD * B.Radius);
    }else
    {
        ContactPoint.Position = BTM.Position + (MTD * B.Radius);
    }
}

void BaseShape::GenerateManifold(const ShapeOverlap& Overlap, ContactManifold& OutManifold)
{
    const BaseShape& A = *Overlap.A;
    const BaseShape& B = *Overlap.B;

    const Shape::Type AType = A.GetType();
    const Shape::Type BType = B.GetType();

    const Transform& ATM = Overlap.AWorldTM;
    const Transform& BTM = Overlap.BWorldTM;

    if(AType == BType && AType == Shape::Circle)
    {
        GenerateCircleShapeCircleShapeManifold(*A.Get<CircleShape>(), ATM, *B.Get<CircleShape>(), BTM, Overlap.MTD, Overlap.PenetrationDepth, OutManifold);
        return;
    }

    if(AType == BType && (AType == Shape::Rectangle))
    {
        GenerateConvexConvexManifold(*A.Get<RectangleShape>(), ATM, *B.Get<RectangleShape>(), BTM, Overlap.MTD, Overlap.PenetrationDepth, OutManifold);
        return;
    }

    if(AType == Shape::Rectangle && BType == Shape::Circle)
    {
        GenerateConvexCircleShapeManifold(*A.Get<RectangleShape>(), ATM, *B.Get<CircleShape>(), BTM, Overlap.MTD, Overlap.PenetrationDepth, OutManifold);
        return;
    }

    if(AType == Shape::Circle && BType == Shape::Rectangle)
    {
        GenerateConvexCircleShapeManifold<true>(*B.Get<RectangleShape>(), BTM, *A.Get<CircleShape>(), ATM, Overlap.MTD, Overlap.PenetrationDepth, OutManifold);
        return;
    }

    assert(false && "Unsupported overlap test between two shapes");

}

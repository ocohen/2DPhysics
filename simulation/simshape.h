#ifndef OC_SIMSHAPE_H
#define OC_SIMSHAPE_H

#include "baseshape.h"

class SimShape
{
public:
    SimShape(BaseShape* InGeometry);
    ~SimShape();

    void SetMass(const float InMass);
    float GetMass() const { return Mass; }

    void SetMomentOfInertia(const float InMOI);
    float GetMomentOfInertia() const { return MOI; }

    static bool OverlapTest(const SimShape& A, const Transform& AWorldTM, const SimShape& B, const Transform& BWorldTM, ShapeOverlap* OverlapResult=0)
    {
        return BaseShape::OverlapTest(*A.Geometry, AWorldTM, *B.Geometry, BWorldTM, OverlapResult);
    }

    void GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& WorldTM)
    {
        Geometry->GenerateRenderVertices(OutVertices, WorldTM);
    }

    const Vector2& GetLocalCOM() const { return LocalCOM; }

public:
    Transform LocalTM;
private:
    Vector2 LocalCOM;
    BaseShape* Geometry;
    float Mass;
    float MOI;
};

#endif

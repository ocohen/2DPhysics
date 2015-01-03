#include "simshape.h"

SimShape::SimShape(BaseShape* InGeometry)
: LocalCOM(Vector2::Zero)
, Geometry(InGeometry)
, Mass(1.f)
, MOI(1.f)
{
}

SimShape::~SimShape()
{
    delete Geometry;
}

void SimShape::SetMass(const float InMass)
{
    Mass = InMass;
}

void SimShape::SetMomentOfInertia(const float InMOI)
{
    MOI = InMOI;
}

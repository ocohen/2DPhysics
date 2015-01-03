#include "simshape.h"

SimShape::SimShape(BaseShape* InGeometry)
: Geometry(InGeometry)
, Mass(1.f)
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

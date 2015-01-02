#include "simshape.h"

SimShape::SimShape(BaseShape* InGeometry) : Geometry(InGeometry)
{
}

SimShape::~SimShape()
{
    delete Geometry;
}

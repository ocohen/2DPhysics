#ifndef OC_SHAPEOVERLAP_H
#define OC_SHAPEOVERLAP_H

#include "transform.h"

struct BaseShape;
struct ShapeOverlap
{
    const BaseShape* A;
    const BaseShape* B;
    /** Minimum translation direction. Unit length only provides direction. Direction is always from A to B so move B along MTD and move A along -MTD */
    Vector2 MTD;
    /** The amount of penetration along the MTD. Combine with MTD to get the minimum translation vector */
    float PenetrationDepth;

    Transform AWorldTM; //A's world transform used by the overlap query
    Transform BWorldTM; //B's world transform used by the overlap query

};

#endif

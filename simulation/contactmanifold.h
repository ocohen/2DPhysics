#ifndef OC_CONTACTMANIFOLD_H
#define OC_CONTACTMANIFOLD_H

#include "shapeoverlap.h"

class Actor;
struct ContactManifold
{
    Actor* A;
    Actor* B;
    ShapeOverlap Overlaps[4];
    int NumOverlaps;

    ContactManifold()
        : A(0), B(0), NumOverlaps(0) {}
};

#endif

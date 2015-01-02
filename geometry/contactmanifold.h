#ifndef OC_CONTACTMANIFOLD_H
#define OC_CONTACTMANIFOLD_H

#include "shapeoverlap.h"

class Actor;
struct Contact
{
    Vector2 Position;
    Vector2 Normal;
    float PenetrationDepth;
};

struct ContactManifold
{
    Actor* A;
    Actor* B;
    Contact ContactPoints[4];
    int NumContacts;

    ContactManifold()
        : A(0), B(0), NumContacts(0) {}
};

#endif

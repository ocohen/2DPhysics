#ifndef OC_ACTORSHAPE_H
#define OC_ACTORSHAPE_H

#include "baseshape.h"

struct ActorShape
{
    BaseShape* Shape;
    Transform LocalTM;

    ActorShape(BaseShape* InShape, const Transform& InLocalTM)
    : Shape(InShape)
    , LocalTM(InLocalTM)
    {
    }

    ~ActorShape()
    {
        delete Shape;
    }
};

#endif

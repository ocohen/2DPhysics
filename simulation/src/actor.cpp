#include "actor.h"
#include "baseshape.h"

Actor::Actor(World* InWorld)
: Pose(Transform::Identity)
, MyWorld(InWorld)
, bIsKinematic(false)
{
}

void Actor::SetKinematic(bool IsKinematic)
{
    bIsKinematic = IsKinematic;
    //TODO: update some structure in the scene
}

void Actor::SetPose(const Transform& InTransform)
{
    Pose = InTransform;
}

Actor::~Actor()
{
    for(BaseShape* Shape : Shapes)
    {
        delete Shape;
    }
}

#include "actor.h"
#include "baseshape.h"

Actor::Actor(World* InWorld)
: WorldTM(Transform::Identity)
, MyWorld(InWorld)
, bIsKinematic(false)
{
}

void Actor::SetKinematic(bool IsKinematic)
{
    bIsKinematic = IsKinematic;
    //TODO: update some structure in the scene
}

void Actor::SetWorldTransform(const Transform& InTransform)
{
    WorldTM = InTransform;
}

Actor::~Actor()
{
    for(ActorShape* Shape : ActorShapes)
    {
        delete Shape;
    }
}

bool Actor::OverlapTest(const Actor* B) const
{
    const Actor* A = this;
    for(const ActorShape* AShape : A->ActorShapes)
    {
        const Transform AWorldTM = WorldTM * AShape->LocalTM;
        for(const ActorShape* BShape : B->ActorShapes)
        {
            if(BaseShape::OverlapTest(*AShape->Shape, AWorldTM, *BShape->Shape, B->WorldTM * BShape->LocalTM))
            {
                return true;
            }
        }
    }

    return false;
}

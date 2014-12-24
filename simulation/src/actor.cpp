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
    for(BaseShape* Shape : Shapes)
    {
        delete Shape;
    }
}

bool Actor::OverlapTest(const Actor* B, std::vector<ShapeOverlap>* Overlaps) const
{
    const Actor* A = this;
    bool bOverlap = false;
    for(const BaseShape* AShape : A->Shapes)
    {
        const Transform AWorldTM = WorldTM * AShape->LocalTM;
        for(const BaseShape* BShape : B->Shapes)
        {
            if(Overlaps)
            {
                ShapeOverlap Overlap;
                if(BaseShape::OverlapTest(*AShape, AWorldTM, *BShape, B->WorldTM * BShape->LocalTM, &Overlap))
                {
                    Overlaps->push_back(Overlap);
                    bOverlap = true;
                }
            }else
            {
                if(BaseShape::OverlapTest(*AShape, AWorldTM, *BShape, B->WorldTM * BShape->LocalTM))
                {
                    return true;
                }
            }
        }
    }

    return bOverlap;
}

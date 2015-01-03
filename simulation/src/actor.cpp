#include "actor.h"
#include "baseshape.h"

Actor::Actor(World* InWorld)
: WorldTM(Transform::Identity)
, MyWorld(InWorld)
, bIsKinematic(false)
, LinearAcceleration(Vector2::Zero)
, LinearVelocity(Vector2::Zero)
, AngularAcceleration(0.f)
, AngularVelocity(0.f)
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
    for(SimShape* Shape : Shapes)
    {
        delete Shape;
    }
}

bool Actor::OverlapTest(const Actor* B, std::vector<ShapeOverlap>* Overlaps) const
{
    const Actor* A = this;
    bool bOverlap = false;
    for(const SimShape* AShape : A->Shapes)
    {
        const Transform AWorldTM = WorldTM * AShape->LocalTM;
        for(const SimShape* BShape : B->Shapes)
        {
            if(Overlaps)
            {
                ShapeOverlap Overlap;
                if(SimShape::OverlapTest(*AShape, AWorldTM, *BShape, B->WorldTM * BShape->LocalTM, &Overlap))
                {
                    Overlaps->push_back(Overlap);
                    bOverlap = true;
                }
            }else
            {
                if(SimShape::OverlapTest(*AShape, AWorldTM, *BShape, B->WorldTM * BShape->LocalTM))
                {
                    return true;
                }
            }
        }
    }

    return bOverlap;
}

void Actor::SetLinearVelocity(const Vector2& InVelocity)
{
    LinearVelocity = InVelocity;
}

void Actor::SetAngularVelocity(const float InAngularVelocity)
{
    AngularVelocity = InAngularVelocity;
}

void Actor::SetLinearAcceleration(const Vector2& InAcceleration)
{
    LinearAcceleration = InAcceleration;
}

void Actor::SetAngularAcceleration(const float InAcceleration)
{
    AngularAcceleration = InAcceleration;
}

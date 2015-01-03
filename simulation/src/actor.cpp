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
, InverseMass(1.f)
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

void Actor::AddForce(const Vector2& InForce)
{
    LinearAcceleration += (InForce * InverseMass);
}

void Actor::AddForceAtLocation(const Vector2& InForce, const Vector2& Location)
{
    AddForce(InForce);
    const Vector2 R = Location - LocalCOM;
    const float Torque = R.X*InForce.Y - R.Y*InForce.X;
    AddTorque(Torque);
}

void Actor::AddTorque(const float InTorque)
{
    AngularAcceleration += (InTorque * InverseMOI);
}

void Actor::UpdateMassAndInertia()
{
    float TotalMass = 0.f;
    Vector2 TotalCOMs = Vector2::Zero;
    for(SimShape* Shape : Shapes)
    {
        TotalMass += Shape->GetMass();
        TotalCOMs += Shape->GetMass() * Shape->LocalTM.TransformPosition(Shape->GetLocalCOM());
    }

    InverseMass = 1.f / TotalMass;
    LocalCOM = TotalCOMs * InverseMass;

    float TotalMOI = 0.f;
    for(SimShape* Shape : Shapes)
    {
        //use parallel axis theorem to compute shape's MOI if the COM was the same as the actor's COM
        const Vector2 ShapeCOM = Shape->LocalTM.TransformPosition(Shape->GetLocalCOM());
        const float Distance2 = (LocalCOM - ShapeCOM).Length2();
        const float ParallelMOI = Shape->GetMomentOfInertia() + (Shape->GetMass() * Distance2);
        TotalMOI += ParallelMOI;
    }

    InverseMOI = 1.f / TotalMOI;

}

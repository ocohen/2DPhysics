#include "actor.h"
#include "baseshape.h"

Actor::Actor(World* InWorld)
: WorldTM(Transform::Identity)
, MyWorld(InWorld)
, bIsKinematic(false)
, LinearAcceleration(Vector2::Zero)
, LinearVelocity(Vector2::Zero)
, LocalCOM(Vector2::Zero)
, AngularAcceleration(0.f)
, AngularVelocity(0.f)
, InverseMass(1.f)
, Restitution(0.f)
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

void Actor::SetWorldPosition(const Vector2& InPosition)
{
    WorldTM.Position = InPosition;
}

void Actor::SetWorldRotation(const float InRotation)
{
    WorldTM.Rotation = InRotation;
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
    const Vector2 R = Location - WorldTM.TransformPosition(LocalCOM);
    const float Torque = R.X*InForce.Y - R.Y*InForce.X;
    AddTorque(Torque);
}

void Actor::AddTorque(const float InTorque)
{
    AngularAcceleration += (InTorque * InverseMOI);
}

void Actor::AddImpulse(const Vector2& InImpulse)
{
    LinearVelocity += (InImpulse * InverseMass);
}

void Actor::AddImpulseAt(const Vector2& InImpulse, const Vector2& Location)
{
    AddImpulse(InImpulse);
    const Vector2 R = Location - GetWorldCOM();
    const float RotationalImpulse = R.X*InImpulse.Y - R.Y*InImpulse.X;
    AddRotationalImpulse(RotationalImpulse);
}

void Actor::AddRotationalImpulse(const float RotationalImpulse)
{
    AngularVelocity += (RotationalImpulse * InverseMOI);
}

void Actor::CalculateMass()
{
    float TotalMass = 0.f;
    for(SimShape* Shape : Shapes)
    {
        TotalMass += Shape->GetMass();
    }

    InverseMass = 1.f / TotalMass;
}

Vector2 Actor::GetLinearVelocityAt(const Vector2& Location) const
{
    const Vector2 COMToLocation = (Location - GetWorldCOM());
    const Vector2 TangentialVelocity = Vector2(-AngularVelocity * COMToLocation.Y, AngularVelocity * COMToLocation.X);
    const Vector2 TotalVelocity = LinearVelocity + TangentialVelocity;
    return TotalVelocity;
}

void Actor::CalculateCOM()
{
    Vector2 TotalCOMs = Vector2::Zero;
    for(SimShape* Shape : Shapes)
    {
        TotalCOMs += Shape->GetMass() * Shape->LocalTM.TransformPosition(Shape->GetLocalCOM());
    }

    LocalCOM = TotalCOMs * InverseMass;
}

void Actor::CalculateInertia()
{
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

void Actor::CalculateMassAndInertia()
{
    CalculateMass();
    CalculateInertia();

}

void Actor::CalculateMassInertiaAndCOM()
{
    CalculateMass();
    CalculateCOM();
    CalculateInertia();
}

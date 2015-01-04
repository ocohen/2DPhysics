#include "solver.h"
#include "actor.h"

void Solver::SolvePosition(Actor* A, Actor* B, const Contact& AContact)
{
    const Vector2& N = AContact.Normal;
    const float Depth = AContact.PenetrationDepth;

    //We want to move A and B Depth apart along N
    //We want to do this based on the mass ratio, so the heavier an object is the less moving it does

    const float AInvMass = A->GetInverseMass();
    const float BInvMass = B->GetInverseMass();
    const float ABOverAPlusB = 1.f / (AInvMass + BInvMass);
    const float ARatio = ABOverAPlusB * BInvMass;
    const float BRatio = ABOverAPlusB * AInvMass;

    //both actors should move away Depth/2
    const Vector2 DepthFix = Depth * N;
    const Vector2 ATarget = A->GetWorldPosition() - DepthFix * (1-ARatio);
    const Vector2 BTarget = B->GetWorldPosition() + DepthFix * (1-BRatio);

    A->SetWorldPosition(ATarget);
    B->SetWorldPosition(BTarget);

}

void Solver::SolveVelocity(Actor* A, Actor* B, const Contact& AContact)
{
    const Vector2& P = AContact.Position;
    const Vector2& N = AContact.Normal;

    const Vector2 Ua = N * Vector2::Dot(A->GetLinearVelocityAt(P), N);
    const Vector2 Ub = N * Vector2::Dot(B->GetLinearVelocityAt(P), N);
    const Vector2 RelVelocity = Ub - Ua;

    //Compute the impulse of A and B using Coefficient of restitution:
    //http://en.wikipedia.org/wiki/Coefficient_of_restitution
    //Restitution is taken as min. In theory we could add different combination modes like max,mult,avg,etc...
    const float Restitution = std::min(A->GetRestitution(), B->GetRestitution());

    if(A->IsKinematic() == false && B->IsKinematic() == false)
    {
        const float AMass = A->GetMass();
        const float BMass = B->GetMass();
        const float TotalMassInv = 1.f / (AMass + BMass);
        const Vector2 Pa = AMass * Ua;
        const Vector2 Pb = BMass * Ub;
        const Vector2 ATargetVelocity = (Pa + Pb + BMass * Restitution * RelVelocity) * TotalMassInv;
        const Vector2 BTargetVelocity = (Pa + Pb + AMass * Restitution * -RelVelocity) * TotalMassInv;

        A->AddImpulseAt((ATargetVelocity - Ua) * AMass, P);
        B->AddImpulseAt((BTargetVelocity - Ub) * BMass, P);
        
    }else
    {
        const Vector2 TargetVelocity = Restitution * RelVelocity;
        if(A->IsKinematic() == false)
        {
            const float AMass = A->GetMass();
            A->AddImpulseAt((TargetVelocity - Ua) * AMass, P);

        }else
        {
            const float BMass = B->GetMass();
            B->AddImpulseAt((-TargetVelocity-Ub) * BMass, P);
        } 
    }
}

void Solver::SolveContact(Actor* A, Actor* B, const Contact& AContact)
{
    SolveVelocity(A,B,AContact);
    SolvePosition(A,B,AContact);
}

void Solver::Solve(const std::vector<ContactManifold>& Manifolds)
{
    for(const ContactManifold& Manifold : Manifolds)
    {
        for(int ContactIdx = 0; ContactIdx < Manifold.NumContacts; ++ContactIdx)
        {
            SolveContact(Manifold.A, Manifold.B, Manifold.ContactPoints[ContactIdx]);
        }

    }
}

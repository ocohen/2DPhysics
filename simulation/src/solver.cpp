#include "solver.h"
#include "actor.h"

void Solver::SolvePosition(Actor* A, Actor* B, const Contact& AContact)
{
    if(A->IsKinematic() == false)
    {
        A->SetWorldPosition(A->GetWorldPosition() - 0.5f*AContact.PenetrationDepth * AContact.Normal);
    }

    if(B->IsKinematic() == false)
    {
        B->SetWorldPosition(B->GetWorldPosition() + 0.5f*AContact.PenetrationDepth * AContact.Normal);
    }
    return;
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

#include <iostream>

void Solver::SolveVelocity(Actor* A, Actor* B, const Contact& AContact)
{
    //const Vector2& P = AContact.Position;
    const Vector2& N = AContact.Normal;

    const float InvMA = A->GetInverseMass();
    const float InvMB = B->GetInverseMass();

    //Lambda = -J Vin * (J M^-1 J^T)^-1
    //J = [-N | N]
    //Vin = [LinearV A, LinearV B]
    //M^-1 = [I33 * InvMA | I33 * InvMB]
    //TODO: add angular velocity

    const float JVin = Vector2::Dot(B->GetLinearVelocity() - A->GetLinearVelocity(), N);
    const float JMInvJT = InvMA + InvMB;

    if(JMInvJT < SMALL_NUMBER)  //two kinematics
    {
        return;
    }

    const float Lambda = -JVin / JMInvJT;

    if(Lambda <= 0.f)
    {
        return;
    }

    if(!A->IsKinematic())
    {
        A->AddImpulse(-N * Lambda);
    }

    if(!B->IsKinematic())
    {
        B->AddImpulse(N * Lambda);
    }

#if 0
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
#endif
}

void Solver::SolveContact(Actor* A, Actor* B, const Contact& AContact)
{
    SolveVelocity(A,B,AContact);
    SolvePosition(A,B,AContact);
}

void Solver::Solve(const std::vector<ContactManifold>& Manifolds)
{
    for(int i=0; i<4; i++)
    {
        for(const ContactManifold& Manifold : Manifolds)
        {
            for(int ContactIdx = 0; ContactIdx < Manifold.NumContacts; ++ContactIdx)
            {
                SolveVelocity(Manifold.A, Manifold.B, Manifold.ContactPoints[ContactIdx]);
            }

        }

    }

        for(const ContactManifold& Manifold : Manifolds)
        {
            for(int ContactIdx = 0; ContactIdx < Manifold.NumContacts; ++ContactIdx)
            {
                SolvePosition(Manifold.A, Manifold.B, Manifold.ContactPoints[ContactIdx]);
            }

        }
}

#include "world.h"
#include "actor.h"
#include "baseshape.h"
#include <cassert>
#include <set>

World::World()
{
}

Actor* World::CreateActor()
{
    Actor* NewActor = new Actor(this);
    Actors.push_back(NewActor);
    return NewActor;
}

void World::RemoveActor(Actor* InActor)
{
    for(Actor*& AnActor : Actors)
    {
        if(InActor == AnActor)
        {
            AnActor = 0;
            delete InActor;
            return;
        }
    }

    assert(false && "Trying to delete an actor that is already deleted");
}

World::~World()
{
    for(Actor* AnActor : Actors)
    {
        delete AnActor;
    }
}

void World::GenerateContactManifolds()
{
    ContactManifolds.clear();
    for(size_t AIdx = 0, NumActors = Actors.size(); AIdx < NumActors; ++AIdx)
    {
        Actor* A = Actors[AIdx];
        for(size_t BIdx = AIdx + 1, NumActors = Actors.size(); BIdx < NumActors; ++BIdx)
        {
           Actor* B = Actors[BIdx];
           std::vector<ShapeOverlap> Overlaps;
           if(A->OverlapTest(B, &Overlaps))
           {
               for(ShapeOverlap& Overlap : Overlaps)
               {
                   ContactManifold AManifold;
                   AManifold.A = A;
                   AManifold.B = B;
                   BaseShape::GenerateManifold(Overlap, AManifold);

                   ContactManifolds.push_back(AManifold);

               } 
            }
        }
    }
}

void World::Integrate(const float DeltaTime)
{
    for(Actor* A : Actors)
    {
        //leapfrog integreation: http://en.wikipedia.org/wiki/Leapfrog_integration
        const float DeltaTime2 = DeltaTime * DeltaTime;
        const Vector2 Offset = A->GetLinearVelocity() * DeltaTime + 0.5f * A->GetLinearAcceleration() * DeltaTime2;
        const float AngularOffset = A->GetAngularVelocity() * DeltaTime + 0.5f * A->GetAngularAcceleration() * DeltaTime2;

        const Transform& TM = A->GetWorldTransform();
        const Transform NewTM = Transform(TM.Position + Offset, TM.Rotation + AngularOffset);
        A->SetWorldTransform(NewTM);

        const Vector2 NewLinearVelocity = (A->GetLinearAcceleration() * DeltaTime) + A->GetLinearVelocity();
        const float NewAngularVelocity = (A->GetAngularAcceleration() * DeltaTime) + A->GetAngularVelocity();
        A->SetLinearVelocity(NewLinearVelocity);
        A->SetAngularVelocity(NewAngularVelocity);

    }
}

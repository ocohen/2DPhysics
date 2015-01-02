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

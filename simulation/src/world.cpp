#include "world.h"
#include "actor.h"
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

struct ManifoldKey
{
    Actor* A;
    Actor* B;

    ManifoldKey(Actor* InA, Actor* InB) : A(InA), B(InB){}
    bool operator==(const ManifoldKey& Other) const
    {
        return (A == Other.A && B == Other.B) || (A == Other.B && B == Other.A);
    }

    bool operator<(const ManifoldKey& Other) const
    {
        size_t Min = std::min((size_t)A,(size_t)B);
        size_t Max = std::max((size_t)A,(size_t)B);
        size_t OtherMin = std::min((size_t)Other.A,(size_t)Other.B);
        size_t OtherMax = std::max((size_t)Other.A,(size_t)Other.B);

        if(Max < OtherMax)
        {
            return true;
        }

        if(Max > OtherMax)
        {
            return false;
        }

        return Min < OtherMin;
    }
};

void World::GenerateContactManifolds()
{
    std::set<ManifoldKey> ManifoldDB;
    for(Actor* A : Actors)
    {
        for(Actor* B: Actors)
        {
            if(A != B)
            {
               std::vector<ShapeOverlap> Overlaps;
               if(A->OverlapTest(B, &Overlaps))
               {
                   ManifoldKey Key(A,B);
                   auto Result = ManifoldDB.find(Key);
                   if(Result == ManifoldDB.end())
                   {
                       ContactManifold AManifold;
                       AManifold.A = A;
                       AManifold.B = B;
                       AManifold.NumOverlaps = std::min(Overlaps.size(), sizeof(AManifold.Overlaps) / sizeof(ShapeOverlap));
                       for(int OverlapIdx = 0; OverlapIdx < AManifold.NumOverlaps; ++OverlapIdx)
                       {
                           AManifold.Overlaps[OverlapIdx] = Overlaps[OverlapIdx];
                       }

                       ContactManifolds.push_back(AManifold);
                       ManifoldDB.insert(Key);

                   }

               } 
            }
        }
    }
}

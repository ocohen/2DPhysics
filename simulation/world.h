#ifndef OC_WORLD_H
#define OC_WORLD_H

#include "contactmanifold.h"
#include <vector>

class Actor;
class World
{
public:
    World();
    ~World();
    Actor* CreateActor();
    void RemoveActor(Actor* InActor);
    const std::vector<Actor*>& GetAllActors() const { return Actors; }
    void Integrate(const float DeltaTime);

    void GenerateContactManifolds();
    const std::vector<ContactManifold>& GetContactManifolds() const { return ContactManifolds; }
private:
    std::vector<Actor*> Actors;
    std::vector<ContactManifold> ContactManifolds;
};

#endif

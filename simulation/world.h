#ifndef OC_WORLD_H
#define OC_WORLD_H

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
private:
    std::vector<Actor*> Actors;
};

#endif

#ifndef OC_ACTOR_H
#define OC_ACTOR_H

#include "transform.h"
#include "actorshape.h"
#include <vector>

struct BaseShape;
class World;
class Actor
{
public:
    Actor(World* InWorld);
    ~Actor();
    void SetKinematic(bool IsKinematic);
    bool IsKinematic() const { return bIsKinematic; }
    World* GetWorld() const { return MyWorld; }
    void SetWorldTransform(const Transform& InTransform);
    const Transform& GetWorldTransform() const { return WorldTM; }
    template <typename T> T* CreateShape(const Transform& LocalTM = Transform::Identity);
    const std::vector<ActorShape*>& GetActorShapes() const { return ActorShapes; }
    bool OverlapTest(const Actor* Other) const;

private:
    Transform WorldTM;
    World* MyWorld;
    bool bIsKinematic;
    std::vector<ActorShape*> ActorShapes;
};

template <typename T> T* Actor::CreateShape(const Transform& LocalTM)
{
    T* NewShape = new T();
    ActorShape* AnActorShape = new ActorShape(NewShape, LocalTM);
    ActorShapes.push_back(AnActorShape);
    return NewShape;
}

#endif

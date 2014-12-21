#ifndef OC_ACTOR_H
#define OC_ACTOR_H

#include "transform.h"
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
    void SetPose(const Transform& InTransform);
    template <typename T> T* CreateShape();
    const std::vector<BaseShape*>& GetShapes() const { return Shapes; }

private:
    Transform Pose;
    World* MyWorld;
    bool bIsKinematic;
    std::vector<BaseShape*> Shapes;
};

template <typename T> T* Actor::CreateShape()
{
    T* NewShape = new T();
    Shapes.push_back(NewShape);
    return NewShape;
}

#endif

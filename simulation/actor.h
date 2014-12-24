#ifndef OC_ACTOR_H
#define OC_ACTOR_H

#include "transform.h"
#include "shapeoverlap.h"
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
    const std::vector<BaseShape*>& GetShapes() const { return Shapes; }
    bool OverlapTest(const Actor* Other, std::vector<ShapeOverlap>* Overlaps = 0) const;

private:
    Transform WorldTM;
    World* MyWorld;
    bool bIsKinematic;
    std::vector<BaseShape*> Shapes;
};

template <typename T> T* Actor::CreateShape(const Transform& LocalTM)
{
    T* NewShape = new T();
    NewShape->LocalTM = LocalTM;
    Shapes.push_back(NewShape);
    return NewShape;
}

#endif

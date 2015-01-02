#ifndef OC_ACTOR_H
#define OC_ACTOR_H

#include "transform.h"
#include "shapeoverlap.h"
#include "simshape.h"
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
    template <typename T> SimShape* CreateShape(const Transform& LocalTM = Transform::Identity);
    template <typename T> SimShape* CreateShape(const T& InGeometry, const Transform& LocalTM = Transform::Identity);
    const std::vector<SimShape*>& GetShapes() const { return Shapes; }
    bool OverlapTest(const Actor* Other, std::vector<ShapeOverlap>* Overlaps = 0) const;

private:
    Transform WorldTM;
    World* MyWorld;
    bool bIsKinematic;
    std::vector<SimShape*> Shapes;
};

template <typename T> SimShape* Actor::CreateShape(const Transform& LocalTM)
{
    T* NewShape = new T();
    SimShape* NewSimShape = new SimShape(NewShape);
    NewSimShape->LocalTM = LocalTM;
    Shapes.push_back(NewSimShape);
    return NewSimShape;
}

template <typename T> SimShape* Actor::CreateShape(const T& InGeometry, const Transform& LocalTM)
{
    T* NewShape = new T(InGeometry);
    SimShape* NewSimShape = new SimShape(NewShape);
    NewSimShape->LocalTM = LocalTM;
    Shapes.push_back(NewSimShape);
    return NewSimShape;
}

#endif

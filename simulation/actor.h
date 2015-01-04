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
    void SetWorldPosition(const Vector2& InPosition);
    const Vector2& GetWorldPosition() const { return WorldTM.Position; }
    void SetWorldRotation(const float InRotation);
    float GetWorldRotation() const { return WorldTM.Rotation; }

    template <typename T> SimShape* CreateShape(const Transform& LocalTM = Transform::Identity);
    template <typename T> SimShape* CreateShape(const T& InGeometry, const Transform& LocalTM = Transform::Identity);
    const std::vector<SimShape*>& GetShapes() const { return Shapes; }
    bool OverlapTest(const Actor* Other, std::vector<ShapeOverlap>* Overlaps = 0) const;

    /** velocity, acceleration, forces, impulses, etc...*/
    void SetLinearVelocity(const Vector2& InVelocity);
    const Vector2& GetLinearVelocity() const { return LinearVelocity; }
    void SetAngularVelocity(const float InAngularVelocity);
    float GetAngularVelocity() const { return AngularVelocity; }
    Vector2 GetLinearVelocityAt(const Vector2& Location) const;
    void SetLinearAcceleration(const Vector2& InAcceleration);
    const Vector2& GetLinearAcceleration() const { return LinearAcceleration; }
    void SetAngularAcceleration(const float InAcceleration);
    float GetAngularAcceleration() const { return AngularAcceleration; }
    void AddForce(const Vector2& InForce);
    void AddForceAtLocation(const Vector2& InForce, const Vector2& Location);
    void AddTorque(const float InTorque);
    void AddImpulse(const Vector2& InImpulse);
    void AddImpulseAt(const Vector2& InImpulse, const Vector2& Location);
    void AddRotationalImpulse(const float RotationalImpulse);

    /** mass COM inertia, etc... */
    float GetMass() const { return 1.f / InverseMass; }
    float GetInverseMass() const { return bIsKinematic ? 0.f : InverseMass; }
    float GetMomentOfInertia() const { return 1.f / InverseMOI; }
    float GetInverseMomentOfInertia() const { return bIsKinematic ? 0.f : InverseMOI; }
    void CalculateMassAndInertia();
    void CalculateMassInertiaAndCOM();
    void CalculateCOM();
    void CalculateMass();
    void CalculateInertia();
    const Vector2& GetLocalCOM() const { return LocalCOM; }
    Vector2 GetWorldCOM() const { return WorldTM.TransformPosition(LocalCOM); }

    /** physical materials. @TODO: move this into a different struct*/
    float GetRestitution() const { return Restitution; }
    void SetRestitution(const float InRestitution) { Restitution = InRestitution; }

private:
    Transform WorldTM;
    World* MyWorld;
    bool bIsKinematic;
    std::vector<SimShape*> Shapes;
    Vector2 LinearAcceleration;
    Vector2 LinearVelocity;
    Vector2 LocalCOM;
    float AngularAcceleration;
    float AngularVelocity;
    float InverseMass;
    float InverseMOI;
    float Restitution;

};

template <typename T> SimShape* Actor::CreateShape(const Transform& LocalTM)
{
    T* NewShape = new T();
    SimShape* NewSimShape = new SimShape(NewShape);
    NewSimShape->LocalTM = LocalTM;
    Shapes.push_back(NewSimShape);
    CalculateMassInertiaAndCOM();
    return NewSimShape;
}

template <typename T> SimShape* Actor::CreateShape(const T& InGeometry, const Transform& LocalTM)
{
    T* NewShape = new T(InGeometry);
    SimShape* NewSimShape = new SimShape(NewShape);
    NewSimShape->LocalTM = LocalTM;
    Shapes.push_back(NewSimShape);
    CalculateMassInertiaAndCOM();
    return NewSimShape;
}

#endif

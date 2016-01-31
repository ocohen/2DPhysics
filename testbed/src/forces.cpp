#include "test.h"
#include "transform.h"
#include "world.h"
#include "actor.h"
#include "circleshape.h"
#include "rectangleshape.h"
#include "renderer.h"

class ForcesTest : public Test
{
public:
    bool Init(int Argc, char** Argv)
    {
        Stiffness = 100.f;
        Damping = 10.f;
        if(Argc > 2)
        {
            Stiffness = atof(Argv[2]);
        }

        if(Argc > 3)
        {
            Damping = atof(Argv[3]);
        }

        CircleShape DefCircleShape(1.f);
        RectangleShape DefRectangleShape(Vector2(.2f,2.f));
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<RectangleShape>(DefRectangleShape);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<RectangleShape>(DefRectangleShape);
            AnActor->SetWorldTransform(Transform(Vector2(3.f, 0.f), 0));
            AnActor->CreateShape<CircleShape>(DefCircleShape, Transform(Vector2(0.f, 3.f)));
            AnActor->SetAngularVelocity(-PI*0.125);
            AnActor->SetKinematic(true);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->SetWorldTransform(Transform(Vector2(-4.f, -1.f)));
            AnActor->CreateShape<CircleShape>(DefCircleShape);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->SetWorldTransform(Transform(Vector2(-7.f, -1.f)));
            AnActor->CreateShape<CircleShape>(DefCircleShape);
        }

        return true;
    }

    bool Tick(Renderer& ARenderer, float DeltaTime)
    {
        const float DefaultColor[] = {1.f, 1.f, 1.f};
        const float OverlapColor[] = {1.f, 0.f, 0.f};
        const std::vector<Actor*>& Actors = AWorld.GetAllActors();

        AWorld.Integrate(DeltaTime);
        AWorld.GenerateContactManifolds();
        const std::vector<ContactManifold>& Manifolds = AWorld.GetContactManifolds();

        for(const ContactManifold& Manifold : Manifolds)
        {
            for(int ContactIdx = 0; ContactIdx < Manifold.NumContacts; ++ContactIdx)
            {
                const Contact& AContact = Manifold.ContactPoints[ContactIdx];

                //Apply force based on penetration depth and normal
                //use spring equation
                Vector2 Force = AContact.Normal * AContact.PenetrationDepth * (Stiffness + Damping / DeltaTime);
                if(Manifold.A->IsKinematic() == false)
                {
                    Manifold.A->AddForceAtLocation(-Force, AContact.Position);
                }
                if(Manifold.B->IsKinematic() == false)
                {
                    Manifold.B->AddForceAtLocation(Force, AContact.Position);
                }
            }
        }

        for(const Actor* AnActor : Actors)
        {
            bool bOverlap = false;
            for(const ContactManifold& Manifold : Manifolds)
            {
                if(AnActor == Manifold.A || AnActor == Manifold.B)
                {
                    bOverlap = true;
                }
            }

            ARenderer.DrawActor(AnActor, bOverlap ? OverlapColor : DefaultColor);
        }
        return true;
    }

private:
    World AWorld;
    float Stiffness;
    float Damping;
};
TEST(ForcesTest, "forces");

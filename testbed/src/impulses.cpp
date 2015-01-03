#include "test.h"
#include "transform.h"
#include "world.h"
#include "actor.h"
#include "circle.h"
#include "rectangle.h"
#include "renderer.h"

class ImpulsesTest : public Test
{
public:
    bool Init(int Argc, const char** Argv)
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

        Circle DefCircle(1.f);
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<Circle>(DefCircle);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<Circle>(DefCircle);
            AnActor->SetWorldTransform(Vector2(0.5f, -3.f));
        }
        
        Rectangle DefFloor(Vector2(10.f,.1f));
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<Rectangle>(DefFloor);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(0.f,-5)));
            AnActor->SetKinematic(true);
        }


        return true;
    }

    bool Tick(Renderer& ARenderer, float DeltaTime)
    {
        const std::vector<Actor*>& Actors = AWorld.GetAllActors();

        for(Actor* A : Actors)
        {
            if(A->IsKinematic() == false)
            {
                A->SetLinearAcceleration(Vector2(0,-9.f));
            }
        }

        AWorld.Integrate(DeltaTime);
        AWorld.GenerateContactManifolds();
        const std::vector<ContactManifold>& Manifolds = AWorld.GetContactManifolds();

        for(const ContactManifold& Manifold : Manifolds)
        {
            for(int ContactIdx = 0; ContactIdx < Manifold.NumContacts; ++ContactIdx)
            {
                Actor* A = Manifold.A;
                Actor* B = Manifold.B;
                const Contact& AContact = Manifold.ContactPoints[ContactIdx];

                //Apply impulse based on penetration depth and normal

                const Vector2 RelativeVelocity = B->GetLinearVelocityAt(AContact.Position) - A->GetLinearVelocityAt(AContact.Position);
                const float RelativeSpeedAlongNormal = Vector2::Dot(RelativeVelocity, AContact.Normal);

                if(RelativeSpeedAlongNormal < 0)    //only apply impulse of not separating
                {
                    Vector2 Impulse = -(1.f + 0.5f) * RelativeSpeedAlongNormal * AContact.Normal;
                    Impulse /= (A->IsKinematic() ? 0.f : 1.f/A->GetMass()) + (B->IsKinematic() ? 0.f : 1.f/B->GetMass());

                    if(A->IsKinematic() == false)
                    {
                        A->AddImpulseAt(-Impulse, AContact.Position);
                    }
                    if(B->IsKinematic() == false)
                    {
                        B->AddImpulseAt(Impulse, AContact.Position);
                    }
                }

            }
        }

        for(const Actor* AnActor : Actors)
        {
            ARenderer.DrawActor(AnActor);
        }
        return true;
    }

private:
    World AWorld;
    float Stiffness;
    float Damping;
};
TEST(ImpulsesTest, "impulses");

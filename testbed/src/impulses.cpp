#include "test.h"
#include "transform.h"
#include "world.h"
#include "actor.h"
#include "circle.h"
#include "rectangle.h"
#include "renderer.h"
#include "solver.h"

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

        Circle DefCircle(.3f);

        {
            float X = -4;
            float Y = 0;
            for(int i=0; i<43; ++i)
            {
                X += 0.7;
                if(X > 4)
                {
                    X = -4;
                    Y += 0.7f;
                }
                Actor* AnActor = AWorld.CreateActor();
                AnActor->CreateShape<Circle>(DefCircle);
                AnActor->SetWorldTransform(Vector2(X,Y));
                AnActor->SetRestitution(0.5f);
            }
        }

        Rectangle DefArm(Vector2(3.f,.1f));
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<Rectangle>(DefArm);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(0.f,-2.f)));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
            AnActor->SetAngularVelocity(PI*0.25f);
        }

        Rectangle DefFloor(Vector2(10.f,.1f));
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<Rectangle>(DefFloor);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(0.f,-5)));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<Rectangle>(DefFloor);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(0.f,5)));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<Rectangle>(DefFloor);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(5.f, 0.f), PI*0.5));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<Rectangle>(DefFloor);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(-5.f, 0.f), PI*0.5));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
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
        Solver ASolver;
        ASolver.Solve(Manifolds);

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

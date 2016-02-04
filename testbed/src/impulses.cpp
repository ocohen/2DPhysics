#include "test.h"
#include "transform.h"
#include "world.h"
#include "actor.h"
#include "circleshape.h"
#include "rectangleshape.h"
#include "renderer.h"
#include "solver.h"

class ImpulsesTest : public Test
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

        CircleShape DefCircleShape(.3f);

        {
            float X = -4;
            float Y = 2;
            for(int i=0; i<50; ++i)
            {
                X += 0.7f;
                if(X > 4)
                {
                    X = -3.3 + i % 4;
                    Y += 0.7f;
                }
                Actor* AnActor = AWorld.CreateActor();
                AnActor->CreateShape<CircleShape>(DefCircleShape);
                AnActor->SetWorldTransform(Vector2(X,Y));
                AnActor->SetRestitution(0.5f);
            }
        }

        RectangleShape DefArm(Vector2(3.f,.1f));
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<RectangleShape>(DefArm);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(0.f,-2.f), 0.3f));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
            //AnActor->SetAngularVelocity(PI*0.25f);
        }
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<RectangleShape>(DefArm);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(-2.f,1.0f), -0.6f));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
            //AnActor->SetAngularVelocity(PI*0.25f);
        }


        RectangleShape DefFloor(Vector2(10.f,.1f));
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<RectangleShape>(DefFloor);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(0.f,-5)));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<RectangleShape>(DefFloor);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(0.f,5)));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<RectangleShape>(DefFloor);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(5.f, 0.f), PI*0.5));
            AnActor->SetKinematic(true);
            AnActor->SetRestitution(1.f);
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<RectangleShape>(DefFloor);
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

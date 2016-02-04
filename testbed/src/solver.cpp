#include "test.h"
#include "transform.h"
#include "world.h"
#include "actor.h"
#include "circleshape.h"
#include "rectangleshape.h"
#include "renderer.h"
#include "solver.h"

class SolverTest : public Test
{
public:
    bool Init(int, char**)
    {
        CircleShape DefCircleShape(.3f);
        float X = 0;
        float Y = 5;
        for(int i=0; i<5; i++)
        {
            Y-= .1;
            X += 0.1;
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<CircleShape>(DefCircleShape);
            AnActor->SetWorldTransform(Vector2(X,Y));
            //if(i == 0){ AnActor->SetKinematic(true); }
        }

        RectangleShape DefFloor(Vector2(10.f,0.1f));
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->CreateShape<RectangleShape>(DefFloor);
            AnActor->CalculateMassInertiaAndCOM();
            AnActor->SetWorldTransform(Transform(Vector2(0.f,-5)));
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

        AWorld.IntegrateVelocity(DeltaTime);
        AWorld.GenerateContactManifolds();
        const std::vector<ContactManifold>& Manifolds = AWorld.GetContactManifolds();
        Solver ASolver;
        ASolver.Solve(Manifolds);

        AWorld.IntegratePosition(DeltaTime);

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
TEST(SolverTest, "solver");

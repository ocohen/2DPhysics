#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "world.h"
#include "actor.h"
#include "circle.h"
#include "rectangle.h"

TEST_CASE( "Actor", "[actor]" )
{
    World World1;
    
    {
        Actor* AnActor = World1.CreateActor();
        AnActor->SetKinematic(true);
        REQUIRE(AnActor->IsKinematic());

        const Transform TM(Vector2(1,2), PI*0.25f);
        AnActor->SetWorldTransform(TM);

        SimShape* ACircle = AnActor->CreateShape<Circle>();
        CHECK(ACircle);
        
        SimShape* ARectangle = AnActor->CreateShape<Rectangle>();
        CHECK(ARectangle);

        const std::vector<SimShape*>& Shapes = AnActor->GetShapes();
        CHECK(Shapes.size() == 2);
    }

    {
        Circle DefCircle(1.f);

        Actor* A = World1.CreateActor();
        SimShape* ACircle = A->CreateShape<Circle>(DefCircle);

        Actor* B = World1.CreateActor();
        SimShape* BCircle = B->CreateShape<Circle>(DefCircle, Transform(Vector2(2.f, 0.f)));

        const std::vector<SimShape*>& Shapes = B->GetShapes();
        REQUIRE(Shapes[0]->LocalTM.Position.X == Approx(2.f));

        CHECK(A->OverlapTest(B));

        B->SetWorldTransform(Transform(Vector2(0.5f, 0.f), 0.f));
        CHECK(A->OverlapTest(B) == false);

        Rectangle DefRectangle(Vector2(1.5f, 1.f));
        std::vector<ShapeOverlap> Overlaps;
        SimShape* BRectangle = B->CreateShape<Rectangle>(DefRectangle);
        REQUIRE(A->OverlapTest(B, &Overlaps));
        CHECK(Overlaps.size() == 1);
    }
}

TEST_CASE( "World", "[world]" )
{
    World AWorld;
    Actor* Actor1 = AWorld.CreateActor();
    CHECK(Actor1->GetWorld() == &AWorld);

    Actor* Actor2 = AWorld.CreateActor();
    CHECK(Actor2->GetWorld() == &AWorld);
    
    const std::vector<Actor*>& Actors = AWorld.GetAllActors();
    CHECK(Actors.size() == 2);
}

TEST_CASE( "Integration", "[integration]" )
{

    const float Dt = 1.f / 60.f;
    {
        World AWorld;
        Actor* Ball = AWorld.CreateActor();
        Ball->CreateShape<Circle>();
        //No forces or velocity implies no movement
        for(int Step = 0; Step < 10; ++Step)
        {
            AWorld.Integrate(Dt);
        }

        const Transform TM = Ball->GetWorldTransform();
        CHECK(TM.Position.X == Approx(0.f));
        CHECK(TM.Position.Y == Approx(0.f));
        CHECK(TM.Rotation == Approx(0.f));
    }

    {
        World AWorld;
        Actor* Ball = AWorld.CreateActor();
        Ball->CreateShape<Circle>();
        //Constant velocity with no acceleration implies constant movement
        Ball->SetLinearVelocity(Vector2(1.f, 2.f));
        Ball->SetAngularVelocity(1.f);
        for(int Step = 0; Step < 60; ++Step)
        {
            AWorld.Integrate(Dt);
        }

        const Transform TM = Ball->GetWorldTransform();
        CHECK(TM.Position.X == Approx(1.f));
        CHECK(TM.Position.Y == Approx(2.f));
        CHECK(TM.Rotation == Approx(1.f));
    }


    {
        World AWorld;
        Actor* Ball = AWorld.CreateActor();
        Ball->CreateShape<Circle>();
        //Constant acceleration with no initial velocity
        Ball->SetLinearAcceleration(Vector2(1.f, 2.f));
        Ball->SetAngularAcceleration(1.f);
        for(int Step = 0; Step < 60; ++Step)
        {
            AWorld.Integrate(Dt);
        }

        const Transform TM = Ball->GetWorldTransform();
        CHECK(TM.Position.X == Approx(0.5f));
        CHECK(TM.Position.Y == Approx(1.f));
        CHECK(TM.Rotation == Approx(0.5f));
    }
}

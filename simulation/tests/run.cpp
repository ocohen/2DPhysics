#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "world.h"
#include "actor.h"
#include "circle.h"
#include "rectangle.h"

TEST_CASE( "Actor", "[actor]" )
{
    World World1;
    
    Actor* AnActor = World1.CreateActor();
    AnActor->SetKinematic(true);
    REQUIRE(AnActor->IsKinematic());

    const Transform TM(Vector2(1,2), PI*0.25f);
    AnActor->SetWorldTransform(TM);

    Circle* ACircle = AnActor->CreateShape<Circle>();
    CHECK(ACircle);
    
    Rectangle* ARectangle = AnActor->CreateShape<Rectangle>();
    CHECK(ARectangle);

    const std::vector<ActorShape*>& Shapes = AnActor->GetActorShapes();
    CHECK(Shapes.size() == 2);
    {
        Actor* A = World1.CreateActor();
        Circle* ACircle = A->CreateShape<Circle>();
        ACircle->SetRadius(1.f);

        Actor* B = World1.CreateActor();
        Circle* BCircle = B->CreateShape<Circle>(Transform(Vector2(2.f, 0.f)));
        BCircle->SetRadius(1.f);

        const std::vector<ActorShape*>& ActorShapes = B->GetActorShapes();
        REQUIRE(ActorShapes[0]->LocalTM.Position.X == Approx(2.f));

        CHECK(A->OverlapTest(B));

        B->SetWorldTransform(Transform(Vector2(0.5f, 0.f), 0.f));
        CHECK(A->OverlapTest(B) == false);

        Rectangle* BRectangle = B->CreateShape<Rectangle>();
        BRectangle->SetExtents(Vector2(1.5f, 1.f));
        CHECK(A->OverlapTest(B));
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

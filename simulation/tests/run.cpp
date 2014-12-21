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
    AnActor->SetPose(TM);

    Circle* ACircle = AnActor->CreateShape<Circle>();
    CHECK(ACircle);
    
    Rectangle* ARectangle = AnActor->CreateShape<Rectangle>();
    CHECK(ARectangle);

    const std::vector<BaseShape*>& Shapes = AnActor->GetShapes();
    CHECK(Shapes.size() == 2);
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

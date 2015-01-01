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

    const std::vector<BaseShape*>& Shapes = AnActor->GetShapes();
    CHECK(Shapes.size() == 2);
    {
        Actor* A = World1.CreateActor();
        Circle* ACircle = A->CreateShape<Circle>();
        ACircle->SetRadius(1.f);

        Actor* B = World1.CreateActor();
        Circle* BCircle = B->CreateShape<Circle>(Transform(Vector2(2.f, 0.f)));
        BCircle->SetRadius(1.f);

        const std::vector<BaseShape*>& BaseShapes = B->GetShapes();
        REQUIRE(BaseShapes[0]->LocalTM.Position.X == Approx(2.f));

        CHECK(A->OverlapTest(B));

        B->SetWorldTransform(Transform(Vector2(0.5f, 0.f), 0.f));
        CHECK(A->OverlapTest(B) == false);

        std::vector<ShapeOverlap> Overlaps;
        Rectangle* BRectangle = B->CreateShape<Rectangle>();
        BRectangle->SetExtents(Vector2(1.5f, 1.f));
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

TEST_CASE( "Manifold", "[manifold]" )
{
    World AWorld;
    Actor* Actor1 = AWorld.CreateActor();
    Actor* Actor2 = AWorld.CreateActor();

    Circle* Circ1 = Actor1->CreateShape<Circle>();
    Circ1->SetRadius(1.f);

    Circle* Circ2 = Actor2->CreateShape<Circle>();
    Circ2->SetRadius(1.f);

    Actor2->SetWorldTransform(Transform(Vector2(1.f, 0.f) ) );

    {
        AWorld.GenerateContactManifolds();
        const std::vector<ContactManifold>& ContactManifolds = AWorld.GetContactManifolds();
        REQUIRE(ContactManifolds.size() == 1);
        const ContactManifold& Manifold = ContactManifolds[0];

        CHECK(Manifold.A == Actor1);
        CHECK(Manifold.B == Actor2);
        REQUIRE(Manifold.NumContacts == 1);
        CHECK(Manifold.ContactPoints[0].PenetrationDepth == Approx(1.f));
        CHECK(Manifold.ContactPoints[0].Normal.X == Approx(1.f));
        CHECK(Manifold.ContactPoints[0].Normal.Y == Approx(0.f));
        CHECK(Manifold.ContactPoints[0].Position.X == Approx(0.f));
        CHECK(Manifold.ContactPoints[0].Position.Y == Approx(0.f));
    }

    Actor1->SetWorldTransform(Transform(Vector2(2, 0)));
    {
        AWorld.GenerateContactManifolds();
        const std::vector<ContactManifold>& ContactManifolds = AWorld.GetContactManifolds();
        REQUIRE(ContactManifolds.size() == 1);
        const ContactManifold& Manifold = ContactManifolds[0];

        REQUIRE(Manifold.NumContacts == 1);
        CHECK(Manifold.ContactPoints[0].PenetrationDepth == Approx(1.f));
        CHECK(Manifold.ContactPoints[0].Normal.X == Approx(-1.f));
        CHECK(Manifold.ContactPoints[0].Normal.Y == Approx(0.f));
        CHECK(Manifold.ContactPoints[0].Position.X == Approx(2.f));
        CHECK(Manifold.ContactPoints[0].Position.Y == Approx(0.f));
    }

    const float sq22 = sqrt(2.f) / 2.f;
    Actor1->SetWorldTransform(Transform::Identity);
    Actor2->SetWorldTransform(Transform(Vector2(sq22, sq22)));
    {
        AWorld.GenerateContactManifolds();
        const std::vector<ContactManifold>& ContactManifolds = AWorld.GetContactManifolds();
        REQUIRE(ContactManifolds.size() == 1);
        const ContactManifold& Manifold = ContactManifolds[0];

        REQUIRE(Manifold.NumContacts == 1);
        CHECK(Manifold.ContactPoints[0].PenetrationDepth == Approx(1.f));
        CHECK(Manifold.ContactPoints[0].Normal.X == Approx(sq22));
        CHECK(Manifold.ContactPoints[0].Normal.Y == Approx(sq22));
        CHECK(Manifold.ContactPoints[0].Position.X == Approx(0.f));
        CHECK(Manifold.ContactPoints[0].Position.Y == Approx(0.f));
    }

}

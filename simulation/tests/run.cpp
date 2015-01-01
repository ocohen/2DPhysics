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

TEST_CASE( "Circle-Circle Contact Points", "[circleCircleContactPoints]" )
{
    Circle Circ1(1.f);
    Circle Circ2(2.f);
    ShapeOverlap Overlap;
    Overlap.A = &Circ1;
    Overlap.B = &Circ2;
    {
        //Assume two circles are at position 0,0 and 2,0
        Overlap.MTD = Vector2(1,0);
        Overlap.PenetrationDepth = 2.f;

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Transform::Identity, Transform(Vector2(2,0)), Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        const Contact& AContact = Manifold.ContactPoints[0];
        CHECK(AContact.Normal.X == Approx(1.f));
        CHECK(AContact.Normal.Y == Approx(0.f));
        CHECK(AContact.Position.X == Approx(0.f));
        CHECK(AContact.Position.Y == Approx(0.f));
    }

    {
        //Assume two circles are at position 4,0 and 2,0
        Overlap.MTD = Vector2(-1,0);
        Overlap.PenetrationDepth = 2.f;

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Transform(Vector2(4,0)), Transform(Vector2(2,0)), Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        const Contact& AContact = Manifold.ContactPoints[0];
        CHECK(AContact.Normal.X == Approx(-1.f));
        CHECK(AContact.Normal.Y == Approx(0.f));
        CHECK(AContact.Position.X == Approx(4.f));
        CHECK(AContact.Position.Y == Approx(0.f));
    }

    {
        const float sq22 = sqrt(2.f) / 2.f;
        //Assume two circles are at position 0,0 and 2,2
        Overlap.MTD = Vector2(sq22, sq22);
        Overlap.PenetrationDepth = 2.f - sq22;

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Transform(Vector2(0,0)), Transform(Vector2(2,2)), Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        const Contact& AContact = Manifold.ContactPoints[0];
        CHECK(AContact.Normal.X == Approx(sq22));
        CHECK(AContact.Normal.Y == Approx(sq22));
        CHECK(AContact.Position.X == Approx(2 - 2.f*sq22));
        CHECK(AContact.Position.Y == Approx(2 - 2.f*sq22));
    }
}

TEST_CASE( "Rectangle-Rectangle Contact Points", "[RectangleRectangleContactPoints]" )
{
    return;
    Rectangle Rect1(Vector2(1,1));
    Rectangle Rect2(Vector2(2,1));
    ShapeOverlap Overlap;
    Overlap.A = &Rect1;
    Overlap.B = &Rect2;
    {
        //Assume two Rectangles are at position 0,0 and 2,0
        Overlap.MTD = Vector2(1,0);
        Overlap.PenetrationDepth = 2.f;

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Transform::Identity, Transform(Vector2(2,0)), Manifold);
        REQUIRE(Manifold.NumContacts == 2);
        {
            const Contact& AContact = Manifold.ContactPoints[0];
            CHECK(AContact.Normal.X == Approx(1.f));
            CHECK(AContact.Normal.Y == Approx(0.f));
            CHECK(AContact.Position.X == Approx(0.f));
            CHECK(AContact.Position.Y == Approx(1.f));
        }

        {
            const Contact& AContact = Manifold.ContactPoints[0];
            CHECK(AContact.Normal.X == Approx(1.f));
            CHECK(AContact.Normal.Y == Approx(0.f));
            CHECK(AContact.Position.X == Approx(0.f));
            CHECK(AContact.Position.Y == Approx(-1.f));
        }
    }
/*
    {
        //Assume two Rectangles are at position 4,0 and 2,0
        Overlap.MTD = Vector2(-1,0);
        Overlap.PenetrationDepth = 2.f;

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Transform(Vector2(4,0)), Transform(Vector2(2,0)), Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        const Contact& AContact = Manifold.ContactPoints[0];
        CHECK(AContact.Normal.X == Approx(-1.f));
        CHECK(AContact.Normal.Y == Approx(0.f));
        CHECK(AContact.Position.X == Approx(4.f));
        CHECK(AContact.Position.Y == Approx(0.f));
    }

    {
        const float sq22 = sqrt(2.f) / 2.f;
        //Assume two Rectangles are at position 0,0 and 2,2
        Overlap.MTD = Vector2(sq22, sq22);
        Overlap.PenetrationDepth = 2.f - sq22;

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Transform(Vector2(0,0)), Transform(Vector2(2,2)), Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        const Contact& AContact = Manifold.ContactPoints[0];
        CHECK(AContact.Normal.X == Approx(sq22));
        CHECK(AContact.Normal.Y == Approx(sq22));
        CHECK(AContact.Position.X == Approx(2 - 2.f*sq22));
        CHECK(AContact.Position.Y == Approx(2 - 2.f*sq22));
    }
    */
}

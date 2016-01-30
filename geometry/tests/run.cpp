#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "CircleShape.h"
#include "RectangleShape.h"
#include "shapeoverlap.h"
#include "convexpolygon.h"
#include "contactmanifold.h"

TEST_CASE( "Rendering Shapes", "[renderingshapes]")
{
    {
        const Vector2 Center(2,3);
        const CircleShape ACircleShape(1.5f);
        const BaseShape* Shape = &ACircleShape;

        std::vector<Vector2> RenderVertices;
        Shape->GenerateRenderVertices(RenderVertices, Center);

        REQUIRE(RenderVertices.size() == 32);
        for(size_t Count=0; Count < RenderVertices.size(); ++Count)
        {
            const float Distance = (RenderVertices[Count] - Center).Length();
            CHECK(Distance == Approx(1.5f));
        }
    }

    {
        const Transform TM(Vector2(1,2), PI*.5f);
        const RectangleShape ARectangleShape(Vector2(1.f, 2.f));
        const BaseShape* Shape = &ARectangleShape;

        std::vector<Vector2> RenderVertices;
        Shape->GenerateRenderVertices(RenderVertices, TM);

        REQUIRE(RenderVertices.size() == 4);    //RectangleShape ignores passed in number of vertices
        //CCW orientation
        CHECK((RenderVertices[0] - RenderVertices[1]).Length() == Approx(2.f));
        CHECK((RenderVertices[1] - RenderVertices[2]).Length() == Approx(4.f));
        CHECK((RenderVertices[2] - RenderVertices[3]).Length() == Approx(2.f));
        CHECK((RenderVertices[3] - RenderVertices[0]).Length() == Approx(4.f));
    }
}

TEST_CASE( "CircleShape Shape", "[CircleShape]" )
{
    CircleShape CircleShape1, CircleShape2;
    CircleShape1.SetRadius(1.f);

    CircleShape2.SetRadius(2.f);

    CHECK(BaseShape::OverlapTest(CircleShape1, Transform(Vector2(1,2)), CircleShape2, Transform(Vector2(4, 2))) == true);
    CHECK(BaseShape::OverlapTest(CircleShape1, Transform(Vector2(1,2)), CircleShape2, Transform(Vector2(4.1f, 2))) == false);
}

TEST_CASE("RectangleShape Shape", "[RectangleShape]")
{
    RectangleShape RectangleShape1, RectangleShape2;

    const Transform TM1;
    const Vector2 Extents1(1.f, 1.f);
    RectangleShape1.SetExtents(Extents1);

    const Transform TM2(Vector2(2.f, 2.f));
    const Vector2 Extents2(1.f, 2.f);
    RectangleShape2.SetExtents(Extents2);

    CHECK(BaseShape::OverlapTest(RectangleShape1, TM1, RectangleShape2, TM2) == true);
    RectangleShape2.SetExtents(Vector2(0.9f,1.5f));
    CHECK(BaseShape::OverlapTest(RectangleShape1, TM1, RectangleShape2, TM2) == false);

    const Transform TM2Rotated(TM2.Position, -PI/4.f);
    CHECK(BaseShape::OverlapTest(RectangleShape1, TM1, RectangleShape2, TM2Rotated) == true);

    const Transform TM2Rotated2(TM2.Position, PI/4.f);
    CHECK(BaseShape::OverlapTest(RectangleShape1, TM1, RectangleShape2, TM2Rotated2) == false);
}

TEST_CASE("Base Shape", "[baseshape]")
{
    RectangleShape RectangleShape1;
    CHECK(RectangleShape1.GetType() == Shape::Rectangle);

    {
        const Transform TM1(Vector2::Zero, 0.f);
        const Vector2 Extents1(1.f, 1.f);
        RectangleShape1.SetExtents(Extents1);

        CircleShape CircleShape1;
        CircleShape1.SetRadius(1.f);

        CHECK(BaseShape::OverlapTest(RectangleShape1, TM1, CircleShape1, Transform(Vector2(2,1))) == true);

        CircleShape1.SetRadius(0.9f);
        CHECK(BaseShape::OverlapTest(RectangleShape1, TM1, CircleShape1, Transform(Vector2(2,1))) == false);
    }

    {
        const Transform TM2(Vector2(3,2), 0);
        const Vector2 Extents(2.f, 1.f);
        RectangleShape1.SetExtents(Extents);

        CircleShape CircleShape2;
        CircleShape2.SetRadius(sqrt(2.f)+0.00001f);

        CHECK(BaseShape::OverlapTest(RectangleShape1, TM2, CircleShape2, Transform::Identity) == true);
        CHECK(BaseShape::OverlapTest(RectangleShape1, Transform(Vector2(3,2), PI*0.5f), CircleShape2, Transform::Identity) == false);
        CHECK(BaseShape::OverlapTest(RectangleShape1, Transform(Vector2(3,2), PI), CircleShape2, Transform::Identity) == true);
    }
}

TEST_CASE("Minimum Displacement Vector", "[mdv]")
{
    ShapeOverlap Overlap;
    {
        CircleShape CircleShape1(1.f);
        CircleShape CircleShape2(2.f);
        {
            REQUIRE(BaseShape::OverlapTest(CircleShape1, Transform::Identity, CircleShape2, Transform(Vector2(3.f, 0.f)), &Overlap));
            CHECK(Overlap.A == &CircleShape1);
            CHECK(Overlap.B == &CircleShape2);
            CHECK(Overlap.MTD.X == Approx(1.f));
            CHECK(Overlap.MTD.Y == Approx(0.f));
            CHECK(Overlap.PenetrationDepth == Approx(0.f));
        }

        {
            REQUIRE(BaseShape::OverlapTest(CircleShape1, Transform::Identity, CircleShape2, Transform(Vector2(3*sqrt(2.f)/2.f, 3*sqrt(2.f)/2.f)), &Overlap));
            CHECK(Overlap.A == &CircleShape1);
            CHECK(Overlap.B == &CircleShape2);
            CHECK(Overlap.MTD.X == Approx(sqrt(2.f) / 2.f));
            CHECK(Overlap.MTD.Y == Approx(sqrt(2.f) / 2.f));
            CHECK(Overlap.PenetrationDepth == Approx(0.f));
        }

        {
            REQUIRE(BaseShape::OverlapTest(CircleShape1, Transform::Identity, CircleShape2, Transform(Vector2(1.f, 0.f)), &Overlap));
            CHECK(Overlap.A == &CircleShape1);
            CHECK(Overlap.B == &CircleShape2);
            CHECK(Overlap.PenetrationDepth == Approx(2.f));

            REQUIRE(BaseShape::OverlapTest(CircleShape1, Transform::Identity, CircleShape2, Transform(Vector2(1.5f, 0.f)), &Overlap));
            CHECK(Overlap.A == &CircleShape1);
            CHECK(Overlap.B == &CircleShape2);
            CHECK(Overlap.PenetrationDepth == Approx(1.5f));

            REQUIRE(BaseShape::OverlapTest(CircleShape1, Transform::Identity, CircleShape2, Transform(Vector2(0.5f, 0.f)), &Overlap));
            CHECK(Overlap.A == &CircleShape1);
            CHECK(Overlap.B == &CircleShape2);
            CHECK(Overlap.PenetrationDepth == Approx(2.5f));
        }
    }

    {
        RectangleShape Rect1(Vector2(1.f, 1.f));
        RectangleShape Rect2(Vector2(2.f, 1.f));

        REQUIRE(BaseShape::OverlapTest(Rect1, Transform::Identity, Rect2, Transform(Vector2(3.f, 0.f)), &Overlap));
        CHECK(Overlap.A == &Rect1);
        CHECK(Overlap.B == &Rect2);
        CHECK(Overlap.MTD.X == Approx(1.f));
        CHECK(Overlap.MTD.Y == Approx(0.f));
        CHECK(Overlap.PenetrationDepth == Approx(0.f));

        REQUIRE(BaseShape::OverlapTest(Rect1, Transform::Identity, Rect2, Transform(Vector2(2.f, 0.5f)), &Overlap));
        CHECK(Overlap.A == &Rect1);
        CHECK(Overlap.B == &Rect2);
        CHECK(Overlap.MTD.X == Approx(1.f));
        CHECK(Overlap.MTD.Y == Approx(0.f));
        CHECK(Overlap.PenetrationDepth == Approx(1.f));

        REQUIRE(BaseShape::OverlapTest(Rect1, Transform::Identity, Rect2, Transform(Vector2(0.f, 0.1f)), &Overlap));
        CHECK(Overlap.A == &Rect1);
        CHECK(Overlap.B == &Rect2);
        CHECK(Overlap.MTD.X == Approx(0.f));
        CHECK(Overlap.MTD.Y == Approx(1.f));
        CHECK(Overlap.PenetrationDepth == Approx(1.9f));
    }

    {
        RectangleShape Rect(Vector2(1.f, 1.f));
        CircleShape Circ(1.f);

        REQUIRE(BaseShape::OverlapTest(Rect, Transform::Identity, Circ, Transform(Vector2(0.f, 2.f)), &Overlap));
        CHECK(Overlap.A == &Rect);
        CHECK(Overlap.B == &Circ);
        CHECK(Overlap.MTD.X == Approx(0.f));
        CHECK(Overlap.MTD.Y == Approx(1.f));
        CHECK(Overlap.PenetrationDepth == Approx(0.f));

        REQUIRE(BaseShape::OverlapTest(Circ, Transform(Vector2(0, 2.f)), Rect, Transform::Identity, &Overlap));
        CHECK(Overlap.A == &Circ);
        CHECK(Overlap.B == &Rect);
        CHECK(Overlap.MTD.X == Approx(0.f));
        CHECK(Overlap.MTD.Y == Approx(-1.f));
        CHECK(Overlap.PenetrationDepth == Approx(0.f));

        REQUIRE(BaseShape::OverlapTest(Rect, Transform::Identity, Circ, Transform(Vector2(0.f, 1.5f)), &Overlap));
        CHECK(Overlap.A == &Rect);
        CHECK(Overlap.B == &Circ);
        CHECK(Overlap.MTD.X == Approx(0.f));
        CHECK(Overlap.MTD.Y == Approx(1.f));
        CHECK(Overlap.PenetrationDepth == Approx(.5f));
        
        REQUIRE(BaseShape::OverlapTest(Rect, Transform::Identity, Circ, Transform(Vector2(1.5f, 1.5f)), &Overlap));
        CHECK(Overlap.A == &Rect);
        CHECK(Overlap.B == &Circ);
        CHECK(Overlap.MTD.X == Approx(sqrt(2)/2.f));
        CHECK(Overlap.MTD.Y == Approx(sqrt(2)/2.f));
        CHECK(Overlap.PenetrationDepth == Approx(1.f - sqrt(2)/2.f));

        REQUIRE(BaseShape::OverlapTest(Rect, Transform::Identity, Circ, Transform(Vector2(0.9f, 0.f)), &Overlap));
        CHECK(Overlap.A == &Rect);
        CHECK(Overlap.B == &Circ);
        CHECK(Overlap.MTD.X == Approx(1.f));
        CHECK(Overlap.MTD.Y == Approx(0.f));
        CHECK(Overlap.PenetrationDepth == Approx(1.1f));

        REQUIRE(BaseShape::OverlapTest(Rect, Transform::Identity, Circ, Transform(Vector2(0.f, 0.1f)), &Overlap));
        CHECK(Overlap.A == &Rect);
        CHECK(Overlap.B == &Circ);
        CHECK(Overlap.MTD.X == Approx(0.f));
        CHECK(Overlap.MTD.Y == Approx(1.f));
        CHECK(Overlap.PenetrationDepth == Approx(1.9f));
    }
}

TEST_CASE( "Convex Shapes", "[convexshapes]")
{
    //generic poly (triangle)
    {
        ConvexPolygon Poly;
        Poly.AddVertex(Vector2::Zero);
        Poly.AddVertex(Vector2(1,0));
        Poly.AddVertex(Vector2(1,1));

        const std::vector<Vector2>& Vertices = Poly.GetVertices();
        REQUIRE(Vertices.size() == 3);

        std::vector<Vector2> Normals;
        Poly.ComputeNormals(Normals);
        REQUIRE(Normals.size() == 3);
        CHECK(Normals[0].X == Approx(0.f));
        CHECK(Normals[0].Y == Approx(-1.f));
        
        CHECK(Normals[1].X == Approx(1.f));
        CHECK(Normals[1].Y == Approx(0.f));

        CHECK(Normals[2].X == Approx(-sqrt(2)/2.f));
        CHECK(Normals[2].Y == Approx(sqrt(2)/2.f));
    }

    //RectangleShape child class
    {
        RectangleShape Rect(Vector2(2.f, 1.f));
        const std::vector<Vector2>& Vertices = Rect.GetVertices();
        REQUIRE(Vertices.size() == 4);
        CHECK(Vertices[0].X == Approx(2.f));
        CHECK(Vertices[0].Y == Approx(-1.f));

        CHECK(Vertices[1].X == Approx(2.f));
        CHECK(Vertices[1].Y == Approx(1.f));

        CHECK(Vertices[2].X == Approx(-2.f));
        CHECK(Vertices[2].Y == Approx(1.f));
        
        CHECK(Vertices[3].X == Approx(-2.f));
        CHECK(Vertices[3].Y == Approx(-1.f));

        std::vector<Vector2> Normals;
        Rect.ComputeNormals(Normals);
        REQUIRE(Normals.size() == 4);
        CHECK(Normals[0].X == Approx(1.f));
        CHECK(Normals[0].Y == Approx(0.f));

        CHECK(Normals[1].X == Approx(0.f));
        CHECK(Normals[1].Y == Approx(1.f));

        CHECK(Normals[2].X == Approx(-1.f));
        CHECK(Normals[2].Y == Approx(0.f));

        CHECK(Normals[3].X == Approx(0.f));
        CHECK(Normals[3].Y == Approx(-1.f));
    }
}

TEST_CASE( "CircleShape-CircleShape Contact Points", "[CircleShapeCircleShapeContactPoints]" )
{
    CircleShape Circ1(1.f);
    CircleShape Circ2(2.f);
    ShapeOverlap Overlap;
    Overlap.A = &Circ1;
    Overlap.B = &Circ2;
    {
        //Assume two CircleShapes are at position 0,0 and 2,0
        Overlap.MTD = Vector2(1,0);
        Overlap.PenetrationDepth = 2.f;
        Overlap.AWorldTM = Transform::Identity;
        Overlap.BWorldTM = Transform(Vector2(2,0));

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        const Contact& AContact = Manifold.ContactPoints[0];
        CHECK(AContact.Normal.X == Approx(1.f));
        CHECK(AContact.Normal.Y == Approx(0.f));
        CHECK(AContact.Position.X == Approx(0.f));
        CHECK(AContact.Position.Y == Approx(0.f));
    }

    {
        //Assume two CircleShapes are at position 4,0 and 2,0
        Overlap.MTD = Vector2(-1,0);
        Overlap.PenetrationDepth = 2.f;
        Overlap.AWorldTM = Transform(Vector2(4,0));
        Overlap.BWorldTM = Transform(Vector2(2,0));

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        const Contact& AContact = Manifold.ContactPoints[0];
        CHECK(AContact.Normal.X == Approx(-1.f));
        CHECK(AContact.Normal.Y == Approx(0.f));
        CHECK(AContact.Position.X == Approx(4.f));
        CHECK(AContact.Position.Y == Approx(0.f));
    }

    {
        const float sq22 = sqrt(2.f) / 2.f;
        //Assume two CircleShapes are at position 0,0 and 2,2
        Overlap.MTD = Vector2(sq22, sq22);
        Overlap.PenetrationDepth = 2.f - sq22;
        Overlap.AWorldTM = Transform(Vector2(0,0));
        Overlap.BWorldTM = Transform(Vector2(2,2));

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        const Contact& AContact = Manifold.ContactPoints[0];
        CHECK(AContact.Normal.X == Approx(sq22));
        CHECK(AContact.Normal.Y == Approx(sq22));
        CHECK(AContact.Position.X == Approx(2 - 2.f*sq22));
        CHECK(AContact.Position.Y == Approx(2 - 2.f*sq22));
    }
}

TEST_CASE( "RectangleShape-RectangleShape Contact Points", "[RectangleShapeRectangleShapeContactPoints]" )
{
    RectangleShape Rect1(Vector2(1,1));
    RectangleShape Rect2(Vector2(2,1));
    ShapeOverlap Overlap;
    Overlap.A = &Rect1;
    Overlap.B = &Rect2;
    {
        //Assume two RectangleShapes are at position 0,0 and 2,0
        Overlap.MTD = Vector2(1,0);
        Overlap.PenetrationDepth = 2.f;
        Overlap.AWorldTM = Transform(Vector2(0,0));
        Overlap.BWorldTM = Transform(Vector2(2,0));

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Manifold);
        REQUIRE(Manifold.NumContacts == 2);
        {
            const Contact& AContact = Manifold.ContactPoints[0];
            CHECK(AContact.Normal.X == Approx(1.f));
            CHECK(AContact.Normal.Y == Approx(0.f));
            CHECK(AContact.Position.X == Approx(0.f));
            CHECK(AContact.Position.Y == Approx(1.f));
        }

        {
            const Contact& AContact = Manifold.ContactPoints[1];
            CHECK(AContact.Normal.X == Approx(1.f));
            CHECK(AContact.Normal.Y == Approx(0.f));
            CHECK(AContact.Position.X == Approx(0.f));
            CHECK(AContact.Position.Y == Approx(-1.f));
        }
    }

    {
        //Assume two RectangleShapes are at position 0,0 and 2,0.1
        Overlap.MTD = Vector2(1,0);
        Overlap.PenetrationDepth = 2.f;
        Overlap.AWorldTM = Transform(Vector2(0,0));
        Overlap.BWorldTM = Transform(Vector2(2,0.1f));

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Manifold);
        REQUIRE(Manifold.NumContacts == 2);
        {
            const Contact& AContact = Manifold.ContactPoints[0];
            CHECK(AContact.Normal.X == Approx(1.f));
            CHECK(AContact.Normal.Y == Approx(0.f));
            CHECK(AContact.Position.X == Approx(0.f));
            CHECK(AContact.Position.Y == Approx(1.f));
        }

        {
            const Contact& AContact = Manifold.ContactPoints[1];
            CHECK(AContact.Normal.X == Approx(1.f));
            CHECK(AContact.Normal.Y == Approx(0.f));
            CHECK(AContact.Position.X == Approx(0.f));
            CHECK(AContact.Position.Y == Approx(-0.9f));
        }
    }

    {
        //Assume two RectangleShapes are at position 0,0 and 0,1+sqrt(2)
        Rect2.SetExtents(Vector2(1,1));
        Overlap.MTD = Vector2(0,1);
        Overlap.PenetrationDepth = 0.001f;
        Overlap.AWorldTM = Transform(Vector2(0,0));
        Overlap.BWorldTM = Transform(Vector2(0,1 + sqrt(2) - 0.001), PI / 4.f);

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Manifold);
        REQUIRE(Manifold.NumContacts == 1);
    }
}

TEST_CASE( "RectangleShape-CircleShape Contact Points", "[RectangleShapeCircleShapeContactPoints]" )
{
    RectangleShape Rect1(Vector2(1,1));
    CircleShape Circ1(1.f);
    ShapeOverlap Overlap;
    Overlap.A = &Rect1;
    Overlap.B = &Circ1;
    {
        //Assume CircleShape is at 0,0 and RectangleShape is at 1,0
        Overlap.MTD = Vector2(-1,0);
        Overlap.PenetrationDepth = 1.f;
        Overlap.AWorldTM = Transform(Vector2(1,0));
        Overlap.BWorldTM = Transform(Vector2(0,0));

        ContactManifold Manifold;
        BaseShape::GenerateManifold(Overlap, Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        {
            const Contact& AContact = Manifold.ContactPoints[0];
            CHECK(AContact.Normal.X == Approx(-1.f));
            CHECK(AContact.Normal.Y == Approx(0.f));
            CHECK(AContact.Position.X == Approx(1.f));
            CHECK(AContact.Position.Y == Approx(0.f));
        }

        //CircleShape is at 1,1 and RectangleShape is at 0,0
        Overlap.MTD = Vector2(0,1);
        Overlap.AWorldTM = Transform(Vector2(0,0));
        Overlap.BWorldTM = Transform(Vector2(1,1));
        BaseShape::GenerateManifold(Overlap, Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        {
            const Contact& AContact = Manifold.ContactPoints[0];
            CHECK(AContact.Normal.X == Approx(0.f));
            CHECK(AContact.Normal.Y == Approx(1.f));
            CHECK(AContact.Position.X == Approx(1.f));
            CHECK(AContact.Position.Y == Approx(0.f));
        }

        //CircleShape is at -1,-1.5 and RectangleShape is at 0,0
        Overlap.MTD = Vector2(0,-1);
        Overlap.PenetrationDepth = 0.5f;
        Overlap.AWorldTM = Transform(Vector2(0,0));
        Overlap.BWorldTM = Transform(Vector2(-1,-1.5));
        BaseShape::GenerateManifold(Overlap, Manifold);
        REQUIRE(Manifold.NumContacts == 1);
        {
            const Contact& AContact = Manifold.ContactPoints[0];
            CHECK(AContact.Normal.X == Approx(0.f));
            CHECK(AContact.Normal.Y == Approx(-1.f));
            CHECK(AContact.Position.X == Approx(-1.f));
            CHECK(AContact.Position.Y == Approx(-0.5f));
        }
    }
}


TEST_CASE("ShapePhysics", "[shapePhysics]" )
{
}

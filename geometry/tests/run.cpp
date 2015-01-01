#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "circle.h"
#include "rectangle.h"
#include "shapeoverlap.h"
#include "convexpolygon.h"

TEST_CASE( "Rendering Shapes", "[renderingshapes]")
{
    {
        const Vector2 Center(2,3);
        const Circle ACircle(1.5f);
        const BaseShape* Shape = &ACircle;

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
        const Rectangle ARectangle(Vector2(1.f, 2.f));
        const BaseShape* Shape = &ARectangle;

        std::vector<Vector2> RenderVertices;
        Shape->GenerateRenderVertices(RenderVertices, TM);

        REQUIRE(RenderVertices.size() == 4);    //rectangle ignores passed in number of vertices
        //CCW orientation
        CHECK((RenderVertices[0] - RenderVertices[1]).Length() == Approx(2.f));
        CHECK((RenderVertices[1] - RenderVertices[2]).Length() == Approx(4.f));
        CHECK((RenderVertices[2] - RenderVertices[3]).Length() == Approx(2.f));
        CHECK((RenderVertices[3] - RenderVertices[0]).Length() == Approx(4.f));
    }
}

TEST_CASE( "Circle Shape", "[circle]" )
{
    Circle Circle1, Circle2;
    Circle1.SetRadius(1.f);

    Circle2.SetRadius(2.f);

    CHECK(BaseShape::OverlapTest(Circle1, Transform(Vector2(1,2)), Circle2, Transform(Vector2(4, 2))) == true);
    CHECK(BaseShape::OverlapTest(Circle1, Transform(Vector2(1,2)), Circle2, Transform(Vector2(4.1, 2))) == false);
}

TEST_CASE("Rectangle Shape", "[rectangle]")
{
    Rectangle Rectangle1, Rectangle2;

    const Transform TM1;
    const Vector2 Extents1(1.f, 1.f);
    Rectangle1.SetExtents(Extents1);

    const Transform TM2(Vector2(2.f, 2.f));
    const Vector2 Extents2(1.f, 2.f);
    Rectangle2.SetExtents(Extents2);

    CHECK(BaseShape::OverlapTest(Rectangle1, TM1, Rectangle2, TM2) == true);
    Rectangle2.SetExtents(Vector2(0.9f,1.5f));
    CHECK(BaseShape::OverlapTest(Rectangle1, TM1, Rectangle2, TM2) == false);

    const Transform TM2Rotated(TM2.Position, -PI/4.f);
    CHECK(BaseShape::OverlapTest(Rectangle1, TM1, Rectangle2, TM2Rotated) == true);

    const Transform TM2Rotated2(TM2.Position, PI/4.f);
    CHECK(BaseShape::OverlapTest(Rectangle1, TM1, Rectangle2, TM2Rotated2) == false);
}

TEST_CASE("Base Shape", "[baseshape]")
{
    Rectangle Rectangle1;
    CHECK(Rectangle1.GetType() == Shape::Rectangle);

    {
        const Transform TM1(Vector2::Zero, 0.f);
        const Vector2 Extents1(1.f, 1.f);
        Rectangle1.SetExtents(Extents1);

        Circle Circle1;
        Circle1.SetRadius(1.f);

        CHECK(BaseShape::OverlapTest(Rectangle1, TM1, Circle1, Transform(Vector2(2,1))) == true);

        Circle1.SetRadius(0.9f);
        CHECK(BaseShape::OverlapTest(Rectangle1, TM1, Circle1, Transform(Vector2(2,1))) == false);
    }

    {
        const Transform TM2(Vector2(3,2), 0);
        const Vector2 Extents(2.f, 1.f);
        Rectangle1.SetExtents(Extents);

        Circle Circle2;
        Circle2.SetRadius(sqrt(2.f)+0.00001f);

        CHECK(BaseShape::OverlapTest(Rectangle1, TM2, Circle2, Transform::Identity) == true);
        CHECK(BaseShape::OverlapTest(Rectangle1, Transform(Vector2(3,2), PI*0.5f), Circle2, Transform::Identity) == false);
        CHECK(BaseShape::OverlapTest(Rectangle1, Transform(Vector2(3,2), PI), Circle2, Transform::Identity) == true);
    }
}

TEST_CASE("Minimum Displacement Vector", "[mdv]")
{
    ShapeOverlap Overlap;
    {
        Circle Circle1(1.f);
        Circle Circle2(2.f);
        {
            REQUIRE(BaseShape::OverlapTest(Circle1, Transform::Identity, Circle2, Transform(Vector2(3.f, 0.f)), &Overlap));
            CHECK(Overlap.A == &Circle1);
            CHECK(Overlap.B == &Circle2);
            CHECK(Overlap.MTD.X == Approx(1.f));
            CHECK(Overlap.MTD.Y == Approx(0.f));
            CHECK(Overlap.PenetrationDepth == Approx(0.f));
        }

        {
            REQUIRE(BaseShape::OverlapTest(Circle1, Transform::Identity, Circle2, Transform(Vector2(3*sqrt(2.f)/2.f, 3*sqrt(2.f)/2.f)), &Overlap));
            CHECK(Overlap.A == &Circle1);
            CHECK(Overlap.B == &Circle2);
            CHECK(Overlap.MTD.X == Approx(sqrt(2.f) / 2.f));
            CHECK(Overlap.MTD.Y == Approx(sqrt(2.f) / 2.f));
            CHECK(Overlap.PenetrationDepth == Approx(0.f));
        }

        {
            REQUIRE(BaseShape::OverlapTest(Circle1, Transform::Identity, Circle2, Transform(Vector2(1.f, 0.f)), &Overlap));
            CHECK(Overlap.A == &Circle1);
            CHECK(Overlap.B == &Circle2);
            CHECK(Overlap.PenetrationDepth == Approx(2.f));

            REQUIRE(BaseShape::OverlapTest(Circle1, Transform::Identity, Circle2, Transform(Vector2(1.5f, 0.f)), &Overlap));
            CHECK(Overlap.A == &Circle1);
            CHECK(Overlap.B == &Circle2);
            CHECK(Overlap.PenetrationDepth == Approx(1.5f));

            REQUIRE(BaseShape::OverlapTest(Circle1, Transform::Identity, Circle2, Transform(Vector2(0.5f, 0.f)), &Overlap));
            CHECK(Overlap.A == &Circle1);
            CHECK(Overlap.B == &Circle2);
            CHECK(Overlap.PenetrationDepth == Approx(2.5f));
        }
    }

    {
        Rectangle Rect1(Vector2(1.f, 1.f));
        Rectangle Rect2(Vector2(2.f, 1.f));

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
        Rectangle Rect(Vector2(1.f, 1.f));
        Circle Circ(1.f);

        REQUIRE(BaseShape::OverlapTest(Rect, Transform::Identity, Circ, Transform(Vector2(0.f, 2.f)), &Overlap));
        CHECK(Overlap.A == &Rect);
        CHECK(Overlap.B == &Circ);
        CHECK(Overlap.MTD.X == Approx(0.f));
        CHECK(Overlap.MTD.Y == Approx(1.f));
        CHECK(Overlap.PenetrationDepth == Approx(0.f));

        REQUIRE(BaseShape::OverlapTest(Circ, Transform(Vector2(0, 2.f)), Rect, Transform::Identity, &Overlap));
        CHECK(Overlap.A == &Rect);
        CHECK(Overlap.B == &Circ);
        CHECK(Overlap.MTD.X == Approx(0.f));
        CHECK(Overlap.MTD.Y == Approx(1.f));
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

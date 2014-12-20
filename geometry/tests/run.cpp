#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "circle.h"
#include "rectangle.h"

TEST_CASE( "Circle Shape", "[circle]" )
{
    Circle Circle1, Circle2;
    Circle1.SetRadius(1.f);
    Circle1.SetPosition(Vector2(1,2));

    Circle2.SetRadius(2.f);
    Circle2.SetPosition(Vector2(4,2));

    CHECK(Circle1.OverlapTest(Circle2) == true);

    Circle2.SetPosition( Vector2(4.1,2));
    CHECK(Circle1.OverlapTest(Circle2) == false);
}

TEST_CASE("Rectangle Shape", "[rectangle]")
{
    Rectangle Rectangle1, Rectangle2;

    const Transform TM1(Vector2::Zero, 0.f);
    const Vector2 Extents1(1.f, 1.f);
    Rectangle1.SetTransform(TM1);
    Rectangle1.SetExtents(Extents1);

    const Transform TM2(Vector2(2.f, 2.f), 0.f);
    const Vector2 Extents2(1.f, 2.f);
    Rectangle2.SetTransform(TM2);
    Rectangle2.SetExtents(Extents2);

    CHECK(Rectangle1.OverlapTest(Rectangle2) == true);
    Rectangle2.SetExtents(Vector2(0.9f,1.5f));
    CHECK(Rectangle1.OverlapTest(Rectangle2) == false);

    const Transform TM2Rotated(TM2.Position, -PI/4.f);
    Rectangle2.SetTransform(TM2Rotated);
    CHECK(Rectangle1.OverlapTest(Rectangle2) == true);

    const Transform TM2Rotated2(TM2.Position, PI/4.f);
    Rectangle2.SetTransform(TM2Rotated2);
    CHECK(Rectangle1.OverlapTest(Rectangle2) == false);
}

TEST_CASE("Base Shape", "[baseshape]")
{
    Rectangle Rectangle1;
    CHECK(Rectangle1.GetType() == Shape::Rectangle);

    {
        const Transform TM1(Vector2::Zero, 0.f);
        const Vector2 Extents1(1.f, 1.f);
        Rectangle1.SetTransform(TM1);
        Rectangle1.SetExtents(Extents1);

        Circle Circle1;
        Circle1.SetPosition(Vector2(2,1));
        Circle1.SetRadius(1.f);

        CHECK(Rectangle1.OverlapTest(Circle1) == true);

        Circle1.SetRadius(0.9f);
        CHECK(Rectangle1.OverlapTest(Circle1) == false);
    }

    {
        const Transform TM2(Vector2(3,2), 0);
        const Vector2 Extents(2.f, 1.f);
        Rectangle1.SetTransform(TM2);
        Rectangle1.SetExtents(Extents);

        Circle Circle2;
        Circle2.SetPosition(Vector2(0,0));
        Circle2.SetRadius(sqrt(2.f)+0.00001f);

        CHECK(Rectangle1.OverlapTest(Circle2) == true);

        Rectangle1.SetTransform(Transform(Vector2(3,2), PI*0.5f));
        CHECK(Rectangle1.OverlapTest(Circle2) == false);

        Rectangle1.SetTransform(Transform(Vector2(3,2), PI));
        CHECK(Rectangle1.OverlapTest(Circle2) == true);
    }


}

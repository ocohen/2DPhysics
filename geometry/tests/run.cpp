#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "circle.h"

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

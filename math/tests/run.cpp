#include "gmath.h"
#include "vector2.h"
#include "transform.h"

#define CATCH_CONFIG_MAIN
#include "catch.h"

TEST_CASE( "General math", "[gmath]" )
{
    CHECK( Abs(-1) == 1 );
    CHECK( Abs(0) == 0);
    CHECK( Abs(2) == 2);
    CHECK( IsZero(0) );
    CHECK( !IsZero(0.001) );
    CHECK( IsZero(0.0001) );
    CHECK( IsNear(1, 1.00001) );
    CHECK( !IsNear(1, 1.01) );
}


TEST_CASE( "Vector math", "[vector2]" )
{
    CHECK( Vector2::Dot(Vector2(1,2), Vector2(3,-4)) == -5);
    CHECK( Vector2::Zero.IsZero() );
    CHECK( IsNear(Vector2(2,3).Length(), 3.60555f) );
    CHECK( Vector2::Zero.GetSafeNormal() == Vector2::Zero);
    
    const Vector2 Addition = Vector2(1,2) + Vector2(-3,4);
    CHECK( Addition.X == -2);
    CHECK( Addition.Y == 6);
    
    const Vector2 Subtraction = Vector2(1,2) - Vector2(-3,4);
    REQUIRE( Subtraction.X == 4);
    REQUIRE( Subtraction.Y == -2);

    const Vector2 Normal = Vector2(1,2).GetNormal();
    const Vector2 Difference = Normal - Vector2(0.4472135955f, 0.894427191f);
    CHECK(Difference.IsZero()); 
}

TEST_CASE("transform math", "[transform]" )
{
    const Transform A( Vector2(2,3), 3.14159f);
    const Vector2 NewPosition = A.TransformPosition( Vector2(0,0) );
    CHECK(NewPosition == Vector2(2,3));

    const Vector2 NewPosition2 = A.TransformVector( Vector2(1,0) );
    CHECK( NewPosition2.X == Approx(-1) );
    CHECK( NewPosition2.Y == Approx(0) );

    const Vector2 CombinedPosition = A.TransformPosition( Vector2(1,0) );
    CHECK( CombinedPosition.X == Approx(1) );
    CHECK( CombinedPosition.Y == Approx(3) );

    const Transform B(Vector2::Zero, -PI / 4.f);
    const Vector2 AnotherRotation = B.TransformVector(Vector2(1,1));
    CHECK( AnotherRotation.X == Approx(Vector2(1,1).Length()));
    CHECK( AnotherRotation.Y == Approx(0.f));
}
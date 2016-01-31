#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "world.h"
#include "actor.h"
#include "circleshape.h"
#include "rectangleshape.h"
#include "solver.h"

TEST_CASE( "Actor", "[actor]" )
{
    World World1;

    {
        Actor* AnActor = World1.CreateActor();
        AnActor->SetKinematic(true);
        REQUIRE(AnActor->IsKinematic());

        const Transform TM(Vector2(1,2), PI*0.25f);
        AnActor->SetWorldTransform(TM);

        SimShape* ACircleShape = AnActor->CreateShape<CircleShape>();
        CHECK(ACircleShape);

        SimShape* ARectangleShape = AnActor->CreateShape<RectangleShape>();
        CHECK(ARectangleShape);

        const std::vector<SimShape*>& Shapes = AnActor->GetShapes();
        CHECK(Shapes.size() == 2);
    }

    {
        CircleShape DefCircleShape(1.f);

        Actor* A = World1.CreateActor();
        SimShape* ACircleShape = A->CreateShape<CircleShape>(DefCircleShape);

        Actor* B = World1.CreateActor();
        SimShape* BCircleShape = B->CreateShape<CircleShape>(DefCircleShape, Transform(Vector2(2.f, 0.f)));

        const std::vector<SimShape*>& Shapes = B->GetShapes();
        REQUIRE(Shapes[0]->LocalTM.Position.X == Approx(2.f));

        CHECK(A->OverlapTest(B));

        B->SetWorldTransform(Transform(Vector2(0.5f, 0.f), 0.f));
        CHECK(A->OverlapTest(B) == false);

        RectangleShape DefRectangleShape(Vector2(1.5f, 1.f));
        std::vector<ShapeOverlap> Overlaps;
        SimShape* BRectangleShape = B->CreateShape<RectangleShape>(DefRectangleShape);
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

TEST_CASE( "Impulses", "[impulses]" )
{
    {
        //check linear velocity at
        World AWorld;
        Actor* Obj = AWorld.CreateActor();
        RectangleShape Rect(Vector2(2.f, 1.f));
        Obj->CreateShape<RectangleShape>(Rect);
        Obj->SetAngularVelocity(1.f);

        {
            const Vector2 VelAt0 = Obj->GetLinearVelocityAt(Vector2::Zero);
            CHECK(VelAt0.X == Approx(0.f));
            CHECK(VelAt0.Y == Approx(0.f));

            const Vector2 VelAtLeft = Obj->GetLinearVelocityAt(Vector2(-2.f, 0.f));
            CHECK(VelAtLeft.X == Approx(0.f));
            CHECK(VelAtLeft.Y == Approx(-2.f));
        }

        Obj->SetLinearVelocity(Vector2(1.f, 1.f));
        {

            const Vector2 VelAt0 = Obj->GetLinearVelocityAt(Vector2::Zero);
            CHECK(VelAt0.X == Approx(1.f));
            CHECK(VelAt0.Y == Approx(1.f));

            const Vector2 VelAtLeft = Obj->GetLinearVelocityAt(Vector2(-2.f, 0.f));
            CHECK(VelAtLeft.X == Approx(1.f));
            CHECK(VelAtLeft.Y == Approx(-1.f));
        }
    }

    {
        World AWorld;
        Actor* Obj = AWorld.CreateActor();
        RectangleShape DefRectangleShape(Vector2(2.f, 1.f));
        SimShape* Rect = Obj->CreateShape<RectangleShape>(DefRectangleShape);
        Rect->SetMass(2.f);
        Rect->SetMomentOfInertia(3.f);
        Obj->CalculateMassInertiaAndCOM();

        Obj->AddImpulse(Vector2(1.f, 1.f));

        CHECK(Obj->GetLinearVelocity().X == Approx(0.5f));
        CHECK(Obj->GetLinearVelocity().Y == Approx(0.5f));

        Obj->AddImpulseAt(Vector2(0.f, 1.f), Vector2(2.f, 0.f));

        CHECK(Obj->GetLinearVelocity().X == Approx(0.5f));
        CHECK(Obj->GetLinearVelocity().Y == Approx(1.f));

        CHECK(Obj->GetAngularVelocity() == Approx(2/3.f));
    }
}

TEST_CASE( "Integration", "[integration]" )
{

    const float Dt = 1.f / 60.f;
    {
        World AWorld;
        Actor* Ball = AWorld.CreateActor();
        Ball->CreateShape<CircleShape>();
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
        Ball->CreateShape<CircleShape>();
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
        Ball->CreateShape<CircleShape>();
        //Constant acceleration with no initial velocity
        for(int Step = 0; Step < 60; ++Step)
        {
            Ball->SetLinearAcceleration(Vector2(1.f, 2.f));
            Ball->SetAngularAcceleration(1.f);
            AWorld.Integrate(Dt);
        }

        const Transform TM = Ball->GetWorldTransform();
        CHECK(TM.Position.X == Approx(0.5f));
        CHECK(TM.Position.Y == Approx(1.f));
        CHECK(TM.Rotation == Approx(0.5f));
    }

    {
        //Make sure rotation happens about COM
        World AWorld;
        Actor* Obj = AWorld.CreateActor();
        Obj->CreateShape<CircleShape>();
        Obj->CreateShape<CircleShape>(Transform(Vector2(2.f, 0.f)));
        Obj->CalculateCOM();
        REQUIRE(Obj->GetWorldCOM().X == Approx(1.f));

        Obj->SetAngularVelocity(PI * 0.5);
        for(int Step = 0; Step < 2; ++Step)
        {
            AWorld.Integrate(.5);
        }

        const Transform TM = Obj->GetWorldTransform();
        CHECK(TM.Position.X == Approx(1.f));
        CHECK(TM.Position.Y == Approx(-1.f));
        CHECK(TM.Rotation == Approx(0.5f * PI));
    }
}

TEST_CASE( "Simulation", "[simulation]" )
{
    const float Dt = 1.f / 60.f;
    Solver ASolver;
    {
        //two CircleShapes of radius 1 at 0,0 and 1, 0
        World AWorld;
        Actor* A = AWorld.CreateActor();
        SimShape* AShape = A->CreateShape<CircleShape>();

        Actor* B = AWorld.CreateActor();
        B->CreateShape<CircleShape>();
        B->SetWorldTransform(Transform(Vector2(1.f, 0.f)));

        std::vector<ContactManifold> Manifolds;
        ContactManifold M;
        M.A = A;
        M.B = B;
        M.NumContacts = 1;
        M.ContactPoints[0].Position = Vector2::Zero;
        M.ContactPoints[0].Normal = Vector2(1.f, 0.f);
        M.ContactPoints[0].PenetrationDepth = 1.f;
        Manifolds.push_back(M);

        {
            ASolver.Solve(Manifolds);

            const Transform& ATM = A->GetWorldTransform();
            const Transform& BTM = B->GetWorldTransform();
            CHECK(ATM.Position.X == Approx(-0.5f));
            CHECK(ATM.Position.Y == Approx(0.f));
            CHECK(BTM.Position.X == Approx(1.5f));
            CHECK(BTM.Position.Y == Approx(0.f));
        }

        {
            A->SetWorldPosition(Vector2::Zero);
            B->SetWorldPosition(Vector2(1.f,0));
            //now make one actor bigger, make sure the seperation is proportionate
            AShape->SetMass(2.f);
            A->CalculateMassInertiaAndCOM();
            ASolver.Solve(Manifolds);

            const Transform& ATM = A->GetWorldTransform();
            const Transform& BTM = B->GetWorldTransform();
            CHECK(ATM.Position.X == Approx(-0.33333f));
            CHECK(ATM.Position.Y == Approx(0.f));
            CHECK(BTM.Position.X == Approx(1.66666f));
            CHECK(BTM.Position.Y == Approx(0.f));
        }

        {
            //now make one actor kinematic and make sure it doesn't move
            A->SetWorldPosition(Vector2::Zero);
            B->SetWorldPosition(Vector2(1.f,0));
            B->SetKinematic(true);
            ASolver.Solve(Manifolds);

            const Transform& ATM = A->GetWorldTransform();
            const Transform& BTM = B->GetWorldTransform();
            CHECK(ATM.Position.X == Approx(-1.f));
            CHECK(ATM.Position.Y == Approx(0.f));
            CHECK(BTM.Position.X == Approx(1.f));
            CHECK(BTM.Position.Y == Approx(0.f));
        }

        {
            //now make A moving and since restitution is 0 the velocity should be completely gone on x axis
            A->SetWorldPosition(Vector2::Zero);
            B->SetWorldPosition(Vector2(1.f,0));
            A->SetLinearVelocity(Vector2::Zero);
            B->SetLinearVelocity(Vector2::Zero);

            A->SetLinearVelocity(Vector2(1.f, 1.f));
            A->SetRestitution(0.f);
            B->SetRestitution(1.f);
            ASolver.Solve(Manifolds);

            CHECK(A->GetLinearVelocity().X == 0.f);
            CHECK(A->GetLinearVelocity().Y == 1.f);
            CHECK(B->GetLinearVelocity().X == 0.f);
            CHECK(B->GetLinearVelocity().Y == 0.f);
        }

        {
            //now make A moving and since restitution is 1 the velocity should be completely negated on x axis
            A->SetWorldPosition(Vector2::Zero);
            B->SetWorldPosition(Vector2(1.f,0));
            A->SetLinearVelocity(Vector2::Zero);
            B->SetLinearVelocity(Vector2::Zero);

            A->SetLinearVelocity(Vector2(1.f, 1.f));
            A->SetRestitution(1.f);
            B->SetRestitution(1.f);
            ASolver.Solve(Manifolds);

            CHECK(A->GetLinearVelocity().X == -1.f);
            CHECK(A->GetLinearVelocity().Y == 1.f);
            CHECK(B->GetLinearVelocity().X == 0.f);
            CHECK(B->GetLinearVelocity().Y == 0.f);
        }

        {
            //now make A moving and B simulated. Both with restitution 0 so A and B should move together
            A->SetWorldPosition(Vector2::Zero);
            B->SetWorldPosition(Vector2(1.f,0));
            A->SetLinearVelocity(Vector2::Zero);
            B->SetLinearVelocity(Vector2::Zero);
            B->SetKinematic(false);

            A->SetLinearVelocity(Vector2(1.f, 0.f));
            A->SetRestitution(0.f);
            B->SetRestitution(0.f);
            ASolver.Solve(Manifolds);

            CHECK(A->GetLinearVelocity().X == Approx(0.6666666f));
            CHECK(A->GetLinearVelocity().Y == 0.f);
            CHECK(B->GetLinearVelocity().X == Approx(0.6666666f));
            CHECK(B->GetLinearVelocity().Y == 0.f);
        }
        /*

        {
            //now make A moving and since restitution is 1 the velocity should become negative + penetration
            A->SetLinearVelocity(Vector2(1.f, 1.f));
            A->SetRestitution(1.f);
            B->SetLinearVelocity(Vector2::Zero);
            B->SetRestitution(1.f);
            ASolver.Solve(Manifolds);

            CHECK(A->GetLinearVelocity().X == Approx(-1 -0.25f / Dt));
            CHECK(A->GetLinearVelocity().Y == 1.f);
            CHECK(B->GetLinearVelocity().X == 0.f);
            CHECK(B->GetLinearVelocity().Y == 0.f);
        }

        */
    }
}

TEST_CASE( "Forces", "[forces]" )
{

    const float Dt = 1.f / 60.f;
    {
        //make sure forces are applied every frame and then reset to 0
        World AWorld;
        Actor* Ball = AWorld.CreateActor();
        Ball->CreateShape<CircleShape>();

        Ball->AddForce(Vector2(1,2));
        const Vector2& LinearAcceleration = Ball->GetLinearAcceleration();
        CHECK(LinearAcceleration.X == Approx(1.f));
        CHECK(LinearAcceleration.Y == Approx(2.f));
        AWorld.Integrate(Dt);
        CHECK(LinearAcceleration.X == Approx(0.f));
        CHECK(LinearAcceleration.Y == Approx(0.f));
    }

    {
        //properly convert force into acceleration with multiple shapes
        World AWorld;
        Actor* Ball = AWorld.CreateActor();
        SimShape* Circ1 = Ball->CreateShape<CircleShape>();
        Circ1->SetMass(10.f);
        SimShape* Circ2 = Ball->CreateShape<CircleShape>();
        Circ2->SetMass(2.f);
        Ball->CalculateMassInertiaAndCOM();
        CHECK(Ball->GetMass() == Approx(12.f));
        Ball->AddForce(Vector2(1,2));
        const Vector2& LinearAcceleration = Ball->GetLinearAcceleration();
        CHECK(LinearAcceleration.X == Approx(1.f / 12.f));
        CHECK(LinearAcceleration.Y == Approx(2.f / 12.f));
    }

    {
        //properly convert torque into acceleration with multiple shapes
        World AWorld;
        Actor* Obj = AWorld.CreateActor();
        SimShape* Circ1 = Obj->CreateShape<CircleShape>(Transform(Vector2(1,0)));
        Circ1->SetMass(1.f);
        Circ1->SetMomentOfInertia(1.f);
        SimShape* Circ2 = Obj->CreateShape<CircleShape>(Transform(Vector2(-1,0)));
        Circ2->SetMass(2.f);
        Circ2->SetMomentOfInertia(3.f);
        Obj->CalculateMassInertiaAndCOM();
        const Vector2& COM = Obj->GetWorldCOM();
        CHECK(COM.X == Approx( -1.f / 3 ));
        CHECK(COM.Y == Approx( 0.f ));
        const float MOI = Obj->GetMomentOfInertia();
        CHECK(MOI == Approx(6.666666667f));
        Obj->AddTorque(6.666666667f);
        CHECK(Obj->GetAngularAcceleration() == Approx(1.f));
    }

    {
        //properly convert force at location into a torque and force at COM
        World AWorld;
        Actor* Obj = AWorld.CreateActor();
        Obj->CreateShape<CircleShape>();
        Obj->AddForceAtLocation(Vector2(0.f, 1.f), Vector2(1.f, 0.f));
        const Vector2& LinearAcceleration = Obj->GetLinearAcceleration();
        CHECK(LinearAcceleration.X == Approx(0.f));
        CHECK(LinearAcceleration.Y == Approx(1.f));
        CHECK(Obj->GetAngularAcceleration() == Approx(1.f));
    }
}

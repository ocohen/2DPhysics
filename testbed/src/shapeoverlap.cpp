#include "test.h"
#include "transform.h"
#include "world.h"
#include "actor.h"
#include "circle.h"
#include "rectangle.h"
#include "renderer.h"

class ShapeOverlapTest : public Test
{
public:
    bool Init(int, const char**)
    {
        Circle DefCircle(1.f);
        Rectangle DefRectangle(Vector2(1.f,2.f));
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->SetWorldTransform(Transform(Vector2(-2,1), PI*.5));
            AnActor->CreateShape<Circle>(DefCircle);
            AnActor->CreateShape<Rectangle>(DefRectangle, Transform(Vector2(2.f,0.f)));
            AnActor->CreateShape<Circle>(DefCircle, Transform(Vector2(4.f,0.f)));
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->SetWorldTransform(Transform(Vector2(3,1)));
            AnActor->CreateShape<Circle>(DefCircle);
            AnActor->CreateShape<Rectangle>(DefRectangle, Transform(Vector2(-2.f,0.f)));
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->SetWorldTransform(Transform(Vector2(-5,2)));
            AnActor->CreateShape<Circle>(DefCircle);
            AnActor->CreateShape<Rectangle>(DefRectangle, Transform(Vector2(-2.f,2.f)));
        }

        return true;
    }
    
    bool Tick(Renderer& ARenderer, float DeltaTime)
    {
        const float DefaultColor[] = {1.f, 1.f, 1.f};
        const float OverlapColor[] = {1.f, 0.f, 0.f};
        const std::vector<Actor*>& Actors = AWorld.GetAllActors();

        static float Rad = 1.95f;
        Rad += DeltaTime * 0.5;

        for(Actor* AnActor : Actors)
        {
            const Transform& TM = AnActor->GetWorldTransform();
            AnActor->SetWorldTransform(Transform(TM.Position, Rad));
        }

        AWorld.GenerateContactManifolds();
        const std::vector<ContactManifold>& Manifolds = AWorld.GetContactManifolds();

        for(const Actor* AnActor : Actors)
        {
            std::vector<Vector2> Normals;
            std::vector<Vector2> ContactPoints;
            bool bOverlap = false;
            for(const ContactManifold& Manifold : Manifolds)
            {
                if(AnActor == Manifold.A || AnActor == Manifold.B)
                {
                    bOverlap = true;
                    for(int ContactIdx = 0; ContactIdx < Manifold.NumContacts; ++ContactIdx)
                    {
                        if(AnActor == Manifold.A)
                        {
                            const Contact& AContact = Manifold.ContactPoints[ContactIdx];
                            ContactPoints.push_back(AContact.Position);
                            Normals.push_back(AContact.Normal);
                        }
                    }
                }
            }

            ARenderer.DrawActor(AnActor, bOverlap ? OverlapColor : DefaultColor);
            for(size_t ContactIdx = 0; ContactIdx < ContactPoints.size(); ++ContactIdx)
            {
                const Vector2& Pt = ContactPoints[ContactIdx];
                const Vector2& Normal = Normals[ContactIdx];
                const Vector2 Pt2 = Pt + Normal;
                float g[3] = {0,1,0};
                ARenderer.DrawArrow(Pt, Pt2, g, 2);
            }
        }
        return true;
    }

private:
    World AWorld;
};
TEST(ShapeOverlapTest, "shapeoverlap");

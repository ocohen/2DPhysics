#include "test.h"
#include "transform.h"
#include "world.h"
#include "actor.h"
#include "circle.h"
#include "rectangle.h"
#include "renderer.h"

class ShapeOverlap : public Test
{
public:
    bool Init(int, const char**)
    {
        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->SetWorldTransform(Transform(Vector2(-2,1), PI*.5));
            Circle* ACircle = AnActor->CreateShape<Circle>();
            ACircle->SetRadius(1.f);

            Rectangle* ARectangle = AnActor->CreateShape<Rectangle>(Transform(Vector2(2.f,0.f)));
            ARectangle->SetExtents(Vector2(1.f,2.f));
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->SetWorldTransform(Transform(Vector2(3,1)));
            Circle* ACircle = AnActor->CreateShape<Circle>();
            ACircle->SetRadius(1.f);

            Rectangle* ARectangle = AnActor->CreateShape<Rectangle>(Transform(Vector2(-2.f,0.f)));
            ARectangle->SetExtents(Vector2(1.f,2.f));
        }

        {
            Actor* AnActor = AWorld.CreateActor();
            AnActor->SetWorldTransform(Transform(Vector2(-5,2)));
            Circle* ACircle = AnActor->CreateShape<Circle>();
            ACircle->SetRadius(1.f);

            Rectangle* ARectangle = AnActor->CreateShape<Rectangle>(Transform(Vector2(-2.f,2.f)));
            ARectangle->SetExtents(Vector2(1.f,2.f));
        }

        return true;
    }
    
    bool Tick(Renderer& ARenderer, float DeltaTime)
    {
        const float DefaultColor[] = {1.f, 1.f, 1.f};
        const float OverlapColor[] = {1.f, 0.f, 0.f};
        const std::vector<Actor*>& Actors = AWorld.GetAllActors();

        static float Rad = 0.f;
        Rad += DeltaTime;

        for(Actor* AnActor : Actors)
        {
            const Transform& TM = AnActor->GetWorldTransform();
            AnActor->SetWorldTransform(Transform(TM.Position, Rad));
        }

        for(const Actor* AnActor : Actors)
        {
            bool bOverlap = false;
            for(Actor* BActor : Actors)
            {
                if(BActor != AnActor)
                {
                    bOverlap |= AnActor->OverlapTest(BActor);
                }
            }

            ARenderer.DrawActor(AnActor, bOverlap ? OverlapColor : DefaultColor);
        }
        return true;
    }

private:
    World AWorld;
};
TEST(ShapeOverlap, "shapeoverlap");

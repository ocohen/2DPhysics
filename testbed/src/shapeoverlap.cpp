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
        Actor* AnActor = AWorld.CreateActor();
        AnActor->SetPose(Transform(Vector2(-2,1), PI*.5));
        Circle* ACircle = AnActor->CreateShape<Circle>();
        ACircle->SetPosition(Vector2::Zero);
        ACircle->SetRadius(1.f);

        Rectangle* ARectangle = AnActor->CreateShape<Rectangle>();
        ARectangle->SetTransform(Transform(Vector2(2.f,0.f), 0));
        ARectangle->SetExtents(Vector2(1.f,2.f));

        return true;
    }
    
    bool Tick(Renderer& ARenderer, float)
    {
        const std::vector<Actor*>& Actors = AWorld.GetAllActors();
        for(Actor* AnActor : Actors)
        {
            ARenderer.DrawActor(AnActor);
        }
        return true;
    }

private:
    World AWorld;
};
TEST(ShapeOverlap, "shapeoverlap");

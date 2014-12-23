#include "renderer.h"
#include "actor.h"
#include "circle.h"
#include "rectangle.h"

Renderer::Renderer(float Width, float Height)
: TinyRenderer::Renderer(Width, Height, true)
{
    const float Position[] = {0,0,10};
    const float Target[] = {0,0,0};
    const float Up[] = {0,1,0};
    SetCameraLookAt(Position, Target, Up);
}

void Renderer::DrawActor(const Actor* AnActor, const float* Color /*=0*/)
{
    const Transform& WorldTM = AnActor->GetWorldTransform();
    const std::vector<ActorShape*>& ActorShapes = AnActor->GetActorShapes();
    for(ActorShape* Shape : ActorShapes)
    {

        std::vector<Vector2> Vertices;
        Shape->Shape->GenerateRenderVertices(Vertices, WorldTM * Shape->LocalTM);

        for(size_t Count = 0; Count < Vertices.size() -1; ++Count)
        {
            DrawLine(&Vertices[Count].X, &Vertices[Count+1].X, Color);
        }

        DrawLine(&Vertices[Vertices.size()-1].X, &Vertices[0].X, Color);
    }

}

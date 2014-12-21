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

void Renderer::DrawActor(const Actor* AnActor)
{
    const Transform& TM = AnActor->GetPose();
    const std::vector<BaseShape*>& Shapes = AnActor->GetShapes();
    for(BaseShape* Shape : Shapes)
    {

        std::vector<Vector2> Vertices;
        Shape->GenerateRenderVertices(Vertices, 32);
        for(Vector2& V : Vertices)
        {
            V = TM.TransformPosition(V);
        }

        for(size_t Count = 0; Count < Vertices.size() -1; ++Count)
        {
            DrawLine(&Vertices[Count].X, &Vertices[Count+1].X);
        }

        DrawLine(&Vertices[Vertices.size()-1].X, &Vertices[0].X);
    }

}

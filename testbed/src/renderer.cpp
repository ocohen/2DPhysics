#include "renderer.h"
#include "actor.h"
#include "CircleShape.h"
#include "RectangleShape.h"

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
    const std::vector<SimShape*>& Shapes = AnActor->GetShapes();
    for(SimShape* Shape : Shapes)
    {
        std::vector<Vector2> Vertices;
        Shape->GenerateRenderVertices(Vertices, WorldTM * Shape->LocalTM);

        for(size_t Count = 0; Count < Vertices.size() -1; ++Count)
        {
            DrawLine(&Vertices[Count].X, &Vertices[Count+1].X, Color);
        }

        DrawLine(&Vertices[Vertices.size()-1].X, &Vertices[0].X, Color);
    }

    const float green[] = {0,1,0};
    const Vector2& COM = AnActor->GetWorldCOM();
    DrawPoint(&COM.X, &green[0]);

}

void Renderer::DrawArrow(const Vector2& Start, const Vector2& End, const float* Color /*=0*/, const float Thickness /*=1*/)
{
    //make arrow head be 1/4 the length of the arrow and with 25 degrees
    const Transform RotateLeft(Vector2::Zero, PI / 8.f);
    const Transform RotateRight(Vector2::Zero, -PI / 8.f);
    const Vector2 AB = End - Start;
    const Vector2 Feather = -AB * 0.25f;
    const Vector2 TipLeft = End + RotateLeft.TransformVector(Feather);
    const Vector2 TipRight = End + RotateRight.TransformVector(Feather);
    DrawLine(&End.X, &TipLeft.X, Color, Thickness);
    DrawLine(&End.X, &TipRight.X, Color, Thickness);
    DrawLine(&Start.X, &End.X, Color, Thickness);
}

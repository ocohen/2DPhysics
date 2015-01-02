#ifndef OC_RENDERER_H
#define OC_RENDERER_H

#include "tinyrenderer.h"

struct Vector2;
class Actor;
class Renderer : public TinyRenderer::Renderer
{
public:
    Renderer(float Width, float Height);
    void DrawActor(const Actor* AnActor, const float* Color = 0);
    void DrawArrow(const Vector2& Start, const Vector2& End, const float* Color = 0, const float Thickness = 1.f);
};

#endif

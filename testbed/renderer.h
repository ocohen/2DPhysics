#ifndef OC_RENDERER_H
#define OC_RENDERER_H

#include "tinyrenderer.h"

class Actor;
class Renderer : public TinyRenderer::Renderer
{
public:
    Renderer(float Width, float Height);
    void DrawActor(const Actor* AnActor, const float* Color = 0);
};

#endif

#ifndef OC_CIRCLE_H
#define OC_CIRCLE_H

#include "baseshape.h"
#include "vector2.h"

struct Circle : public BaseShape
{
public:
    Circle();
    Circle(const float InRadius);
    void SetRadius(const float InRadius);
    virtual void GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& WorldTM) const override;

public:
    float Radius;
    static Shape::Type ShapeType;
};
#endif

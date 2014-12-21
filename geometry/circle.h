#ifndef OC_CIRCLE_H
#define OC_CIRCLE_H

#include "baseshape.h"
#include "vector2.h"

struct Circle : public BaseShape
{
public:
    Circle();
    Circle(const Vector2& InPosition, const float InRadius);
    void SetPosition(const Vector2& InPosition);
    void SetRadius(const float InRadius);
    virtual void GenerateRenderVertices(std::vector<Vector2>& OutVertices, unsigned NumVertices) const override;

public:
    Vector2 Position;
    float Radius;
    static Shape::Type ShapeType;
};
#endif

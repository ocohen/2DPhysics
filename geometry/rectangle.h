#ifndef OC_RECTANGLE_H
#define OC_RECTANGLE_H

#include "baseshape.h"
#include "transform.h"

struct Rectangle : public BaseShape
{
public:
    Rectangle();
    Rectangle(const Transform& InTM, const Vector2& InExtents);
    void SetTransform(const Transform& InTransform);
    void SetExtents(const Vector2& InExtents);
    virtual void GenerateRenderVertices(std::vector<Vector2>& OutVertices, unsigned NumVertices) const override;

public:
    Transform TM;
    Vector2 Extents;
    static Shape::Type ShapeType;
};
#endif

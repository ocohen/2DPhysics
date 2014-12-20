#ifndef OC_RECTANGLE_H
#define OC_RECTANGLE_H

#include "baseshape.h"
#include "transform.h"

struct Rectangle : public BaseShape
{
public:
    Rectangle();
    void SetTransform(const Transform& InTransform);
    void SetExtents(const Vector2& InExtents);

public:
    Transform TM;
    Vector2 Extents;
    static Shape::Type ShapeType;
};
#endif

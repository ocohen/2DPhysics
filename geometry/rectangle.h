#ifndef OC_RECTANGLE_H
#define OC_RECTANGLE_H

#include "baseshape.h"
#include "transform.h"
#include "convexpolygon.h"

struct Rectangle : public ConvexPolygon
{
public:
    Rectangle();
    Rectangle(const Vector2& InExtents);
    void SetExtents(const Vector2& InExtents);
    virtual void GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& WorldTM) const override;

public:
    Vector2 Extents;
    static Shape::Type ShapeType;
};
#endif

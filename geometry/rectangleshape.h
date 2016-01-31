#ifndef OC_RECTANGLESHAPE_H
#define OC_RECTANGLESHAPE_H

#include "baseshape.h"
#include "transform.h"
#include "convexpolygon.h"

struct RectangleShape : public ConvexPolygon
{
public:
    RectangleShape();
    RectangleShape(const Vector2& InExtents);
    void SetExtents(const Vector2& InExtents);
    virtual void GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& WorldTM) const override;

public:
    Vector2 Extents;
    static Shape::Type ShapeType;

private:
    void UpdateVertices();
};
#endif

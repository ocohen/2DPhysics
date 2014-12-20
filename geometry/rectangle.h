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
    bool OverlapTest(const Rectangle& OtherRectangle) const;

public:
    Transform TM;
    Vector2 Extents;
};
#endif

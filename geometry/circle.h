#ifndef OC_CIRCLE_H
#define OC_CIRCLE_H

#include "baseshape.h"
#include "vector2.h"

struct Circle : public BaseShape
{
public:
    Circle();
    void SetPosition(const Vector2& InPosition);
    void SetRadius(const float InRadius);
    bool OverlapTest(const Circle& OtherCircle) const;

public:
    Vector2 Position;
    float Radius;
};
#endif

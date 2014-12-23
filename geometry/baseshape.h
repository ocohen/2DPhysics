#ifndef OC_BASESHAPE_H
#define OC_BASESHAPE_H

#include "transform.h"
#include <vector>

namespace Shape
{
    enum Type
    {
        Circle,
        Rectangle
    };
}

struct Circle;
struct Rectangle;
class Actor;
struct ShapeOverlap;

struct BaseShape
{
public:
    BaseShape(const Shape::Type InType);
    virtual ~BaseShape(){}
    virtual void GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& WorldTM) const = 0;
    static bool OverlapTest(const BaseShape& A, const Transform& AWorldTM, const BaseShape& B, const Transform& BWorldTM, ShapeOverlap* OverlapResult=0);
    Shape::Type GetType() const { return ShapeType; }

    template <typename T>
    T* Get();

    template <typename T>
    const T* Get() const;

private:
    Shape::Type ShapeType; 
};

template <typename T>
T* BaseShape::Get()
{
    if(T::ShapeType == ShapeType)
    {
        return (T*) this;
    }

    return 0;
}

template <typename T>
const T* BaseShape::Get() const
{
    if(T::ShapeType == ShapeType)
    {
        return (const T*) this;
    }

    return 0;
}
#endif

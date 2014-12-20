#ifndef OC_BASESHAPE_H
#define OC_BASESHAPE_H

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

struct BaseShape
{
public:
    virtual ~BaseShape(){}
    bool OverlapTest(const BaseShape&) const;
    Shape::Type GetType() const { return ShapeType; }

    template <typename T>
    T* Get();

    template <typename T>
    const T* Get() const;
protected:
    BaseShape(const Shape::Type InType);
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

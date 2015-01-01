#ifndef OC_CONVEXPOLYGON_H
#define OC_CONVEXPOLYGON_H

#include "baseshape.h"
#include "transform.h"
#include <vector>

struct ConvexPolygon : public BaseShape
{
public:
    ConvexPolygon();
    ConvexPolygon(Shape::Type ShapeType);  //child classes should always use this constructor to make sure their type is used
    virtual void GenerateRenderVertices(std::vector<Vector2>& OutVertices, const Transform& WorldTM) const override;
    const std::vector<Vector2>& GetVertices() const { return Vertices; }
    void ComputeNormals(std::vector<Vector2>& OutNormals) const;

    //Add vertices in a CCW winding
    void AddVertex(const Vector2& Vertex);

public:
    static Shape::Type ShapeType;
private:
    std::vector<Vector2> Vertices;

};
#endif

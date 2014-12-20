#ifndef OC_TRANSFORM_H
#define OC_TRANSFORM_H

#include "vector2.h"

struct Transform
{
	Vector2 Position;
	float Rotation;

	Transform(const Vector2& InPosition = Vector2::Zero, const float  InRotation = 0.f)
    : Position(InPosition)
    , Rotation(InRotation)
    {}
   
	Vector2 TransformPosition(const Vector2& V) const
	{
		return TransformVector(V) + Position;
	}
	
    
	Vector2 TransformVector(const Vector2 & V) const
	{
        const float CosR = cos(Rotation);
        const float SinR = sin(Rotation);
        return Vector2(CosR * V.X - SinR * V.Y, SinR * V.X + CosR * V.Y);
	}

    Transform operator*(const Transform B) const
    {
        const float NewRotation = Rotation + B.Rotation;
        const Vector2 NewPosition = TransformPosition(B.Position);
        return Transform(NewPosition, NewRotation);
    }

    Transform GetInverse() const
    {
        return Transform(-Position, -Rotation);
    }

    static Transform Identity;
};
#endif

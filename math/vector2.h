#ifndef OC_VECTOR2_H
#define OC_VECTOR2_H

#include "gmath.h"

struct Vector2
{
	float X;
	float Y;
	
	Vector2(){}
	Vector2(const Vector2& Rhs) : X(Rhs.X), Y(Rhs.Y){}
	Vector2(float InX, float InY) : X(InX), Y(InY){}
	
	static float Dot(const Vector2& A, const Vector2& B)
	{
        return A.X * B.X + A.Y * B.Y;
	}
	
	bool IsZero() const
	{
        return ::IsZero(X) && ::IsZero(Y);
	}

    bool IsNear(const Vector2& Rhs) const
    {
        return ::IsNear(X, Rhs.X) && ::IsNear(Y, Rhs.Y);
    }
	
	float Length2() const
	{
		return Dot(*this,*this);
	}
	
	float Length() const
	{
		return sqrt(Length2());
	}
	
	const Vector2& SafeNormalize()
	{
        *this = GetSafeNormal();
        return *this;
	}

    Vector2 GetSafeNormal() const
    {
		const float L = Length();
		if(!::IsZero(L))
		{
			return Vector2(X / L, Y/L);
		}else
        {
            return Vector2::Zero;
        }
    }
	
	const Vector2& Normalize()
	{
        *this = GetNormal();
		return *this;
	}

    Vector2 GetNormal() const
    {
		const float L = Length();
        return Vector2(X / L, Y/L);
    }

    bool operator==(const Vector2& Rhs) const
    {
        return X == Rhs.X && Y == Rhs.Y;
    }
        
    
	Vector2 operator+(const Vector2& Rhs) const
	{
		return Vector2(X+Rhs.X, Y+Rhs.Y);
	}
	
	const Vector2& operator+=(const Vector2& Rhs)
	{
		X+= Rhs.X;
		Y+= Rhs.Y;
		return *this;
	}
	
	Vector2 operator-(const Vector2& Rhs) const
	{
		return Vector2(X-Rhs.X, Y-Rhs.Y);
	}
	
	const Vector2& operator-=(const Vector2& Rhs)
	{
		X-= Rhs.X;
		Y-= Rhs.Y;
		return *this;
	}
	
	const Vector2& operator=(const Vector2& Rhs)
	{
		X = Rhs.X;
		Y = Rhs.Y;
		return *this;
	}
	
	Vector2 operator*(float K) const
	{
		return Vector2(X*K, Y*K);
	}
	
	const Vector2& operator*=(float K)
	{
		X *= K;
		Y *= K;
		return *this;
	}
	
	Vector2 operator/(float K) const
	{
		return Vector2(X/K, Y/K);
	}
	
	const Vector2& operator/=(float K)
	{
		X /= K;
		Y /= K;
		return *this;
	}
    
    Vector2 operator-() const
    {
        return Vector2(-X, -Y);
    }
    
	static Vector2 Zero;  
};

inline Vector2 operator*(float K, const Vector2& Rhs)
{
	return Rhs.operator*(K);
}
#endif

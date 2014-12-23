#ifndef OC_MATH_H
#define OC_MATH_H

#include <cmath>
#include <algorithm>
#include <limits>

const float SMALL_NUMBER = 1e-4;
const float PI = 3.141592653589793238;

template <typename T> T Abs(T Val)
{
	return Val >= 0 ? Val : -Val;
}

inline bool IsNear(float RealValue, float TargetValue)
{
	return Abs(RealValue - TargetValue) <= SMALL_NUMBER;
}

inline bool IsZero(float Val)
{
	return IsNear(Val, 0);
}

template <typename T> T Clamp(T Val, T Min, T Max)
{
    return std::min(std::max(Val, Min), Max);
}

template <typename T> bool InRange(T Val, T Min, T Max)
{
    return Val >= Min && Val <= Max;
}

#endif

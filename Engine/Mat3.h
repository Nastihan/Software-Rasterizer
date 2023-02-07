#pragma once
#include "Vec3.h"

template <typename T>
class _Mat3
{
public:
	_Mat3& operator=(const _Mat3& rhs)
	{
		memcpy(elements, rhs.elements, sizeof(elements));
		return *this;
	}
	_Mat3& operator*=(T rhs)
	{
		for (auto& row : elements)
		{
			for (T& e : row)
			{
				e *= rhs;
			}
		}
		return *this;
	}
	_Mat3 operator*(T rhs) const
	{
		_Mat3 result = *this;
		return result *= rhs;
	}
	_Mat3 operator*(const _Mat3& rhs) const
	{
		_Mat3 result;
		for (size_t j = 0; j < 3; j++)
		{
			for (size_t k = 0; k < 3; k++)
			{
				T sum = (T)0.0;
				for (size_t i = 0; i < 3; i++)
				{
					sum += elements[j][i] * rhs.elements[i][k];
				}
				result.elements[j][k] = sum;
			}
		}
		return result;
	}
	static _Mat3 Identity()
	{
		return {
			(T)1.0,(T)0.0,(T)0.0,
			(T)0.0,(T)1.0,(T)0.0,
			(T)0.0,(T)0.0,(T)1.0
		};
	}
	static _Mat3 Scaling(T factor)
	{
		return{
			factor,(T)0.0,(T)0.0,
			(T)0.0,factor,(T)0.0,
			(T)0.0,(T)0.0,factor
		};
	}
public:
	// [ row ][ col ]
	T elements[3][3];
};

template<typename T>
_Vec3<T>& operator*=(_Vec3<T>& lhs, const _Mat3<T>& rhs)
{
	return lhs = lhs * rhs;
}

template<typename T>
_Vec3<T> operator*(const _Vec3<T>& lhs, const _Mat3<T>& rhs)
{
	return{
		lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0],
		lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1],
		lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2]
	};
}

typedef _Mat3<float> Mat3;
typedef _Mat3<double> Mad3;
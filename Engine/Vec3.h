#pragma once
#include "ChiliMath.h"
#include "Vec2.h"

template <typename T>
class _Vec3 : public _Vec2<T>
{
public:
	_Vec3() {}
	_Vec3(T x, T y,T z)
		:
		_Vec2(x,y),
		z(z)
	{}
	_Vec3(const _Vec3& vect)
		:
		_Vec3(vect.x, vect.y, vect.z)
	{}
	template <typename T2>
	explicit operator _Vec3<T2>() const
	{
		return { (T2)x,(T2)y,(T2)z};
	}
	T		LenSq() const
	{
		return sq(*this);
	}
	T		Len() const
	{
		return sqrt(LenSq());
	}
	_Vec3& Normalize()
	{
		const T length = Len();
		x /= length;
		y /= length;
		z /= length;
		return *this;
	}
	_Vec3	GetNormalized() const
	{
		_Vec3 norm = *this;
		norm.Normalize();
		return norm;
	}
	_Vec3	operator-() const
	{
		return _Vec3(-x, -y, -z);
	}
	_Vec3& operator=(const _Vec3& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}
	_Vec3& operator+=(const _Vec3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	_Vec3& operator-=(const _Vec3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	T		operator*(const _Vec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
	_Vec3	operator+(const _Vec3& rhs) const
	{
		return _Vec3(*this) += rhs;
	}
	_Vec3	operator-(const _Vec3& rhs) const
	{
		return _Vec3(*this) -= rhs;
	}
	_Vec3& operator*=(const T& rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	_Vec3	operator*(const T& rhs) const
	{
		return _Vec3(*this) *= rhs;
	}
	_Vec3 CrossProd(const _Vec3& rhs) {
		return _Vec3(
			y * rhs.z - z * rhs.y ,
			z * rhs.x - x * rhs.z ,
			x * rhs.y - y * rhs.x
		)
	}
	_Vec3& operator/=(const T& rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
	_Vec3	operator/(const T& rhs) const
	{
		return _Vec3(*this) /= rhs;
	}
	bool	operator==(const _Vec3& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	bool	operator!=(const _Vec3& rhs) const
	{
		return !(*this == rhs);
	}
public:
	T z;

};
typedef _Vec3<float> Vec3;
typedef _Vec3<double> Ved3;
typedef _Vec3<int> Vei3;
#pragma once
#include "Pipeline.h"

class SolidEffect
{
public:
	class Vertex
	{
	public:
		Vertex() = default;
		Vertex(const Vec3& pos)
			:
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Vertex& src)
			:
			color(src.color),
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Color& color)
			:
			color(color),
			pos(pos)
		{}
		Vertex& operator+=(const Vertex& rhs)
		{
			pos += rhs.pos;
			return *this;
		}
		Vertex operator+(const Vertex& rhs) const
		{
			return Vertex(*this) += rhs;
		}
		Vertex& operator-=(const Vertex& rhs)
		{
			pos -= rhs.pos;
			return *this;
		}
		Vertex operator-(const Vertex& rhs) const
		{
			return Vertex(*this) -= rhs;
		}
		Vertex& operator*=(float rhs)
		{
			pos *= rhs;
			return *this;
		}
		Vertex operator*(float rhs) const
		{
			return Vertex(*this) *= rhs;
		}
		Vertex& operator/=(float rhs)
		{
			pos /= rhs;
			return *this;
		}
		Vertex operator/(float rhs) const
		{
			return Vertex(*this) /= rhs;
		}

	public:
		Vec3 pos;
		// Color
		Color color;
	};
	class PixelShader
	{
	public:
		template <typename input>
		Color operator()(const input& in)
		{
			return in.color;
		}

	};
public:
	PixelShader ps;

};
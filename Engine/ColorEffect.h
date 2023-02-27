#pragma once
#include "Pipeline.h"
#include "DefaultVertexShader.h"
class ColorEffect 
{
public:
	class Vertex 
	{
		public:
			Vertex() = default;
			Vertex(Vec3 pos)
				: pos(pos)
			{

			}
			// this enables template functions clone a vertex
			// while changing the pos only
			Vertex(const Vec3& pos, const Vertex& src)
				:
				color(src.color),
				pos(pos)
			{}
			Vertex(const Vec3& pos, const Vec3& color)
				:
				color(color),
				pos(pos)
			{}
			Vertex& operator+=(const Vertex& rhs)
			{
				pos += rhs.pos;
				color += rhs.color;
				return *this;
			}
			Vertex operator+(const Vertex& rhs) const
			{
				return Vertex(*this) += rhs;
			}
			Vertex& operator-=(const Vertex& rhs)
			{
				pos -= rhs.pos;
				color -= rhs.color;
				return *this;
			}
			Vertex operator-(const Vertex& rhs) const
			{
				return Vertex(*this) -= rhs;
			}
			Vertex& operator*=(float rhs)
			{
				pos *= rhs;
				color *= rhs;
				return *this;
			}
			Vertex operator*(float rhs) const
			{
				return Vertex(*this) *= rhs;
			}
			Vertex& operator/=(float rhs)
			{
				pos /= rhs;
				color /= rhs;
				return *this;
			}
			Vertex operator/(float rhs) const
			{
				return Vertex(*this) /= rhs;
			}

		public:
			Vec3 pos;
			// Color
			Vec3 color;
	};

	typedef DefaultVertexShader<Vertex> VertexShader;

	class PixelShader
	{
		public:
			template <typename input>
			Color operator()(const input& in) 
			{
				return Color(in.color);
			}

	};
public:
	PixelShader ps;
	VertexShader vs;
};

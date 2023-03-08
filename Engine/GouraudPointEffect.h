#pragma once
#pragma once
#include "Pipeline.h"
#include "DefaultVertexShader.h"
#include "DefaultGeometryShader.h"


class GouraudPointEffect {
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
			n(src.n),
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Vec3& n)
			:
			n(n),
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
		Vec3 n;
	};

	class VertexShader
	{
	public:
		class Output
		{
		public:
			Output() = default;
			Output(const Vec3& pos)
				:
				pos(pos)
			{}
			Output(const Vec3& pos, const Output& src)
				:
				color(src.color),
				pos(pos)
			{}
			Output(const Vec3& pos, const Vec3& color)
				:
				color(color),
				pos(pos)
			{}
			Output& operator+=(const Output& rhs)
			{
				pos += rhs.pos;
				color += rhs.color;
				return *this;
			}
			Output operator+(const Output& rhs) const
			{
				return Output(*this) += rhs;
			}
			Output& operator-=(const Output& rhs)
			{
				pos -= rhs.pos;
				color -= rhs.color;
				return *this;
			}
			Output operator-(const Output& rhs) const
			{
				return Output(*this) -= rhs;
			}
			Output& operator*=(float rhs)
			{
				pos *= rhs;
				color *= rhs;
				return *this;
			}
			Output operator*(float rhs) const
			{
				return Output(*this) *= rhs;
			}
			Output& operator/=(float rhs)
			{
				pos /= rhs;
				color /= rhs;
				return *this;
			}
			Output operator/(float rhs) const
			{
				return Output(*this) /= rhs;
			}
		public:
			Vec3 pos;
			Vec3 color;
		};
	public:
		void BindRotation(const Mat3& rotation_in)
		{
			rotation = rotation_in;
		}
		void BindTranslation(const Vec3& translation_in)
		{
			translation = translation_in;
		}

		Output operator()(const Vertex& v) const
		{

			const auto pos = v.pos * rotation + translation;

			const auto vecL = light_pos - pos;
			const auto distance = vecL.Len();
			const auto dir = vecL / distance;

			const auto attenuation = 1.0f /
				(constant_attenuation + linear_attenuation * distance + quadradic_attenuation * sq(distance));

			const auto d = light_diffuse * attenuation * std::max( 0.0f,(v.n * rotation) * dir );

			const auto c = color.GetHadamard(d + light_ambient).Saturate() * 255.0f;

			return { pos, c };

		}

		void SetLightPos(const Vec3& p)
		{
			light_pos = p;
		}
		void SetAmbientLight(const Vec3& a)
		{
			light_ambient = a;
		}
		void SetDiffuseLight(const Vec3& d)
		{
			light_diffuse = d;
		}

	private:
		Mat3 rotation;
		Vec3 translation;
		Vec3 light_pos = { 0.0f,0.0f,0.5f };
		Vec3 light_diffuse = { 1.0f,1.0f,1.0f };
		Vec3 light_ambient = { 0.1f,0.1f,0.1f };
		Vec3 color = { 0.8f,0.85f,1.0f };
		float linear_attenuation = 0.6f;
		float quadradic_attenuation = 1.3f;
		float constant_attenuation = 0.1f;
	};


	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;

	class PixelShader
	{
	public:
		template<class Input>
		Color operator()(const Input& in) const
		{
			return Color(in.color);
		}
	private:

	};
public:
	PixelShader ps;
	VertexShader vs;
	GeometryShader gs;
};
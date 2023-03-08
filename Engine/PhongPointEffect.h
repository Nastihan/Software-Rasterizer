#pragma once
#include "Pipeline.h"
#include "DefaultGeometryShader.h"


class PhongPointEffect {
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
				n(src.n),
				worldPos(src.worldPos),
				pos(pos)
			{}
			Output(const Vec3& pos, const Vec3& n, const Vec3& worldPos)
				:
				n(n),
				pos(pos),
				worldPos(worldPos)
			{}
			Output& operator+=(const Output& rhs)
			{
				pos += rhs.pos;
				n += rhs.n;
				worldPos += rhs.worldPos;
				return *this;
			}
			Output operator+(const Output& rhs) const
			{
				return Output(*this) += rhs;
			}
			Output& operator-=(const Output& rhs)
			{
				pos -= rhs.pos;
				n -= rhs.n;
				worldPos -= rhs.worldPos;
				return *this;
			}
			Output operator-(const Output& rhs) const
			{
				return Output(*this) -= rhs;
			}
			Output& operator*=(float rhs)
			{
				pos *= rhs;
				n *= rhs;
				worldPos *= rhs;
				return *this;
			}
			Output operator*(float rhs) const
			{
				return Output(*this) *= rhs;
			}
			Output& operator/=(float rhs)
			{
				pos /= rhs;
				n /= rhs;
				worldPos /= +rhs;
				return *this;
			}
			Output operator/(float rhs) const
			{
				return Output(*this) /= rhs;
			}
		public:
			Vec3 pos;
			Vec3 n;
			Vec3 worldPos;
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
			return { pos,v.n * rotation , pos };
		}
	private:
		Mat3 rotation;
		Vec3 translation;
		
	};


	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;

	class PixelShader
	{
	public:
		template<class Input>
		Color operator()(const Input& in) const
		{
					// light to object vector ***** bad naming****
	const auto vecL = light_pos - in.worldPos;
			const auto distance = vecL.Len();
			const auto dir = vecL / distance;
			// calculate attentuation
			const auto attenuation = 1.0f /
				(constant_attenuation + linear_attenuation * distance + quadradic_attenuation * sq(distance));

			const auto d = light_diffuse * attenuation * std::max(0.0f, (in.n.GetNormalized()  * dir));

			const auto c = color.GetHadamard(d + light_ambient).Saturate() * 255.0f;

			return  Color( c );
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
		Vec3 light_pos = { 0.0f,0.0f,0.5f };
		Vec3 light_diffuse = { 1.0f,1.0f,1.0f };
		Vec3 light_ambient = { 0.1f,0.1f,0.1f };
		Vec3 color = { 0.8f,0.85f,1.0f };
		float linear_attenuation = 0.6f;
		float quadradic_attenuation = 1.3f;
		float constant_attenuation = 0.1f;
	};
public:
	PixelShader ps;
	VertexShader vs;
	GeometryShader gs;
};
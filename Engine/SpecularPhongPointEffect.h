#pragma once
#include "Pipeline.h"
#include "DefaultGeometryShader.h"


class SpecularPhongPointEffect {
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
			Output(const Vec4& pos)
				:
				pos(pos)
			{}
			Output(const Vec4& pos, const Output& src)
				:
				n(src.n),
				worldPos(src.worldPos),
				pos(pos)
			{}
			Output(const Vec4& pos, const Vec3& n, const Vec3& worldPos)
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
			Vec4 pos;
			Vec3 n;
			Vec3 worldPos;
		};
	public:
		void BindWorld(const Mat4& transformation_in)
		{
			world = transformation_in;
			worldProj = world * proj;
		}
		void BindProjection(const Mat4& transformation_in)
		{
			proj = transformation_in;
			worldProj = world * proj;
		}
		const Mat4& GetProj() const
		{
			return proj;
		}
		Output operator()(const Vertex& v) const
		{
			const auto p4 = Vec4(v.pos);
			return { p4 * worldProj,Vec4{ v.n,0.0f } *world,p4 * world };
		}
	private:
		Mat4 world = Mat4::Identity();
		Mat4 proj = Mat4::Identity();
		Mat4 worldProj = Mat4::Identity();
	};


	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;

	class PixelShader
	{
	public:
		template<class Input>
		Color operator()(const Input& in) const
		{
			// normalizing the interpolated surface normal
			const auto surfaceNormal = in.n.GetNormalized();
			//in.n.Normalize();
			// light to object vector ***** bad naming****
			const auto vecL = light_pos - in.worldPos;
			const auto distance = vecL.Len();
			const auto dir = vecL / distance;
			// calculate attentuation
			const auto attenuation = 1.0f /
				(constant_attenuation + linear_attenuation * distance + quadradic_attenuation * sq(distance));
			// diffuse
			const auto d = light_diffuse * attenuation * std::max(0.0f, (surfaceNormal * dir));
			// reflection vector
			const auto r = (surfaceNormal * (surfaceNormal * vecL)) * 2.0f  - vecL;
			// specular
			const auto s = light_diffuse * specular_intensity * std::pow(std::max(0.0f, -r.GetNormalized() * in.worldPos.GetNormalized()), specular_power);

			return Color(color.GetHadamard(d + light_ambient + s).Saturate() * 255.0f);

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

		float specular_power = 60.0f;
		float specular_intensity = 0.7f;
	};
public:
	PixelShader ps;
	VertexShader vs;
	GeometryShader gs;
};
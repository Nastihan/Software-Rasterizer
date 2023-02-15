#pragma once
#include "Scene.h"
#include "XQuads.h"
#include "cubeScreenTransformer.h"
#include "Mat3.h"

class XMutualScene : public Scene
{
public:
	XMutualScene() = default;
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override
	{
		if (kbd.KeyIsPressed('Q'))
		{
			theta_x = wrap_angle(theta_x + dTheta * dt);
		}
		if (kbd.KeyIsPressed('W'))
		{
			theta_y = wrap_angle(theta_y + dTheta * dt);
		}
		if (kbd.KeyIsPressed('E'))
		{
			theta_z = wrap_angle(theta_z + dTheta * dt);
		}
		if (kbd.KeyIsPressed('A'))
		{
			theta_x = wrap_angle(theta_x - dTheta * dt);
		}
		if (kbd.KeyIsPressed('S'))
		{
			theta_y = wrap_angle(theta_y - dTheta * dt);
		}
		if (kbd.KeyIsPressed('D'))
		{
			theta_z = wrap_angle(theta_z - dTheta * dt);
		}
		if (kbd.KeyIsPressed('R'))
		{
			offset_z += 2.0f * dt;
		}
		if (kbd.KeyIsPressed('F'))
		{
			offset_z -= 2.0f * dt;
		}
	}
	virtual void Draw(Graphics& gfx) const override {
		auto triangles = hex.GetTriangles();
		// Generate rotation matrix
		const auto rotation = Mat3::RotationZ(theta_z) * Mat3::RotationY(theta_y) * Mat3::RotationX(theta_x);

		for (auto& v : triangles.vertices)
		{
			v *= rotation;
			v += {0.0f, 0.0f, offset_z};
			v += {0.0f, 0.0f, 2.0f};
		}
		// Backface culling
		for (size_t i = 0, end = triangles.indices.size() / 3; i < end; i++)
		{
			const Vec3& v0 = triangles.vertices[triangles.indices[i * 3]];
			const Vec3& v1 = triangles.vertices[triangles.indices[i * 3 + 1]];
			const Vec3& v2 = triangles.vertices[triangles.indices[i * 3 + 2]];


			triangles.cullFlags[i] = (v1 - v0).CrossProd((v2 - v0)) * v0 > 0.0f;
		}
		// Transform to screen space (3d perspective division is also done here) 
		for (auto& v : triangles.vertices)
		{
			cst.Transform(v);
		}
		// Drawing the triangles
		for (size_t i = 0,
			end = triangles.indices.size() / 3;
			i < end; i++)
		{

			if (!triangles.cullFlags[i])
			{
				gfx.DrawTriangle(
					triangles.vertices[triangles.indices[i * 3]],
					triangles.vertices[triangles.indices[i * 3 + 1]],
					triangles.vertices[triangles.indices[i * 3 + 2]],
					colors[i]);
			}
		}

	}
private:
	CubeScreenTransformer cst;
	XQuads hex = XQuads(1.0f);
	static constexpr Color colors[12] = {
		Colors::Red,
		Colors::Red,
		Colors::Yellow,
		Colors::Yellow,
	};
	static constexpr float dTheta = PI;
	float offset_z = 0.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
};
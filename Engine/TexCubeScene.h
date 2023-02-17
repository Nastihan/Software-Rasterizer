#pragma once
#include "Scene.h"
#include "Cube.h"
#include "CubeScreenTransformer.h"
#include "Mat3.h"
#include "Surface.h"

class TexCubeScene : public Scene {
public:
	TexCubeScene() = default;
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
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
		auto triangles = cube.GetTrianglesTex();
		// Generate rotation matrix
		const auto rotation = Mat3::RotationZ(theta_z) * Mat3::RotationY(theta_y) * Mat3::RotationX(theta_x);

		for (auto& v : triangles.vertices)
		{
			v.pos *= rotation;
			v.pos += {0.0f, 0.0f, offset_z};
			v.pos += {0.0f, 0.0f, 2.0f};
		}
		// Backface culling
		for (size_t i = 0, end = triangles.indices.size() / 3; i < end; i++)
		{
			const Vec3& v0 = triangles.vertices[triangles.indices[i * 3]].pos;
			const Vec3& v1 = triangles.vertices[triangles.indices[i * 3 + 1]].pos;
			const Vec3& v2 = triangles.vertices[triangles.indices[i * 3 + 2]].pos;


			triangles.cullFlags[i] = (v1 - v0).CrossProd((v2 - v0)) * v0 > 0.0f;
		}
		// Transform to screen space (3d perspective division is also done here) 
		for (auto& v : triangles.vertices)
		{
			cst.Transform(v.pos);
		}
		// Drawing the triangles
		for (size_t i = 0,
			end = triangles.indices.size() / 3;
			i < end; i++)
		{

			if (!triangles.cullFlags[i])
			{
				gfx.DrawTriangleTex(
					triangles.vertices[triangles.indices[i * 3]],
					triangles.vertices[triangles.indices[i * 3 + 1]],
					triangles.vertices[triangles.indices[i * 3 + 2]],
					tex);
			}
		}

	}

private:
	CubeScreenTransformer cst;
	Cube cube = Cube(1.0f);
	Surface tex = Surface::FromFile(L"Images\\eye-100x100.png");
	static constexpr float dTheta = PI;
	float offset_z = 0.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
};
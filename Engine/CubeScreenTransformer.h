#pragma once
#include "Vec3.h"
#include "Graphics.h"

class CubeScreenTransformer {
public:
	CubeScreenTransformer()
		:
	xFactor (float( Graphics::ScreenWidth / 2 )),
	yFactor (float( Graphics::ScreenHeight / 2))
	{
	}
	template <typename Vertex>
	Vertex& Transform(Vertex& v) const{
		const float zInverse = 1.0f / v.pos.z;

		v *= zInverse;

		v.pos.x = (v.pos.x + 1.0f) * xFactor;
		v.pos.y = (-v.pos.y + 1.0f) * yFactor;

		v.pos.z = zInverse;

		return v;
	}

	template<class Vertex>
	Vertex GetTransformed(const Vertex& v) const
	{
		return Transform(Vertex(v));
	}
private:
	float xFactor;
	float yFactor;
};
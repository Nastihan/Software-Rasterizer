#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include "Graphics.h"

class NDCScreenTransformer {
public:
	NDCScreenTransformer()
		:
	xFactor (float( Graphics::ScreenWidth / 2 )),
	yFactor (float( Graphics::ScreenHeight / 2))
	{
	}
	template <typename Vertex>
	Vertex& Transform(Vertex& v) const{
		const float wInverse = 1.0f / v.pos.w;

		v *= wInverse;

		v.pos.x = (v.pos.x + 1.0f) * xFactor;
		v.pos.y = (-v.pos.y + 1.0f) * yFactor;

		v.pos.w = wInverse;

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
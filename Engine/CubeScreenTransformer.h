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

	Vec3& Transform(Vec3& v) const{
		v.x = (v.x + 1.0f) * xFactor;
		v.y = (-v.y + 1.0f) * yFactor;
		return v;
	}

	Vec3 GetTransformed(const Vec3& v) const
	{
		return Transform(Vec3(v));
	}
private:
	float xFactor;
	float yFactor;
};
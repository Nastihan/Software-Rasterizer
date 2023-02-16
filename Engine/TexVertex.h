#pragma once
#include "Vec2.h"
#include "Vec3.h"

class TexVertex
{
public:

	TexVertex(Vec3 pos, Vec2 tc)
		:	pos(pos),
			tc(tc)
	{

	}
	TexVertex InterpolateTo(const TexVertex& dest, float alpha) const
	{
		return {
			pos.InterpolateTo(dest.pos,alpha),
			tc.InterpolateTo(dest.tc,alpha)
		};
	}

private:
	Vec3 pos;
	// Texture coordinate
	Vec2 tc;
};
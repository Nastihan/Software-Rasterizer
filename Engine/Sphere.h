#pragma once
#include "IndexedTriangleList.h"
#include "Vec3.h"
#include "Mat3.h"


class Sphere
{
public:
	template <typename T>
	static IndexedTriangleList<T> GetPlain(float radius = 1.0f, int latDiv = 12, int longDiv = 24)
	{
		const Vec3 base = { 0.0f,0.0f,radius };
		const auto latAngle = PI / latDiv;
		const auto longAngle = 2.0f * PI / longDiv;

		std::vector<T> vertices;

		for (auto iLat = 1; iLat < latDiv; iLat++)
		{
			const auto latBase = base * Mat3::RotationX(iLat * latAngle);
			for (auto iLong = 0; iLong < longDiv; iLong++)
			{
				const auto vertex = latBase * Mat3::RotationZ(iLong * longAngle);
				vertices.emplace_back();
				vertices.back().pos = vertex;
			}
		}

		const auto iNorthPole = vertices.size();
		vertices.emplace_back();
		vertices.back().pos = base;
		const auto iSouthPole = vertices.size();
		vertices.emplace_back();
		vertices.back().pos = -base;

		const auto calcIdx = [latDiv, longDiv](int iLat, int iLong)
		{ return iLat * longDiv + iLong; };
		std::vector<size_t> indices;
		for (int iLat = 0; iLat < latDiv - 2; iLat++)
		{
			for (int iLong = 0; iLong < longDiv - 1; iLong++)
			{
				indices.push_back(calcIdx(iLat, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong + 1));
			}
			
			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, 0));
			
		}

		for (int iLong = 0; iLong < longDiv - 1; iLong++)
		{
			indices.push_back(iNorthPole);
			indices.push_back(calcIdx(0, iLong));
			indices.push_back(calcIdx(0, iLong + 1));

			indices.push_back(calcIdx(latDiv - 2, iLong + 1));
			indices.push_back(calcIdx(latDiv - 2, iLong));
			indices.push_back(iSouthPole);
		}

		indices.push_back(iNorthPole);
		indices.push_back(calcIdx(0, longDiv - 1));
		indices.push_back(calcIdx(0, 0));

		indices.push_back(calcIdx(latDiv - 2, 0));
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
		indices.push_back(iSouthPole);

		return{ std::move(vertices),std::move(indices) };
	}
	
	template<class V>
	static IndexedTriangleList<V> GetPlainNormals(float radius = 1.0f, int latDiv = 12, int longDiv = 24)
	{
		auto sphere = GetPlain<V>(radius, latDiv, longDiv);
		for (auto& v : sphere.vertices)
		{
			v.n = v.pos.GetNormalized();
		}
		return sphere;
	}
};
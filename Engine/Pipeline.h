#pragma once
#include "Graphics.h"
#include "CubeScreenTransformer.h"
#include "Surface.h"
#include "IndexedTriangleList.h"
#include "Triangle.h"
#include "ChiliMath.h"
#include "Mat3.h"
#include <algorithm>

// fixed-function triangle drawing pipeline
// draws textured triangle lists with clamping
class Pipeline {
public:
	class Vertex {
	public:
		Vertex() = default;
		Vertex(Vec3 pos)
			: pos(pos)
		{

		}
		// this enables template functions clone a vertex
		// while changing the pos only
		Vertex(const Vec3& pos, const Vertex& src)
			:
			t(src.t),
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Vec2& t)
			:
			t(t),
			pos(pos)
		{}
		Vertex& operator+=(const Vertex& rhs)
		{
			pos += rhs.pos;
			t += rhs.t;
			return *this;
		}
		Vertex operator+(const Vertex& rhs) const
		{
			return Vertex(*this) += rhs;
		}
		Vertex& operator-=(const Vertex& rhs)
		{
			pos -= rhs.pos;
			t -= rhs.t;
			return *this;
		}
		Vertex operator-(const Vertex& rhs) const
		{
			return Vertex(*this) -= rhs;
		}
		Vertex& operator*=(float rhs)
		{
			pos *= rhs;
			t *= rhs;
			return *this;
		}
		Vertex operator*(float rhs) const
		{
			return Vertex(*this) *= rhs;
		}
		Vertex& operator/=(float rhs)
		{
			pos /= rhs;
			t /= rhs;
			return *this;
		}
		Vertex operator/(float rhs) const
		{
			return Vertex(*this) /= rhs;
		}

	public:
		Vec3 pos;
		// Texture coordinate
		Vec2 t;
	};
public:
	Pipeline(Graphics& gfx)
		: gfx(gfx)
	{

	}
	void Draw(IndexedTriangleList<Vertex>& triList)
	{
		ProcessVertices(triList.vertices, triList.indices);
	}
	void BindRotation(const Mat3& rotation_in)
	{
		rotation = rotation_in;
	}
	void BindTranslation(const Vec3& translation_in)
	{
		translation = translation_in;
	}
	void BindTexture(const std::wstring& filename)
	{
		pTex = std::make_unique<Surface>(Surface::FromFile(filename));
	}

	// vertex processing function
	// applies rotations, and translations on the vertices and then calls the triangle assembler
private:
	void ProcessVertices(std::vector<Vertex>& vertices, const std::vector<size_t>& indices) {


		std::vector<Vertex> verticesout;

		for (auto v : vertices) {
			verticesout.emplace_back(v.pos * rotation + translation, v.t);
		}

		AssembleTriangles(verticesout, indices);
	}
	// triangle assembly function
	// assembles indexed vertex stream into triangles and passes them to thr triangle processing function
	// does the backface culling
	void AssembleTriangles(const std::vector<Vertex>& vertices, const std::vector<size_t>& indices) {

		// assemble triangles in the stream and process
		for (size_t i = 0, end = indices.size() / 3;
			i < end; i++)
		{
			// determine triangle vertices via indexing
			const auto& v0 = vertices[indices[i * 3]];
			const auto& v1 = vertices[indices[i * 3 + 1]];
			const auto& v2 = vertices[indices[i * 3 + 2]];
			// backface culling
			if ((v1.pos - v0.pos).CrossProd(v2.pos - v0.pos) * v0.pos <= 0.0f)
			{
				// process 3 vertices into a triangle
				ProcessTriangle(v0, v1, v2);
			}
		}
	}
	// triangle processing function
	// takes 3 vertices to generate triangle and calls the post-processing function
	void ProcessTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	{
		// left for later (geometary shader)
		PostProcessTriangleVertices(Triangle<Vertex>{ v0, v1, v2 });
	}
	// vertex post-processing function
	// performs perspective division and screen transformation on the vertices and calls the draw function
	void PostProcessTriangleVertices(Triangle<Vertex>& triangle)
	{

		cst.Transform(triangle.v0.pos);
		cst.Transform(triangle.v1.pos);
		cst.Transform(triangle.v2.pos);
		// perspective division and screen transformation done

		// draw the triangle
		DrawTriangle(triangle);
	}
	void DrawTriangle(const Triangle<Vertex>& triangle) {

		const Vertex* pv0 = &triangle.v0;
		const Vertex* pv1 = &triangle.v1;
		const Vertex* pv2 = &triangle.v2;

		// Sorting vec2 pointers by y
		if (pv1->pos.y < pv0->pos.y) std::swap(pv0, pv1);
		if (pv2->pos.y < pv1->pos.y) std::swap(pv1, pv2);
		if (pv1->pos.y < pv0->pos.y) std::swap(pv0, pv1);

		// natural flat top
		if (pv0->pos.y == pv1->pos.y) {

			if (pv1->pos.x < pv0->pos.x) std::swap(pv0, pv1);
			DrawFlatTopTriangle(*pv0, *pv1, *pv2);
		}

		// natural flat bottom 
		else if (pv1->pos.y == pv2->pos.y) {
			if (pv2->pos.x < pv1->pos.x) std::swap(pv1, pv2);
			DrawFlatBottomTriangle(*pv0, *pv1, *pv2);
		}
		else {

			// find alpha too find the spilitting vertex
			const float alpha = (pv1->pos.y - pv0->pos.y) / (pv2->pos.y - pv0->pos.y);

			const Vertex vi = interpolate(*pv0, *pv2, alpha);

			if (pv1->pos.x < vi.pos.x) // major right
			{
				DrawFlatBottomTriangle(*pv0, *pv1, vi);
				DrawFlatTopTriangle(*pv1, vi, *pv2);
			}
			else // major left
			{
				DrawFlatBottomTriangle(*pv0, vi, *pv1);
				DrawFlatTopTriangle(vi, *pv1, *pv2);
			}
		}
	}
	void DrawFlatTopTriangle(const Vertex& it0,
		const Vertex& it1,
		const Vertex& it2)
	{
		// calculate delta_y 
		const float delta_y = it2.pos.y - it0.pos.y;

		// calulcate dVertex / dy
		// change in interpolant for every 1 change in y
		const Vertex dit0 = (it2 - it0) / delta_y;
		const Vertex dit1 = (it2 - it1) / delta_y;

		// create edge interpolant
		Vertex itEdge1 = it1;

		DrawFlatTriangle(it0, it1, it2, dit0, dit1, itEdge1);
	}
	void DrawFlatBottomTriangle(const Vertex& it0,
		const Vertex& it1,
		const Vertex& it2) {
		// calculate delta_y 
		const float delta_y = it2.pos.y - it0.pos.y;

		// calulcate dVertex / dy
		// change in interpolant for every 1 change in y
		const Vertex dit0 = (it1 - it0) / delta_y;
		const Vertex dit1 = (it2 - it0) / delta_y;

		// create edge interpolant
		Vertex itEdge1 = it0;

		DrawFlatTriangle(it0, it1, it2, dit0, dit1, itEdge1);

	}

	void DrawFlatTriangle(const Vertex& it0,
		const Vertex& it1,
		const Vertex& it2,
		const Vertex& dv0,
		const Vertex& dv1,
		Vertex itEdge1)
	{
		// create edge interpolant for left edge (always v0)
		Vertex itEdge0 = it0;

		// Calculate first and last scanline
		const int yStart = (int)ceil(it0.pos.y - 0.5);
		const int yEnd = (int)ceil(it2.pos.y - 0.5f);

		// interpolants prestep
		itEdge0 += dv0 * (float(yStart) + 0.5f - it0.pos.y);
		itEdge1 += dv1 * (float(yStart) + 0.5f - it0.pos.y);

		// Helper values
		const float tex_width = float(pTex->GetWidth());
		const float tex_height = float(pTex->GetHeight());
		const float tex_xclamp = tex_width - 1.0f;
		const float tex_yclamp = tex_height - 1.0f;

		for (int y = yStart; y < yEnd; y++, itEdge0 += dv0, itEdge1 += dv1) {

			// calculate start and end pixels
			const int xStart = (int)ceil(itEdge0.pos.x - 0.5f);
			const int xEnd = (int)ceil(itEdge1.pos.x - 0.5f);

			// calculate scanline dTexCoord / dx
			auto iLine = itEdge0;
			const float dx = itEdge1.pos.x - itEdge0.pos.x;
			const auto diLine = (itEdge1 - iLine) / dx;
			// create scanline tex coord interpolant and prestep
			iLine += diLine * (float(xStart) + 0.5f - itEdge0.pos.x);


			for (int x = xStart; x < xEnd; x++, iLine += diLine)
			{
				// perform texture lookup, clamp, and write pixel
				gfx.PutPixel(x, y, pTex->GetPixel(
					(unsigned int)std::min(iLine.t.x * tex_width + 0.5f, tex_xclamp),
					(unsigned int)std::min(iLine.t.y * tex_height + 0.5f, tex_yclamp)
				));
			}
		}
	}
private:
	Graphics& gfx;
	CubeScreenTransformer cst;
	Mat3 rotation;
	Vec3 translation;
	std::unique_ptr<Surface> pTex;
};
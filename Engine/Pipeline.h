#pragma once
#include "Graphics.h"
#include "NDCScreenTransformer.h"
#include "Surface.h"
#include "IndexedTriangleList.h"
#include "Triangle.h"
#include "ChiliMath.h"
#include "Mat.h"
#include <algorithm>
#include "ZBuffer.h"
#include "Vec4.h"
#include <memory>





// triangle drawing pipeline with programable
// pixel shading stage

template <typename Effect>
class Pipeline {
	
public:
	typedef typename Effect::Vertex Vertex;
	typedef typename Effect::VertexShader::Output VSOut;
	typedef typename Effect::GeometryShader::Output GSOut;

public:
	Pipeline(Graphics& gfx)
		: Pipeline(gfx,std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight))
	{
	}
	Pipeline(Graphics& gfx, std::shared_ptr<ZBuffer> pZb_in)
		:
		gfx(gfx),
		pZb(std::move(pZb_in))
	{
		assert(pZb->GetHeight() == gfx.ScreenHeight && pZb->GetWidth() == gfx.ScreenWidth);
	}
	
	void Draw(IndexedTriangleList<Vertex>& triList)
	{
		ProcessVertices(triList.vertices, triList.indices);
	}
	
	// ZBuffer resets after each frame
	void BeginFrame()
	{
		pZb->Clear();
	}

	// vertex processing function
	// applies rotations, and translations on the vertices and then calls the triangle assembler
private:
	void ProcessVertices(std::vector<Vertex>& vertices, const std::vector<size_t>& indices) {


		std::vector<VSOut> verticesOut(vertices.size());

		std::transform(vertices.begin(), vertices.end(),
			verticesOut.begin(),
			effect.vs);

		AssembleTriangles(verticesOut, indices);
	}
	// triangle assembly function
	// assembles indexed vertex stream into triangles and passes them to thr triangle processing function
	// does the backface culling
	void AssembleTriangles(const std::vector<VSOut>& vertices, const std::vector<size_t>& indices) 
	{

		const auto eyepos = Vec4{ 0.0f,0.0f,0.0f,1.0f } *effect.vs.GetProj();

		// assemble triangles in the stream and process
		for (size_t i = 0, end = indices.size() / 3;
			i < end; i++)
		{
			// determine triangle vertices via indexing
			const auto& v0 = vertices[indices[i * 3]];
			const auto& v1 = vertices[indices[i * 3 + 1]];
			const auto& v2 = vertices[indices[i * 3 + 2]];
			// backface culling
			if ((v1.pos - v0.pos).CrossProd(v2.pos - v0.pos) * Vec3(v0.pos - eyepos) <= 0.0f)
			{
				// process 3 vertices into a triangle
				ProcessTriangle(v0, v1, v2,i);
			}
		}
	}
	// triangle processing function
	// takes 3 vertices to generate triangle and calls the post-processing function
	void ProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2, size_t triangle_index)
	{
		// generate triangle from 3 vertices using geometry shader 
		// call clipper
		ClipCullTriangle(effect.gs(v0, v1, v2, triangle_index));

	}

	void ClipCullTriangle(Triangle<GSOut>& t)
	{
		// right plane cull test
		if (t.v0.pos.x > t.v0.pos.w &&
			t.v1.pos.x > t.v1.pos.w &&
			t.v2.pos.x > t.v2.pos.w)
		{
			return;
		}
		// left plane cull test
		if (t.v0.pos.x < -t.v0.pos.w &&
			t.v1.pos.x < -t.v1.pos.w &&
			t.v2.pos.x < -t.v2.pos.w)
		{
			return;
		}
		// top plane cull test
		if (t.v0.pos.y > t.v0.pos.w &&
			t.v1.pos.y > t.v1.pos.w &&
			t.v2.pos.y > t.v2.pos.w)
		{
			return;
		}
		// bottom plane cull test
		if (t.v0.pos.y < -t.v0.pos.w &&
			t.v1.pos.y < -t.v1.pos.w &&
			t.v2.pos.y < -t.v2.pos.w)
		{
			return;
		}
		// far plane cull test
		if (t.v0.pos.z > t.v0.pos.w &&
			t.v1.pos.z > t.v1.pos.w &&
			t.v2.pos.z > t.v2.pos.w)
		{
			return;
		}
		// near plane cull test
		if (t.v0.pos.z < 0.0f &&
			t.v1.pos.z < 0.0f &&
			t.v2.pos.z < 0.0f)
		{
			return;
		}

		// geometric clipping for triangles with one vertex on the other side of the z near plane
		const auto Clip1 = [this](GSOut& v0, GSOut& v1, GSOut& v2)
		{
			const auto alpha1 = (-v0.pos.z) / (v1.pos.z - v0.pos.z);
			const auto alpha2 = (-v0.pos.z) / (v2.pos.z - v0.pos.z);

			const auto v0a = interpolate(v0, v1, alpha1);
			const auto v0b = interpolate(v0, v2, alpha2);

			PostProcessTriangleVertices(Triangle<GSOut>{ v0a, v1, v2 });
			PostProcessTriangleVertices(Triangle<GSOut>{ v0b, v0a, v2 });
		};

		// geometric clipping for triangles with two vertices on the other side of the z near plane
		const auto Clip2 = [this](GSOut& v0, GSOut& v1, GSOut& v2)
		{
			
			const float alpha0 = (-v0.pos.z) / (v2.pos.z - v0.pos.z);
			const float alpha1 = (-v1.pos.z) / (v2.pos.z - v1.pos.z);

			v0 = interpolate(v0, v2, alpha0);
			v1 = interpolate(v1, v2, alpha1);

			PostProcessTriangleVertices(Triangle<GSOut>{ v0, v1, v2 });
		};



		if (t.v0.pos.z < 0.0f)
		{
			if (t.v1.pos.z < 0.0f)
			{
				Clip2(t.v0, t.v1, t.v2);
			}
			else if (t.v2.pos.z < 0.0f)
			{
				Clip2(t.v0, t.v2, t.v1);
			}
			else
			{
				Clip1(t.v0, t.v1, t.v2);
			}
		}
		else if (t.v1.pos.z < 0.0f)
		{
			if (t.v2.pos.z < 0.0f)
			{
				Clip2(t.v1, t.v2, t.v0);
			}
			else
			{
				Clip1(t.v1, t.v0, t.v2);
			}
		}
		else if (t.v2.pos.z < 0.0f)
		{
			Clip1(t.v2, t.v0, t.v1);
		}
		else // no near clipping
		{
			PostProcessTriangleVertices(t);
		}

	

	}
	// vertex post-processing function
	// performs perspective division and screen transformation on the vertices and calls the draw function
	void PostProcessTriangleVertices(Triangle<GSOut>& triangle)
	{

		cst.Transform(triangle.v0);
		cst.Transform(triangle.v1);
		cst.Transform(triangle.v2);
		// perspective division and screen transformation done

		// draw the triangle
		DrawTriangle(triangle);
	}
	void DrawTriangle(const Triangle<GSOut>& triangle) {

		const GSOut* pv0 = &triangle.v0;
		const GSOut* pv1 = &triangle.v1;
		const GSOut* pv2 = &triangle.v2;

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

			const auto vi = interpolate(*pv0, *pv2, alpha);

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
	void DrawFlatTopTriangle(const GSOut& it0,
		const GSOut& it1,
		const GSOut& it2)
	{
		// calculate delta_y 
		const float delta_y = it2.pos.y - it0.pos.y;

		// calulcate dVertex / dy
		// change in interpolant for every 1 change in y
		const auto dit0 = (it2 - it0) / delta_y;
		const auto dit1 = (it2 - it1) / delta_y;

		// create edge interpolant
		auto itEdge1 = it1;

		DrawFlatTriangle(it0, it1, it2, dit0, dit1, itEdge1);
	}
	void DrawFlatBottomTriangle(const GSOut& it0,
		const GSOut& it1,
		const GSOut& it2) {
		// calculate delta_y 
		const float delta_y = it2.pos.y - it0.pos.y;

		// calulcate dVertex / dy
		// change in interpolant for every 1 change in y
		const auto dit0 = (it1 - it0) / delta_y;
		const auto dit1 = (it2 - it0) / delta_y;

		// create edge interpolant
		auto itEdge1 = it0;

		DrawFlatTriangle(it0, it1, it2, dit0, dit1, itEdge1);

	}

	void DrawFlatTriangle(const GSOut& it0,
		const GSOut& it1,
		const GSOut& it2,
		const GSOut& dv0,
		const GSOut& dv1,
		GSOut itEdge1)
	{
		// create edge interpolant for left edge (always v0)
		auto itEdge0 = it0;

		// Calculate first and last scanline
		const int yStart = std::max((int)ceil(it0.pos.y - 0.5f), 0);
		const int yEnd = std::min((int)ceil(it2.pos.y - 0.5f), (int)Graphics::ScreenHeight - 1);

		// interpolants prestep
		itEdge0 += dv0 * (float(yStart) + 0.5f - it0.pos.y);
		itEdge1 += dv1 * (float(yStart) + 0.5f - it0.pos.y);

		for (int y = yStart; y < yEnd; y++, itEdge0 += dv0, itEdge1 += dv1) {

			// calculate start and end pixels
			const int xStart = std::max((int)ceil(itEdge0.pos.x - 0.5f), 0);
			const int xEnd = std::min((int)ceil(itEdge1.pos.x - 0.5f), (int)Graphics::ScreenWidth - 1);

			// calculate scanline dTexCoord / dx
			auto iLine = itEdge0;
			const float dx = itEdge1.pos.x - itEdge0.pos.x;
			const auto diLine = (itEdge1 - iLine) / dx;
			// create scanline tex coord interpolant and prestep
			iLine += diLine * (float(xStart) + 0.5f - itEdge0.pos.x);


			for (int x = xStart; x < xEnd; x++, iLine += diLine)
			{

				// depth culling
				// z rejection / update of z buffer
				if (pZb->TestAndSet(x, y, iLine.pos.z))
				{
					 float w = 1.0f / iLine.pos.w;

					const auto attr = iLine * w;

					gfx.PutPixel(x, y, effect.ps(attr));

				}
			}
		
		}
	}
public:
	Effect effect;
private:
	Graphics& gfx;
	NDCScreenTransformer cst;
	Mat3 rotation;
	Vec3 translation;
	std::shared_ptr<ZBuffer> pZb;
	
};
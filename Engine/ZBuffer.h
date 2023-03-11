#pragma once
#include <limits>
#include <cassert>
#include <memory>
#include <vector>
#include <algorithm>

class ZBuffer
{
public:
	ZBuffer(int width, int height)
		: width(width),height(height)
	{
		pBuffer = std::make_unique<std::vector<float>>( width * height );
	}
	~ZBuffer()
	{
		pBuffer.release();
	}
	ZBuffer(const ZBuffer&) = delete;
	ZBuffer& operator=(const ZBuffer&) = delete;

	void Clear()
	{
		const int nDepths = width * height;
		for (int i = 0; i < nDepths; i++)
		{
			(*pBuffer).at(i) = std::numeric_limits<float>::infinity();
		}
	}
	float& At(int x, int y)
	{
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);
		return (*pBuffer).at(y * width + x);
	}
	const float& At(int x, int y) const
	{
		return const_cast<ZBuffer*>(this)->At(x, y);
	}
	bool TestAndSet(int x, int y, float depth)
	{
		float& depthInBuffer = At(x, y);
		if (depth < depthInBuffer)
		{
			depthInBuffer = depth;
			return true;
		}
		return false;
	}
	int GetWidth()
	{
		return width;
	}
	int GetHeight()
	{
		return height;
	}
	/*auto GetMinMax() const
	{
		return std::minmax_element(pBuffer, pBuffer + width * height);
	}*/
private:
	int width;
	int height;
	std::unique_ptr < std::vector<float>> pBuffer ;
};
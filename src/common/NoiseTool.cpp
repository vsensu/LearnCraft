#include "NoiseTool.h"
#include "Perlin_Noise/PerlinNoise.h"

namespace
{
    PerlinNoise pn(237);
}

float NoiseTool::FractalBrownianMotion(float x, float y, int oct, float pers)
{
	float total = 0.f;
	float frequency = 1.f;
	float amplitude = 1.f;
	float max_value = 0.f;
	float offset = 32000.f;
	for (int i = 0; i < oct; ++i)
	{
		total += pn.noise((x + offset) * frequency, (y + offset) * frequency, 0.) * amplitude;
		max_value += amplitude;
		amplitude *= pers;
		frequency *= 2.f;
	}

	return total / max_value;
}

int NoiseTool::GenerateHeight(int x, int y)
{
	return MapTo(0, max_height, 0, 1, FractalBrownianMotion(x * smooth, y * smooth, octaves, persistence));
}

void NoiseTool::GenerateHeightCache(int minx, int miny, int maxx, int maxy)
{
	for(int x = minx; x <= maxx; ++x)
	{
		for(int y = miny; y <= maxy; ++y)
		{
			cached_height_[std::make_tuple(x, y)] = GenerateHeight(x, y);
		}
	}
}

int NoiseTool::GenerateHeightWithCache(int x, int y)
{
	auto result = cached_height_.find(std::make_tuple(x, y));
	if(result != cached_height_.end())
	{
		return result->second;
	}

	return GenerateHeight(x, y);
}

template <typename T>
inline T lerp(T a, T b, T t)
{
	return a + t * (b - a);
}

template <typename T>
inline T inverse_lerp(T a, T b, T value)
{
	if(a == b)
		return a;

	return (value - a) / (b - a);
}

float MapTo(float newmin, float newmax, float originmin, float originmax, float value)
{
	return lerp(newmin, newmax, inverse_lerp(originmin, originmax, value));
}

#pragma once

#include <unordered_map>
#include <tuple>

#include "HashTuple.h"

struct NoiseTool
{
public:
	static int GenerateHeight(int x, int y);
	static void GenerateHeightCache(int minx, int miny, int maxx, int maxy);
	static int GenerateHeightWithCache(int x, int y);

private:
	static float FractalBrownianMotion(float x, float y, int oct, float pers);
	static constexpr int max_height{ 150 };
	static constexpr float smooth{ 0.01f };
	static constexpr int octaves{ 4 };
	static constexpr float persistence{ 0.5f };

	static inline std::unordered_map<std::tuple<int, int>, int, hash_tuple> cached_height_;
};

float MapTo(float newmin, float newmax, float origmin, float originmax, float value);

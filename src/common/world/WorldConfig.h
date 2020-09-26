//
// Created by vsensu on 2020/9/26.
//

#ifndef LEARNCRAFT_SRC_COMMON_WORLD_WORLDCONFIG_H
#define LEARNCRAFT_SRC_COMMON_WORLD_WORLDCONFIG_H

class WorldConfig
{
public:
	[[nodiscard]] static inline constexpr auto ChunkSize() { return kChunkSizeX * kChunkSizeY * kChunkSizeZ; }

	static constexpr int kChunkSizeX{ 15 };
	static constexpr int kChunkSizeY{ 15 };
	static constexpr int kChunkSizeZ{ 15 };
};

#endif //LEARNCRAFT_SRC_COMMON_WORLD_WORLDCONFIG_H

//
// Created by vsensu on 2020/9/26.
//

#ifndef LEARNCRAFT_SRC_COMMON_WORLD_WORLDCONFIG_H
#define LEARNCRAFT_SRC_COMMON_WORLD_WORLDCONFIG_H

class WorldConfig
{
public:
	[[nodiscard]] static inline constexpr auto ChunkSize() { return kChunkSizeX * kChunkSizeY * kChunkSizeZ; }

	static constexpr std::size_t kChunkSizeX{ 15 };
	static constexpr std::size_t kChunkSizeY{ 15 };
	static constexpr std::size_t kChunkSizeZ{ 15 };
};

#endif //LEARNCRAFT_SRC_COMMON_WORLD_WORLDCONFIG_H

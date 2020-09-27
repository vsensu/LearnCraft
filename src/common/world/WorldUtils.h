//
// Created by vsensu on 2020/9/26.
//

#ifndef LEARNCRAFT_SRC_COMMON_WORLD_WORLDUTILS_H
#define LEARNCRAFT_SRC_COMMON_WORLD_WORLDUTILS_H

#include "WorldTypes.h"
#include "WorldConfig.h"

class WorldUtils
{
public:
	[[nodiscard]] static inline bool IsVoxelOutOfChunkBounds(const VoxelIndex &voxelIndex)
	{
		return
		voxelIndex.x < 0 || voxelIndex.x >= WorldConfig::kChunkSizeX ||
				voxelIndex.y < 0 || voxelIndex.y >= WorldConfig::kChunkSizeY ||
					voxelIndex.z < 0 || voxelIndex.z >= WorldConfig::kChunkSizeZ;
	}

	static ChunkIndex
	GetChunkIndexViaLocation(float x, float y, float z);
	static VoxelIndex
	GlobalLocationToVoxelIndex(float x, float y, float z);
	static Position
	VoxelIndexToGlobalLocation(const VoxelIndex &voxelIndex);

	static inline Position ChunkIndexToPosition(const ChunkIndex &chunkIndex)
	{
		auto [chunk_index_x, chunk_index_y, chunk_index_z] = chunkIndex;
		return Position{chunk_index_x * WorldConfig::kChunkSizeX, chunk_index_y * WorldConfig::kChunkSizeY, chunk_index_z * WorldConfig::kChunkSizeZ};
	}

	static inline std::size_t voxel_index_to_data_index(const VoxelIndex &voxelIndex)
	{
		return voxelIndex.x * (WorldConfig::kChunkSizeY * WorldConfig::kChunkSizeZ) + voxelIndex.z * WorldConfig::kChunkSizeY + voxelIndex.y;
	}

	static inline voxel_t make_air_voxel()
	{
		return static_cast<voxel_t>(CommonVoxel::Air);
	}
};

#endif //LEARNCRAFT_SRC_COMMON_WORLD_WORLDUTILS_H

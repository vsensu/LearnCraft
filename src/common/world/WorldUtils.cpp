//
// Created by vsensu on 2020/9/26.
//

#include "WorldUtils.h"

ChunkIndex
WorldUtils::GetChunkIndexViaLocation(float x, float y, float z)
{
	int index_x = static_cast<int>(std::floor(x / WorldConfig::kChunkSizeX));
	int index_y = static_cast<int>(std::floor(y / WorldConfig::kChunkSizeY));
	int index_z = static_cast<int>(std::floor(z / WorldConfig::kChunkSizeZ));
	return std::make_tuple(index_x, index_y, index_z);
}

VoxelIndex
WorldUtils::GlobalLocationToVoxelIndex(float x, float y, float z)
{
	auto[chunk_index_x, chunk_index_y, chunk_index_z] = GetChunkIndexViaLocation(x, y, z);
	int block_x = static_cast<int>(std::floor(x - chunk_index_x * WorldConfig::kChunkSizeX));
	int block_y = static_cast<int>(std::floor(y - chunk_index_y * WorldConfig::kChunkSizeY));
	int block_z = static_cast<int>(std::floor(z - chunk_index_z * WorldConfig::kChunkSizeZ));
	return VoxelIndex(block_x, block_y, block_z);
}
Position
WorldUtils::VoxelIndexToGlobalLocation(const VoxelIndex& voxelIndex)
{
	return Position{};
}

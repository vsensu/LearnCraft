#pragma once

#include <array>

#include "World.h"
#include "WorldConfig.h"
#include "WorldUtils.h"

class Chunk final
{
public:
	Chunk(World& world, const ChunkIndex &chunkIndex) : world_(world), chunk_index_(chunkIndex) {}

	voxel_t
	GetVoxel(const VoxelIndex &voxelIndex) const
	{
		return voxels_[WorldUtils::voxel_index_to_data_index(voxelIndex)];
	}

	bool
	SetVoxel(const VoxelIndex &voxelIndex, voxel_t t)
    {
	    if(WorldUtils::IsVoxelOutOfChunkBounds(voxelIndex))
	        return false;

        voxels_[WorldUtils::voxel_index_to_data_index(voxelIndex)] = t;
	    return true;
    }

    inline const ChunkIndex &
    GetChunkIndex() const { return chunk_index_; }

	void
	Save();

	static Chunk*
	Load(World& world, ChunkIndex index);

	static bool
	IsStored(ChunkIndex index);

private:
	World& world_;
    ChunkIndex chunk_index_;
    std::array<voxel_t , WorldConfig::ChunkSize()> voxels_;
};

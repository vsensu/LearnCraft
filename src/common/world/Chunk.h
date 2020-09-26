#pragma once

#include <array>

#include "World.h"
#include "WorldConfig.h"
#include "WorldUtils.h"

class Chunk
{
public:
	Chunk(World& world);

	~Chunk();

	voxel_t
	GetVoxel(const VoxelIndex &voxelIndex) const
	{
		return data->chunk_data[WorldUtils::voxel_index_to_data_index(voxelIndex)];
	}

//	bool
//	SetVoxel(const VoxelIndex &localPos, voxel_t t);

	void
	Save();

	static Chunk*
	Load(World& world, ChunkIndex index);

	static bool
	IsStored(ChunkIndex index);

//private:
	World& mWorld;
	ChunkData *data;
};

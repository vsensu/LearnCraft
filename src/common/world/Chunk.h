#pragma once

#include <array>

#include "World.h"

class Chunk
{
public:
	Chunk(World& world, ChunkIndex index);

	~Chunk();

	[[nodiscard]] inline bool IsVoxelOutOfChunkBounds(const VoxelLocalPosition &voxelPosition) const {
		return
			 voxelPosition.x >= mWorld.kChunkSizeX ||
				 voxelPosition.y >= mWorld.kChunkSizeY ||
					voxelPosition.z >= mWorld.kChunkSizeZ;
	}

	void
	CreateChunkData();

	bool
	IsSolid(const VoxelLocalPosition &localPos);

	voxel_t
	GetVoxel(const VoxelLocalPosition &localPos) const
	{
		return voxels_[localPos.x][localPos.y][localPos.z];
	}

	bool
	SetVoxel(const VoxelLocalPosition &localPos, voxel_t t);

	void
	Save();

	static Chunk*
	Load(World& world, ChunkIndex index);

	static bool
	IsStored(ChunkIndex index);

private:
	World& mWorld;
	ChunkIndex chunkIndex;
	// Block voxels_[World::kChunkSizeX][World::kChunkSizeY][World::kChunkSizeZ]; // x,y,z
	voxel_t *** voxels_{ nullptr }; // x,y,z
	int chunkStartIndexX;
	int chunkStartIndexY;
	int chunkStartIndexZ;
};

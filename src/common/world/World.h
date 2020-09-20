#pragma once

#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "common/HashTuple.h"
#include "common/types.h"
#include "VoxelManager.h"
#include "WorldTypes.h"

//class Block
//{
//public:
//	Block() = default;
//
//	BlockType Type;
//	bool IsSolid{ true };
//};

inline voxel_t make_air_voxel()
{
	return 0;
}

class World
{
public:
	World(int sx, int sy, int sz);
	~World();

	class Chunk*
	CreateChunkData(ChunkIndex index);
	class Chunk*
	LoadChunkData(ChunkIndex index);
	class Chunk*
	GetChunkData(ChunkIndex index) const;
	void
	DestroyChunkData(ChunkIndex index);

	voxel_t
	GetVoxel(float x, float y, float z) const;
	bool
	SetVoxel(float x, float y, float z, voxel_t t);
	ChunkIndex
	GetChunkIndexViaLocation(float x, float y, float z) const;
	std::tuple<int, int, int>
	BlockGlobalLocationToChunkLocation(float x, float y, float z) const;

	bool
	HasChunk(ChunkIndex index) const
	{
		return chunks_.find(index) != chunks_.end();
	}

	int kChunkSizeX{ 32 };
	int kChunkSizeY{ 32 };
	int kChunkSizeZ{ 32 };

	void
	Save();
	void
	Load();
	bool
	IsChunkStored(ChunkIndex index);

	VoxelManager voxel_manager;
private:
	std::unordered_map<ChunkIndex, class Chunk*, hash_tuple> chunks_;
};

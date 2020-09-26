#pragma once

#include <memory>
#include <unordered_map>
#include <array>

#include <glm/glm.hpp>

#include "common/HashTuple.h"
#include "common/types.h"
#include "VoxelManager.h"
#include "WorldTypes.h"
#include "WorldConfig.h"

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

struct ChunkData
{
	ChunkIndex chunk_index;
	std::array<voxel_t , WorldConfig::ChunkSize()> chunk_data;
};

class World
{
public:
	World();
	~World();

	class Chunk*
	GetChunkData(ChunkIndex index) const;

	[[deprecated("Use GetVoxel(const Position &pos).")]]
	voxel_t
	GetVoxel(float x, float y, float z) const;

	voxel_t
	GetVoxel(const Position &pos) const;

	voxel_t
	GetVoxel(const ChunkIndex& chunkIndex, const VoxelIndex &voxelIndex) const;

	inline bool IsVoxelTypeAir(const ChunkIndex& chunkIndex, const VoxelIndex &voxelIndex)
	{
		return voxel_manager.GetType(GetVoxel(chunkIndex, voxelIndex)).air;
	}

	inline bool IsVoxelTypeAir(const Position &pos)
	{
		return voxel_manager.GetType(GetVoxel(pos)).air;
	}

	inline bool IsVoxelTypeSolid(const ChunkIndex& chunkIndex, const VoxelIndex &voxelIndex)
	{
		return voxel_manager.GetType(GetVoxel(chunkIndex, voxelIndex)).solid;
	}

	inline bool IsVoxelTypeSolid(const Position &pos)
	{
		return voxel_manager.GetType(GetVoxel(pos)).solid;
	}

	inline bool IsVoxelTypeSolidUnbound(const ChunkIndex& chunkIndex, const VoxelIndex& unboundIndex)
	{
		return voxel_manager.GetType(GetVoxelViaUnboundIndex(chunkIndex, unboundIndex)).solid;
	}

	voxel_t GetVoxelViaUnboundIndex(const ChunkIndex& chunkIndex, const VoxelIndex& unboundIndex);

	void AddChunkData(const ChunkIndex &chunkIndex, Chunk *chunk)
	{
		chunks_[chunkIndex] = chunk;
	}

//	bool
//	SetVoxel(float x, float y, float z, voxel_t t);

	bool
	HasChunk(ChunkIndex index) const
	{
		return chunks_.find(index) != chunks_.end();
	}



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

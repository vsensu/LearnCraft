#pragma once

#include <memory>
#include <unordered_map>
#include <array>

#include <glm/glm.hpp>

#include "common/HashTuple.h"
#include "common/types.h"
#include "VoxelManager.h"
#include "TextureManager.h"
#include "WorldTypes.h"
#include "WorldConfig.h"

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

	inline bool IsVoxelTypeOpaque(const ChunkIndex& chunkIndex, const VoxelIndex &voxelIndex)
	{
		return voxel_manager_.GetTypeData(GetVoxel(chunkIndex, voxelIndex)).opaque;
	}

	inline bool IsVoxelTypeOpaque(const Position &pos)
	{
		return voxel_manager_.GetTypeData(GetVoxel(pos)).opaque;
	}

	inline bool IsVoxelTypeSolidUnbound(const ChunkIndex& chunkIndex, const UnboundVoxelIndex& unboundIndex)
	{
		return voxel_manager_.GetTypeData(GetVoxelViaUnboundIndex(chunkIndex, unboundIndex)).opaque;
	}

	voxel_t GetVoxelViaUnboundIndex(const ChunkIndex& chunkIndex, const UnboundVoxelIndex& unboundIndex);

	void AddChunk(const ChunkIndex &chunkIndex, Chunk *chunk)
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

    tex_t GetVoxelTextureLayer(voxel_t voxel, CubeSide side)
    {
        switch (side) {
            case CubeSide::Top:
                return texture_manager_.GetVoxelTextureLayer(voxel_manager_.GetTypeData(voxel).textures.top);
            case CubeSide::Bottom:
                return texture_manager_.GetVoxelTextureLayer(voxel_manager_.GetTypeData(voxel).textures.bottom);
            case CubeSide::Left:
                return texture_manager_.GetVoxelTextureLayer(voxel_manager_.GetTypeData(voxel).textures.left);
            case CubeSide::Right:
                return texture_manager_.GetVoxelTextureLayer(voxel_manager_.GetTypeData(voxel).textures.right);
            case CubeSide::Front:
                return texture_manager_.GetVoxelTextureLayer(voxel_manager_.GetTypeData(voxel).textures.front);
            case CubeSide::Back:
                return texture_manager_.GetVoxelTextureLayer(voxel_manager_.GetTypeData(voxel).textures.back);
        }
        return texture_manager_.GetVoxelTextureLayer(voxel_manager_.GetTypeData(voxel).textures.top);
    }

    void
    Save();
	void
	Load();
	bool
	IsChunkStored(ChunkIndex index);

	inline VoxelManager&
	GetVoxelManager() { return voxel_manager_; }

    inline TextureManager&
    GetTextureManager() { return texture_manager_; }

private:
    VoxelManager voxel_manager_;
	TextureManager texture_manager_;
	std::unordered_map<ChunkIndex, class Chunk*, hash_tuple> chunks_;
};

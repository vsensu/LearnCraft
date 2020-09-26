#include "World.h"

#include <cmath>

#include "Chunk.h"
#include "WorldUtils.h"

struct CustomData
{
	int hp;
};


World::World()
{
	CustomData a {10};
	if(!voxel_manager.RegisterType(static_cast<voxel_t>(CommonVoxel::Air), VoxelTypeData { "Air", false, true, a }))
	{
	}
	if(!voxel_manager.RegisterType(static_cast<voxel_t>(CommonVoxel::Dirt), VoxelTypeData { "Dirt", true, false }))
	{
	}
	if(!voxel_manager.RegisterType(static_cast<voxel_t>(CommonVoxel::Grass), VoxelTypeData { "Grass", true, false }))
	{
	}
	if(!voxel_manager.RegisterType(static_cast<voxel_t>(CommonVoxel::Stone), VoxelTypeData { "Stone", true, false }))
	{
	}
}

World::~World()
{
	for (auto&& chunk : chunks_)
	{
		delete chunk.second;
	}
	chunks_.clear();
}

Chunk*
World::GetChunkData(ChunkIndex index) const
{
	if (auto result = chunks_.find(index); result != chunks_.end())
	{
		return result->second;
	}
	else
	{
		return nullptr;
	}
}

voxel_t
World::GetVoxel(float x, float y, float z) const
{
	// Get chunk
	auto chunk_index = WorldUtils::GetChunkIndexViaLocation(x, y, z);

	// return block
	return GetVoxel(chunk_index, WorldUtils::GlobalLocationToVoxelIndex(x, y, z));
}

voxel_t
World::GetVoxel(const Position& pos) const
{
	// Get chunk
	auto chunk_index = WorldUtils::GetChunkIndexViaLocation(pos.x, pos.y, pos.z);

	// return block
	return GetVoxel(chunk_index, WorldUtils::GlobalLocationToVoxelIndex(pos.x, pos.y, pos.z));
}

voxel_t
World::GetVoxel(const ChunkIndex& chunkIndex, const VoxelIndex& voxelIndex) const
{
	auto* chunk_data = GetChunkData(chunkIndex);
	if (chunk_data != nullptr)
	{
		if (WorldUtils::IsVoxelOutOfChunkBounds(voxelIndex))
		{
//			UE_LOG(LogTemp, Warning, TEXT("block %d, %d, %d index not valid"), block_x, block_y, block_z);
			return make_air_voxel();
		}
		return chunk_data->GetVoxel(VoxelIndex(voxelIndex.x, voxelIndex.y, voxelIndex.z));
	}
	else
	{
		auto[index_x, index_y, index_z] = chunkIndex;
//		UE_LOG(LogTemp, Warning, TEXT("chunk %d, %d, %d not found"), index_x, index_y, index_z);
		return make_air_voxel();
	}
}

//bool
//World::SetVoxel(float x, float y, float z, voxel_t t)
//{
//	auto chunk_index = WorldUtils::GetChunkIndexViaLocation(x, y, z);
//	auto* chunk_data = GetChunkData(chunk_index);
//	if (chunk_data != nullptr)
//	{
//		auto voxel_index = WorldUtils::GlobalLocationToVoxelIndex(x, y, z);
//		chunk_data->SetVoxel(voxel_index, t);
//		return true;
//	}
//	return false;
//}

void
World::Save()
{
	// save world info

	// save chunk data
	for (auto& chunk : chunks_)
	{
		chunk.second->Save();
	}
}

void
World::Load()
{
}

bool
World::IsChunkStored(ChunkIndex index)
{
	return Chunk::IsStored(index);
}
voxel_t
World::GetVoxelViaUnboundIndex(const ChunkIndex& chunkIndex, const UnboundVoxelIndex& unboundIndex)
{
	if(WorldUtils::IsVoxelOutOfChunkBounds(unboundIndex))
	{
		// neighbour
		auto[index_x, index_y, index_z] = chunkIndex;
		int x = unboundIndex.x, y = unboundIndex.y, z = unboundIndex.z;
		if (x < 0)
		{
			index_x--;
			x = WorldConfig::kChunkSizeX - 1;
		}
		else if (x > WorldConfig::kChunkSizeX - 1)
		{
			index_x++;
			x = 0;
		}

		if (y < 0)
		{
			index_y--;
			y = WorldConfig::kChunkSizeY - 1;
		}
		else if (y > WorldConfig::kChunkSizeY - 1)
		{
			index_y++;
			y = 0;
		}

		if (z < 0)
		{
			index_z--;
			z = WorldConfig::kChunkSizeZ - 1;
		}
		else if (z > WorldConfig::kChunkSizeZ - 1)
		{
			index_z++;
			z = 0;
		}

		auto new_chunk_index = std::make_tuple(index_x, index_y, index_z);
		if (new_chunk_index == chunkIndex)
		{
//			UE_LOG(LogTemp, Warning, TEXT("Recursive chunk: %d,%d,%d"), index_x, index_y, index_z);
			return static_cast<voxel_t>(CommonVoxel::Stone);
		}

		// TODO: Optimize, cache neighbor chunk data
		auto chunk_data = GetChunkData(new_chunk_index);
		if (nullptr == chunk_data)
		{
			// 邻接区块为null时，认为是固体
			return static_cast<voxel_t>(CommonVoxel::Stone);
		}
		else
		{
			auto new_voxel_index = VoxelIndex(x, y, z);
			return GetVoxel(new_chunk_index, new_voxel_index);
		}
	}
	else
	{
		return GetVoxel(chunkIndex, unboundIndex);
	}
}

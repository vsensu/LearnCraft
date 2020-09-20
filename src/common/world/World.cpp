#include "World.h"

#include <cmath>

#include "Chunk.h"

struct CustomData
{
	int hp;
};


World::World(int sx, int sy, int sz)
	: kChunkSizeX(sx), kChunkSizeY(sy), kChunkSizeZ(sz)
{
	CustomData a {10};
	if(!voxel_manager.RegisterType(static_cast<voxel_t>(CommonVoxel::Air), VoxelData { "Air", false, a }))
	{
	}
	if(!voxel_manager.RegisterType(static_cast<voxel_t>(CommonVoxel::Dirt), VoxelData { "Dirt", true }))
	{
	}
	if(!voxel_manager.RegisterType(static_cast<voxel_t>(CommonVoxel::Grass), VoxelData { "Grass", true }))
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
World::CreateChunkData(ChunkIndex index)
{
	if (auto chunk_data = GetChunkData(index); chunk_data != nullptr)
		return chunk_data;

	auto chunk = new Chunk(*this, index);
	chunk->CreateChunkData();
	chunks_[index] = chunk;
	return chunk;
}

Chunk*
World::LoadChunkData(ChunkIndex index)
{
	if (auto chunk_data = GetChunkData(index); chunk_data != nullptr)
		return chunk_data;

	auto chunk = Chunk::Load(*this, index);
	chunks_[index] = chunk;
	return chunk;
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

void
World::DestroyChunkData(ChunkIndex index)
{
	if (auto result = chunks_.find(index); result != chunks_.end())
	{
		delete result->second;
		chunks_.erase(result);
	}
}

voxel_t
World::GetVoxel(float x, float y, float z) const
{
	// Get chunk
	auto chunk_index = GetChunkIndexViaLocation(x, y, z);

	// return block
	auto* chunk_data = GetChunkData(chunk_index);
	if (chunk_data != nullptr)
	{
		auto[block_x, block_y, block_z] = BlockGlobalLocationToChunkLocation(x, y, z);
		if (block_x < 0 || block_y < 0 || block_z < 0 || block_x > World::kChunkSizeX - 1
			|| block_y > World::kChunkSizeY - 1 || block_z > World::kChunkSizeZ - 1)
		{
//			UE_LOG(LogTemp, Warning, TEXT("block %d, %d, %d index not valid"), block_x, block_y, block_z);
			return make_air_voxel();
		}
		return chunk_data->GetVoxel(VoxelLocalPosition(block_x, block_y, block_z));
	}
	else
	{
		auto[index_x, index_y, index_z] = chunk_index;
//		UE_LOG(LogTemp, Warning, TEXT("chunk %d, %d, %d not found"), index_x, index_y, index_z);
		return make_air_voxel();
	}
}

bool
World::SetVoxel(float x, float y, float z, voxel_t t)
{
	auto chunk_index = GetChunkIndexViaLocation(x, y, z);
	auto* chunk_data = GetChunkData(chunk_index);
	if (chunk_data != nullptr)
	{
		auto[block_x, block_y, block_z] = BlockGlobalLocationToChunkLocation(x, y, z);
		chunk_data->SetVoxel(VoxelLocalPosition(block_x, block_y, block_z), t);
		return true;
	}
	return false;
}

ChunkIndex
World::GetChunkIndexViaLocation(float x, float y, float z) const
{
	int index_x = static_cast<int>(std::floor(x / kChunkSizeX));
	int index_y = static_cast<int>(std::floor(y / kChunkSizeY));
	int index_z = static_cast<int>(std::floor(z / kChunkSizeZ));
	return std::make_tuple(index_x, index_y, index_z);
}

std::tuple<int, int, int>
World::BlockGlobalLocationToChunkLocation(float x, float y, float z) const
{
	auto[chunk_index_x, chunk_index_y, chunk_index_z] = GetChunkIndexViaLocation(x, y, z);
	int block_x = static_cast<int>(std::floor(x - chunk_index_x * kChunkSizeX));
	int block_y = static_cast<int>(std::floor(y - chunk_index_y * kChunkSizeY));
	int block_z = static_cast<int>(std::floor(z - chunk_index_z * kChunkSizeZ));
	return std::make_tuple(block_x, block_y, block_z);
}

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

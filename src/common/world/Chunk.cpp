#include "Chunk.h"

#include <sstream>
#include <fstream>
#include <filesystem>

#include "common/NoiseTool.h"

//#include "Common/Path.h"

Chunk::Chunk(World& world, ChunkIndex index)
	: mWorld(world), chunkIndex(index)
{
	auto[x, y, z] = index;
	chunkStartIndexX = x * mWorld.kChunkSizeX;
	chunkStartIndexY = y * mWorld.kChunkSizeY;
	chunkStartIndexZ = z * mWorld.kChunkSizeZ;

	voxels_ = new voxel_t ** [mWorld.kChunkSizeX];
	for (int ix = 0; ix < mWorld.kChunkSizeX; ++ix)
	{
		voxels_[ix] = new voxel_t * [mWorld.kChunkSizeY];
		for (int iy = 0; iy < mWorld.kChunkSizeY; ++iy)
		{
			voxels_[ix][iy] = new voxel_t [mWorld.kChunkSizeZ];
			for (int iz = 0; iz < mWorld.kChunkSizeZ; ++iz)
			{
				voxels_[ix][iy][iz] = make_air_voxel();
			}
		}
	}
}

Chunk::~Chunk()
{
	for (int ix = 0; ix < mWorld.kChunkSizeX; ++ix)
	{
		for (int iy = 0; iy < mWorld.kChunkSizeY; ++iy)
		{
			delete[] voxels_[ix][iy];
		}
		delete[] voxels_[ix];
	}
	delete voxels_;
}

void
Chunk::CreateChunkData()
{
	const int dirt_base = 60;
	for (int y = 0; y < mWorld.kChunkSizeY; ++y)
	{
		for (int x = 0; x < mWorld.kChunkSizeX; ++x)
		{
			auto horizontal = NoiseTool::GenerateHeightWithCache(chunkStartIndexX + x, chunkStartIndexY + y);
			for (int z = 0; z < mWorld.kChunkSizeZ; ++z)
			{
				auto& block = voxels_[x][y][z];
				int global_z = chunkStartIndexZ + z;
				if (global_z > horizontal)
				{
					block = static_cast<voxel_t>(CommonVoxel::Air);
				}
				else if (global_z == horizontal)
				{
					block = static_cast<voxel_t>(CommonVoxel::Grass);
				}
				else if (global_z >= dirt_base)
				{
					block = static_cast<voxel_t>(CommonVoxel::Dirt);
				}
				else
				{
					block = static_cast<voxel_t>(CommonVoxel::Stone);
				}
			}
		}
	}
}

bool
Chunk::IsSolid(const VoxelLocalPosition &localPos)
{
	if(IsVoxelOutOfChunkBounds(localPos))
		return true;

	auto x = localPos.x, y = localPos.y, z = localPos.z;
	return mWorld.voxel_manager.GetType(voxels_[x][y][z]).solid;

	// neighbour
	if (x < 0 || y < 0 || z < 0 || x > mWorld.kChunkSizeX - 1 || y > mWorld.kChunkSizeY - 1
		|| z > mWorld.kChunkSizeZ - 1)
	{
		auto[index_x, index_y, index_z] = chunkIndex;
		if (x < 0)
		{
			index_x--;
			x = mWorld.kChunkSizeX - 1;
		}
		else if (x > mWorld.kChunkSizeX - 1)
		{
			index_x++;
			x = 0;
		}

		if (y < 0)
		{
			index_y--;
			y = mWorld.kChunkSizeY - 1;
		}
		else if (y > mWorld.kChunkSizeY - 1)
		{
			index_y++;
			y = 0;
		}

		if (z < 0)
		{
			index_z--;
			z = mWorld.kChunkSizeZ - 1;
		}
		else if (z > mWorld.kChunkSizeZ - 1)
		{
			index_z++;
			z = 0;
		}

		auto new_index = std::make_tuple(index_x, index_y, index_z);
		if (new_index == chunkIndex)
		{
//			UE_LOG(LogTemp, Warning, TEXT("Recursive chunk: %d,%d,%d"), index_x, index_y, index_z);
			return true;
		}

		// TODO: Optimize, cache neighbor chunk data
		auto chunk_data = mWorld.GetChunkData(new_index);
		if (nullptr == chunk_data)
		{
			return true;
		}
		else
		{
			return chunk_data->IsSolid(VoxelLocalPosition(x, y, z));
		}
	}
	else
	{
		return mWorld.voxel_manager.GetType(voxels_[x][y][z]).solid;
	}
}

bool
Chunk::SetVoxel(const VoxelLocalPosition &localPos, voxel_t t)
{
	auto x = localPos.x, y = localPos.y, z = localPos.z;
	if (x < 0 || y < 0 || z < 0 || x > mWorld.kChunkSizeX - 1 || y > mWorld.kChunkSizeY - 1
		|| z > mWorld.kChunkSizeZ - 1)
		return false;
	voxels_[x][y][z] = t;
	return true;
}

void
Chunk::Save()
{
	auto[index_x, index_y, index_z] = chunkIndex;
	std::wstringstream filename;
//	filename << *Path::GetSaveDir() << "/World/first/" << index_x << "_" << index_y << "_" << index_z << ".dat";
	std::filesystem::path ps = filename.str();
//	UE_LOG(LogTemp, Warning, TEXT("save path: %s"), std::filesystem::absolute(ps).c_str());
	std::filesystem::create_directories(ps.parent_path());
	std::ofstream chunk_file(ps, std::ios::binary);
	for (int z = 0; z < mWorld.kChunkSizeZ; ++z)
	{
		for (int y = 0; y < mWorld.kChunkSizeY; ++y)
		{
			for (int x = 0; x < mWorld.kChunkSizeX; ++x)
			{
				// TODO: save
				auto type_id = voxels_[x][y][z];
//				auto type_string = voxels_[x][y][z].Type;
//				int type_id = 0;
//				if (type_string == "Grass")
//				{
//					type_id = 1;
//				}
//				else if (type_string == "Dirt")
//				{
//					type_id = 2;
//				}
//				else if (type_string == "Stone")
//				{
//					type_id = 3;
//				}
				chunk_file.write(reinterpret_cast<char*>(&type_id), sizeof(type_id));
			}
		}
	}
	chunk_file.close();
}

Chunk*
Chunk::Load(World& world, ChunkIndex index)
{
	auto[index_x, index_y, index_z] = index;
	std::wstringstream filename;
//	filename << *Path::GetSaveDir() << "/World/first/" << index_x << "_" << index_y << "_" << index_z << ".dat";
	std::filesystem::path ps = filename.str();
//	UE_LOG(LogTemp, Warning, TEXT("load path: %s"), std::filesystem::absolute(ps).c_str());
	std::ifstream chunk_file(ps, std::ios::binary);
	auto chunk = new Chunk(world, index);
	for (int z = 0; z < world.kChunkSizeZ; ++z)
	{
		for (int y = 0; y < world.kChunkSizeY; ++y)
		{
			for (int x = 0; x < world.kChunkSizeX; ++x)
			{
				auto& block = chunk->voxels_[x][y][z];
				int type_id = 0;
				chunk_file.read((char*)&type_id, sizeof(type_id));
				block = type_id;
				// TODO: load
//				if (type_id == 0)
//				{
//					block.Type = "Air";
//					block.IsSolid = false;
//				}
//				else if (type_id == 1)
//				{
//					block.Type = "Grass";
//					block.IsSolid = true;
//				}
//				else if (type_id == 2)
//				{
//					block.Type = "Dirt";
//					block.IsSolid = true;
//				}
//				else if (type_id == 3)
//				{
//					block.Type = "Stone";
//					block.IsSolid = true;
//				}
			}
		}
	}
	chunk_file.close();
	return chunk;
}

bool
Chunk::IsStored(ChunkIndex index)
{
	auto[index_x, index_y, index_z] = index;
	std::wstringstream filename;
//	filename << *Path::GetSaveDir() << "/World/first/" << index_x << "_" << index_y << "_" << index_z << ".dat";
	std::filesystem::path ps = filename.str();

	return std::filesystem::exists(ps);
}

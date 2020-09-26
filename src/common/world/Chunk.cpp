#include "Chunk.h"

#include <sstream>
#include <fstream>
#include <filesystem>

#include "common/NoiseTool.h"

#include "WorldUtils.h"

//#include "Common/Path.h"

Chunk::Chunk(World& world)
	: mWorld(world)
{

}

Chunk::~Chunk()
{
}

//bool
//Chunk::SetVoxel(const VoxelIndex &voxelIndex, voxel_t t)
//{
//	if (WorldUtils::IsVoxelOutOfChunkBounds(voxelIndex))
//		return false;
//	voxels_[voxelIndex.x][voxelIndex.y][voxelIndex.z] = t;
//	return true;
//}

void
Chunk::Save()
{
//	auto[index_x, index_y, index_z] = chunkIndex;
//	std::wstringstream filename;
////	filename << *Path::GetSaveDir() << "/World/first/" << index_x << "_" << index_y << "_" << index_z << ".dat";
//	std::filesystem::path ps = filename.str();
////	UE_LOG(LogTemp, Warning, TEXT("save path: %s"), std::filesystem::absolute(ps).c_str());
//	std::filesystem::create_directories(ps.parent_path());
//	std::ofstream chunk_file(ps, std::ios::binary);
//	for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z)
//	{
//		for (int y = 0; y < WorldConfig::kChunkSizeY; ++y)
//		{
//			for (int x = 0; x < WorldConfig::kChunkSizeX; ++x)
//			{
//				// TODO: save
//				auto type_id = voxels_[x][y][z];
////				auto type_string = voxels_[x][y][z].Type;
////				int type_id = 0;
////				if (type_string == "Grass")
////				{
////					type_id = 1;
////				}
////				else if (type_string == "Dirt")
////				{
////					type_id = 2;
////				}
////				else if (type_string == "Stone")
////				{
////					type_id = 3;
////				}
//				chunk_file.write(reinterpret_cast<char*>(&type_id), sizeof(type_id));
//			}
//		}
//	}
//	chunk_file.close();
}

Chunk*
Chunk::Load(World& world, ChunkIndex index)
{
//	auto[index_x, index_y, index_z] = index;
//	std::wstringstream filename;
////	filename << *Path::GetSaveDir() << "/World/first/" << index_x << "_" << index_y << "_" << index_z << ".dat";
//	std::filesystem::path ps = filename.str();
////	UE_LOG(LogTemp, Warning, TEXT("load path: %s"), std::filesystem::absolute(ps).c_str());
//	std::ifstream chunk_file(ps, std::ios::binary);
//	auto chunk = new Chunk(world, index);
//	for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z)
//	{
//		for (int y = 0; y < WorldConfig::kChunkSizeY; ++y)
//		{
//			for (int x = 0; x < WorldConfig::kChunkSizeX; ++x)
//			{
//				auto& block = chunk->voxels_[x][y][z];
//				int type_id = 0;
//				chunk_file.read((char*)&type_id, sizeof(type_id));
//				block = type_id;
//				// TODO: load
////				if (type_id == 0)
////				{
////					block.Type = "Air";
////					block.IsSolid = false;
////				}
////				else if (type_id == 1)
////				{
////					block.Type = "Grass";
////					block.IsSolid = true;
////				}
////				else if (type_id == 2)
////				{
////					block.Type = "Dirt";
////					block.IsSolid = true;
////				}
////				else if (type_id == 3)
////				{
////					block.Type = "Stone";
////					block.IsSolid = true;
////				}
//			}
//		}
//	}
//	chunk_file.close();
//	return chunk;

return nullptr;
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

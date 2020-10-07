//
// Created by vsensu on 2020/9/26.
//

#ifndef LEARNCRAFT_SRC_COMMON_WORLD_WORLDUTILS_H
#define LEARNCRAFT_SRC_COMMON_WORLD_WORLDUTILS_H

#include <array>

#include "WorldTypes.h"
#include "WorldConfig.h"
#include "common/Camera.h"

class WorldUtils
{
public:
	[[nodiscard]] static inline bool IsVoxelOutOfChunkBounds(const VoxelIndex &voxelIndex)
	{
		return
		voxelIndex.x < 0 || voxelIndex.x >= WorldConfig::kChunkSizeX ||
				voxelIndex.y < 0 || voxelIndex.y >= WorldConfig::kChunkSizeY ||
					voxelIndex.z < 0 || voxelIndex.z >= WorldConfig::kChunkSizeZ;
	}

	static ChunkIndex
	GetChunkIndexViaLocation(float x, float y, float z);
	static VoxelIndex
	GlobalLocationToVoxelIndex(float x, float y, float z);
	static Position
	VoxelIndexToGlobalLocation(const VoxelIndex &voxelIndex);

	static inline Position ChunkIndexToPosition(const ChunkIndex &chunkIndex)
	{
		auto [chunk_index_x, chunk_index_y, chunk_index_z] = chunkIndex;
		return Position{chunk_index_x * WorldConfig::kChunkSizeX, chunk_index_y * WorldConfig::kChunkSizeY, chunk_index_z * WorldConfig::kChunkSizeZ};
	}

	static inline std::size_t voxel_index_to_data_index(const VoxelIndex &voxelIndex)
	{
//		return voxelIndex.x * (WorldConfig::kChunkSizeY * WorldConfig::kChunkSizeZ) + voxelIndex.z * WorldConfig::kChunkSizeY + voxelIndex.y;
        return voxelIndex.y * (WorldConfig::kChunkSizeX * WorldConfig::kChunkSizeZ) + voxelIndex.z * WorldConfig::kChunkSizeX + voxelIndex.x;
	}

	static inline voxel_t make_air_voxel()
	{
		return static_cast<voxel_t>(CommonVoxel::Air);
	}

	static bool IsPointInSightRange(const Position &cameraPos, const Position &pos, float sightRange);
	static bool chunkIsInFrustum(const ViewFrustum &frustum, const Position &chunkPosition) noexcept;

	static inline bool IsBorder(const VoxelIndex &index)
    {
	    if(index.x == 0 || index.x + 1 == WorldConfig::kChunkSizeX || index.y == 0 || index.y + 1 == WorldConfig::kChunkSizeY || index.z == 0 || index.z + 1 == WorldConfig::kChunkSizeZ)
	        return true;

	    return false;
    }

    std::tuple<ChunkIndex, VoxelIndex> StandardizeVoxelIndex(const ChunkIndex& chunkIndex, UnboundVoxelIndex unboundIndex)
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
                terminate();
//			UE_LOG(LogTemp, Warning, TEXT("Recursive chunk: %d,%d,%d"), index_x, index_y, index_z);
            }
            auto new_voxel_index = VoxelIndex(x, y, z);
            return std::make_tuple(new_chunk_index, new_voxel_index);
        }
        else
        {
            return std::make_tuple(chunkIndex, unboundIndex);
        }
    }
};

#endif //LEARNCRAFT_SRC_COMMON_WORLD_WORLDUTILS_H

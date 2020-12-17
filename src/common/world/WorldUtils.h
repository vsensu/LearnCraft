//
// Created by vsensu on 2020/9/26.
//

#ifndef LEARNCRAFT_SRC_COMMON_WORLD_WORLDUTILS_H
#define LEARNCRAFT_SRC_COMMON_WORLD_WORLDUTILS_H

#include <array>
#include <vector>
#include <optional>

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
    GetChunkIndexViaLocation(const Position &pos)
    {
        int index_x = static_cast<int>(std::floor(pos.x / WorldConfig::kChunkSizeX));
        int index_y = static_cast<int>(std::floor(pos.y / WorldConfig::kChunkSizeY));
        int index_z = static_cast<int>(std::floor(pos.z / WorldConfig::kChunkSizeZ));
        return std::make_tuple(index_x, index_y, index_z);
    }

	static ChunkIndex
	GetChunkIndexViaLocation(float x, float y, float z);
	static VoxelIndex
	GlobalLocationToVoxelIndex(float x, float y, float z);
	static Position
	VoxelIndexToGlobalLocation(const VoxelIndex &voxelIndex);
    static std::tuple<ChunkIndex, VoxelIndex>
    GlobalLocationToVoxel(const VoxelPosition &pos);

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

    static Position toVoxelPosition(const glm::vec3& vec)
    {
        auto x = static_cast<i32>(std::floor(vec.x));
        auto y = static_cast<i32>(std::floor(vec.y));
        auto z = static_cast<i32>(std::floor(vec.z));
        return {x, y, z};
    }

    // uses fast voxel traversal to get the voxels intersected by a ray of length `range`
    // assumes voxels are 1x1x1
    static std::vector<VoxelPosition> getIntersectedVoxels(const glm::vec3& startPoint,
                                                    const glm::vec3& direction, float range)
    {
        // Ensures passed direction is normalized
        auto nDirection = glm::normalize(direction);
        auto endPoint = startPoint + nDirection * range;
        auto startVoxel = toVoxelPosition(startPoint);

        // +1, -1, or 0
        i32 stepX = (nDirection.x > 0) ? 1 : ((nDirection.x < 0) ? -1 : 0);
        i32 stepY = (nDirection.y > 0) ? 1 : ((nDirection.y < 0) ? -1 : 0);
        i32 stepZ = (nDirection.z > 0) ? 1 : ((nDirection.z < 0) ? -1 : 0);

        float tDeltaX =
                (stepX != 0) ? fmin(stepX / (endPoint.x - startPoint.x), FLT_MAX) : FLT_MAX;
        float tDeltaY =
                (stepY != 0) ? fmin(stepY / (endPoint.y - startPoint.y), FLT_MAX) : FLT_MAX;
        float tDeltaZ =
                (stepZ != 0) ? fmin(stepZ / (endPoint.z - startPoint.z), FLT_MAX) : FLT_MAX;

        float tMaxX = (stepX > 0.0f) ? tDeltaX * (1.0f - startPoint.x + startVoxel.x)
                                     : tDeltaX * (startPoint.x - startVoxel.x);
        float tMaxY = (stepY > 0.0f) ? tDeltaY * (1.0f - startPoint.y + startVoxel.y)
                                     : tDeltaY * (startPoint.y - startVoxel.y);
        float tMaxZ = (stepZ > 0.0f) ? tDeltaZ * (1.0f - startPoint.z + startVoxel.z)
                                     : tDeltaZ * (startPoint.z - startVoxel.z);

        auto currentVoxel = startVoxel;
        std::vector<VoxelPosition> intersected;
        intersected.push_back(startVoxel);

        // sanity check to prevent leak
        while (intersected.size() < range * 3) {
            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    currentVoxel.x += stepX;
                    tMaxX += tDeltaX;
                }
                else {
                    currentVoxel.z += stepZ;
                    tMaxZ += tDeltaZ;
                }
            }
            else {
                if (tMaxY < tMaxZ) {
                    currentVoxel.y += stepY;
                    tMaxY += tDeltaY;
                }
                else {
                    currentVoxel.z += stepZ;
                    tMaxZ += tDeltaZ;
                }
            }
            if (tMaxX > 1 && tMaxY > 1 && tMaxZ > 1)
                break;
            intersected.push_back(currentVoxel);
        }
        return intersected;
    }

    static glm::vec3 forwardsVector(const glm::vec3& rotation)
    {
        float yaw = glm::radians(rotation.y + 90);
        float pitch = glm::radians(rotation.x);
        float x = glm::cos(yaw) * glm::cos(pitch);
        float y = glm::sin(pitch);
        float z = glm::cos(pitch) * glm::sin(yaw);

        return {-x, -y, -z};
    }
};

#endif //LEARNCRAFT_SRC_COMMON_WORLD_WORLDUTILS_H

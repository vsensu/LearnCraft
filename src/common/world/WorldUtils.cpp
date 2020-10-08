//
// Created by vsensu on 2020/9/26.
//

#include "WorldUtils.h"

ChunkIndex
WorldUtils::GetChunkIndexViaLocation(float x, float y, float z)
{
	int index_x = static_cast<int>(std::floor(x / WorldConfig::kChunkSizeX));
	int index_y = static_cast<int>(std::floor(y / WorldConfig::kChunkSizeY));
	int index_z = static_cast<int>(std::floor(z / WorldConfig::kChunkSizeZ));
	return std::make_tuple(index_x, index_y, index_z);
}

VoxelIndex
WorldUtils::GlobalLocationToVoxelIndex(float x, float y, float z)
{
	auto[chunk_index_x, chunk_index_y, chunk_index_z] = GetChunkIndexViaLocation(x, y, z);
	int block_x = static_cast<int>(std::floor(x - chunk_index_x * WorldConfig::kChunkSizeX));
	int block_y = static_cast<int>(std::floor(y - chunk_index_y * WorldConfig::kChunkSizeY));
	int block_z = static_cast<int>(std::floor(z - chunk_index_z * WorldConfig::kChunkSizeZ));
	return VoxelIndex(block_x, block_y, block_z);
}

std::tuple<ChunkIndex, VoxelIndex>
WorldUtils::GlobalLocationToVoxel(const VoxelPosition &pos)
{
    auto chunkIndex = GetChunkIndexViaLocation(pos.x, pos.y, pos.z);
    auto[chunk_index_x, chunk_index_y, chunk_index_z] = chunkIndex;
    int block_x = static_cast<int>(std::floor(pos.x - chunk_index_x * WorldConfig::kChunkSizeX));
    int block_y = static_cast<int>(std::floor(pos.y - chunk_index_y * WorldConfig::kChunkSizeY));
    int block_z = static_cast<int>(std::floor(pos.z - chunk_index_z * WorldConfig::kChunkSizeZ));
    return std::make_tuple(chunkIndex, VoxelIndex(block_x, block_y, block_z));
}

Position
WorldUtils::VoxelIndexToGlobalLocation(const VoxelIndex& voxelIndex)
{
	return Position{};
}

bool
WorldUtils::IsPointInSightRange(const Position& cameraPos, const Position &pos, float sightRange)
{
	return std::pow(cameraPos.x - pos.x, 2) +
		std::pow(cameraPos.y - pos.y, 2) +
		std::pow(cameraPos.z - pos.z, 2) <=
		std::pow(sightRange, 2);
}

bool WorldUtils::chunkIsInFrustum(const ViewFrustum &frustum, const Position &chunkPosition) noexcept
{
//	box *= CHUNK_SIZE;
	auto box = chunkPosition;

	auto getVP = [&](const glm::vec3& normal) {
		auto res = box;

		if (normal.x > 0) {
			res.x += WorldConfig::kChunkSizeX;
		}
		if (normal.y > 0) {
			res.y += WorldConfig::kChunkSizeY;
		}
		if (normal.z > 0) {
			res.z += WorldConfig::kChunkSizeZ;
		}

		return glm::vec3{res.x, res.y, res.z};
	};

	auto getVN = [&](const glm::vec3& normal) {
		auto res = box;

		if (normal.x < 0) {
			res.x += WorldConfig::kChunkSizeX;
		}
		if (normal.y < 0) {
			res.y += WorldConfig::kChunkSizeY;
		}
		if (normal.z < 0) {
			res.z += WorldConfig::kChunkSizeZ;
		}

		return glm::vec3{res.x, res.y, res.z};
	};

	bool result = true;
	for (auto& plane : frustum.m_planes) {
		if (plane.distanceToPoint(getVP(plane.normal)) < 0) {
			return false;
		}
		else if (plane.distanceToPoint(getVN(plane.normal)) < 0) {
			result = true;
		}
	}
	return result;
}

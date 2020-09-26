//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_SRC_COMMON_WORLD_WORLDTYPES_H
#define LEARNCRAFT_SRC_COMMON_WORLD_WORLDTYPES_H

#include <string>
#include <tuple>
#include <glm/glm.hpp>

#include "common/types.h"

using BlockType = std::string;
//using ChunkIndex = glm::i32vec3;
using ChunkIndex = std::tuple<i32, i32, i32>;
using VoxelIndex = glm::u8vec3;
using Position = glm::vec3;
using voxel_t = u16;

enum class CubeSide : u8
{
	Top,
	Bottom,
	Left,
	Right,
	Front,
	Back
};

enum class CommonVoxel : voxel_t
{
	Air = 0,
	Dirt,
	Grass,
	Stone,
};

#endif //LEARNCRAFT_SRC_COMMON_WORLD_WORLDTYPES_H

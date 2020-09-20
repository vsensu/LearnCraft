//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_SRC_COMMON_WORLD_VOXELMANAGER_H
#define LEARNCRAFT_SRC_COMMON_WORLD_VOXELMANAGER_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <any>

#include "common/TypeManager.h"
#include "WorldTypes.h"

using name_t = std::string_view;

struct VoxelData
{
	name_t name;
	bool solid {true};
	std::any metadata;
};

class VoxelManager : public TypeManager<voxel_t, VoxelData>
{
};


#endif //LEARNCRAFT_SRC_COMMON_WORLD_VOXELMANAGER_H

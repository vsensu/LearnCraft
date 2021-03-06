//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_SRC_COMMON_WORLD_VOXELMANAGER_H
#define LEARNCRAFT_SRC_COMMON_WORLD_VOXELMANAGER_H

#include <any>

#include "WorldTypes.h"
#include "common/TypeManager.h"
#include "common/world/TextureManager.h"

using name_t = std::string_view;

struct VoxelTypeData
{
	name_t name;
	bool opaque {true};
    VoxelTextureNames textures;
	std::any metadata;
};

class VoxelManager : public TypeManager<voxel_t, VoxelTypeData>
{
};


#endif //LEARNCRAFT_SRC_COMMON_WORLD_VOXELMANAGER_H

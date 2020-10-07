//
// Created by reili on 2020/10/7.
//

#ifndef LEARNCRAFT_VOXELDATACOMPONENT_H
#define LEARNCRAFT_VOXELDATACOMPONENT_H

#include <common/world/WorldTypes.h>

using voxel_traits_t = u32;

struct VoxelDataComponent
{
    std::string name;
    voxel_traits_t traits;
};

enum VoxelTrait: voxel_traits_t
{
    Empty = 1,
    Opaque = 1u << 1,
};

bool has_trait(voxel_traits_t traits, VoxelTrait trait)
{
    return traits & trait;
}

bool has_trait_empty(voxel_traits_t traits)
{
    return traits & VoxelTrait::Empty;
}

bool has_trait_opaque(voxel_traits_t traits)
{
    return traits & (VoxelTrait::Opaque);
}

#endif //LEARNCRAFT_VOXELDATACOMPONENT_H

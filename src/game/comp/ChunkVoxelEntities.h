//
// Created by reili on 2020/10/7.
//

#ifndef LEARNCRAFT_CHUNKVOXELENTITIES_H
#define LEARNCRAFT_CHUNKVOXELENTITIES_H

#include <array>

#include <entt/entt.hpp>
#include <common/world/WorldConfig.h>

struct ChunkVoxelEntitiesComponent
{
    std::array<entt::entity , WorldConfig::ChunkSize()> voxels;
};

#endif //LEARNCRAFT_CHUNKVOXELENTITIES_H

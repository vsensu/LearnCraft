//
// Created by reili on 2020/10/7.
//

#ifndef LEARNCRAFT_CHUNKVOXELENTITIESCOMPONENT_H
#define LEARNCRAFT_CHUNKVOXELENTITIESCOMPONENT_H

#include <array>

#include <entt/entt.hpp>
#include <common/world/WorldConfig.h>

struct ChunkVoxelEntitiesComponent
{
    std::array<entt::entity , WorldConfig::ChunkSize()> voxels;
};

#endif //LEARNCRAFT_CHUNKVOXELENTITIESCOMPONENT_H

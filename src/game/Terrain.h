//
// Created by reili on 2020/10/7.
//

#ifndef LEARNCRAFT_TERRAIN_H
#define LEARNCRAFT_TERRAIN_H

#include <entt/entt.hpp>
#include <common/world/WorldTypes.h>
#include <common/world/WorldConfig.h>
#include <common/NoiseTool.h>
#include <common/world/WorldUtils.h>

struct VoxelEntityGenerator
{
    auto operator()(const Position &chunkPosition)
    {
        std::array<entt::entity, WorldConfig::ChunkSize()> voxels;
        const int dirt_base = 60;
        for(int x = 0; x < WorldConfig::kChunkSizeX; ++x)
        {
            for(int z = 0; z < WorldConfig::kChunkSizeZ; ++z)
            {
                auto horizontal = NoiseTool::GenerateHeightWithCache(chunkPosition.x + x, chunkPosition.z + z);
                for(int y = 0; y < WorldConfig::kChunkSizeY; ++y)
                {
                    entt::entity voxel;
                    int global_height = chunkPosition.y + y;

                    if (global_height > horizontal)
                    {
                        voxel = CoreEntity::Block_Empty;
//                        voxel = named_entities[NamedEntities::Block_Empty];
                    }
                    else if(WorldUtils::IsBorder(VoxelIndex {x,y,z}))
                    {
                        voxel = CoreEntity::Debug_Border;
                    }
                    else if (global_height == horizontal)
                    {
                        voxel = CoreEntity::Block_Grass;
                    }
                    else if (global_height >= dirt_base)
                    {
                        voxel = CoreEntity::Block_Dirt;
                    }
                    else
                    {
                        voxel = CoreEntity::Block_Stone;
                    }
                    voxels[WorldUtils::voxel_index_to_data_index(VoxelIndex{x, y, z})] = voxel;
                }
            }
        }
        return voxels;
    }
};

#endif //LEARNCRAFT_TERRAIN_H

//
// Created by reili on 2020/9/27.
//

#ifndef LEARNCRAFT_CHUNGENERATOR_H
#define LEARNCRAFT_CHUNGENERATOR_H

#include "common/NoiseTool.h"
#include "WorldTypes.h"
#include "Chunk.h"

struct DefaultGenerator
{
    void CreateChunkData(const Position &chunkPosition, Chunk &chunk)
    {
        const int dirt_base = 60;
        for(int x = 0; x < WorldConfig::kChunkSizeX; ++x)
        {
            for(int z = 0; z < WorldConfig::kChunkSizeZ; ++z)
            {
                auto horizontal = NoiseTool::GenerateHeightWithCache(chunkPosition.x + x, chunkPosition.z + z);
                for(int y = 0; y < WorldConfig::kChunkSizeY; ++y)
                {
                    voxel_t voxel;
                    int global_height = chunkPosition.y + y;
                    if (global_height > horizontal)
                    {
                        voxel = static_cast<voxel_t>(CommonVoxel::Air);
                    }
                    else if (global_height == horizontal)
                    {
                        voxel = static_cast<voxel_t>(CommonVoxel::Grass);
                    }
                    else if (global_height >= dirt_base)
                    {
                        voxel = static_cast<voxel_t>(CommonVoxel::Dirt);
                    }
                    else
                    {
                        voxel = static_cast<voxel_t>(CommonVoxel::Stone);
                    }
                    chunk.SetVoxel(VoxelIndex{x, y, z}, voxel);
                }
            }
        }
    }
};

#endif //LEARNCRAFT_CHUNGENERATOR_H

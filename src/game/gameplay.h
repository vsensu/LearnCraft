//
// Created by reili on 2020/9/30.
//

#ifndef LEARNCRAFT_GAMEPLAY_H
#define LEARNCRAFT_GAMEPLAY_H

#include <entt/entt.hpp>
#include <glad/glad.h>
#include <functional>
#include <array>

#include "common/world/WorldUtils.h"
#include "common/world/WorldTypes.h"

#include "common/Shader.hpp"

/*
 * define ENTT_ID_TYPE std::uint32_t
 * ===================================
 * Config(Type) is comfirmed instance.
 * ===================================
 *
 * ============================
 * tables
 * ============================
 *
 * Entity Table
 * ------------------------------------------------
 * id   |   component_inst_ids(vector)
 * ------------------------------------------------
 *
 * Component Table
 * --------------------------------------------------------------
 * component_inst_id    |   class_reflection  |   serialized_data(str)
 * --------------------------------------------------------------
 * default_1
 * default_2
 * ...
 * inst_1
 * inst_2
 *
 * ===========================
 * Generated Data
 * ===========================
 * Chunk Mesh
 * Chunk to Create
 * */

struct ChunkInitComponent
{
    ChunkIndex chunk_index;
};

struct ChunkIndexComponent
{
    ChunkIndex val;
};

struct ChunkComponent
{
    std::array<voxel_t , WorldConfig::ChunkSize()> voxels;
};

struct ChunkInitSystem{
    static void Tick(std::function<std::array<voxel_t, WorldConfig::ChunkSize()>(const Position &)> generator,
                      entt::registry &registry) {
        auto view = registry.view<ChunkInitComponent>();
        for (auto entity : view) {
            auto &chunk_init = view.get<ChunkInitComponent>(entity);

            registry.emplace<ChunkIndexComponent>(entity, chunk_init.chunk_index);
            registry.emplace<ChunkComponent>(entity, generator(WorldUtils::ChunkIndexToPosition(chunk_init.chunk_index)));
            registry.remove<ChunkInitComponent>(entity);
        }
    }
};


#endif //LEARNCRAFT_GAMEPLAY_H

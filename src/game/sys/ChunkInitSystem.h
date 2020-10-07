//
// Created by reili on 2020/10/7.
//

#ifndef LEARNCRAFT_CHUNKINITSYSTEM_H
#define LEARNCRAFT_CHUNKINITSYSTEM_H

#include <functional>

#include <common/world/WorldTypes.h>
#include <entt/entt.hpp>
#include <common/world/WorldConfig.h>
#include <game/comp/chunk_index.h>
#include <game/comp/ChunkVoxelEntities.h>
#include <common/world/WorldUtils.h>
#include <game/comp/ChunkInit.h>
#include <game/comp/ChunkMeshInit.h>


struct ChunkInitSystem{
    static void Tick(std::unordered_map<ChunkIndex, entt::entity, hash_tuple> &chunk_entity_map,
            std::function<std::array<entt::entity, WorldConfig::ChunkSize()> (const Position &)> generator,
                     entt::registry &registry) {
        auto view = registry.view<ChunkInitComponent>();
        for (auto entity : view) {
            auto &chunk_init = view.get<ChunkInitComponent>(entity);

            registry.emplace<ChunkIndexComponent>(entity, chunk_init.chunk_index);
            registry.emplace<ChunkVoxelEntitiesComponent>(entity, ChunkVoxelEntitiesComponent{.voxels = generator(
                    WorldUtils::ChunkIndexToPosition(chunk_init.chunk_index))});
            chunk_entity_map[chunk_init.chunk_index] = entity;
            registry.emplace<ChunkMeshInitComponent>(entity);
        }

        auto remove_view = registry.view<ChunkInitComponent>();
        for(auto entity : remove_view)
        {
            registry.remove<ChunkInitComponent>(entity);
        }
    }
};

#endif //LEARNCRAFT_CHUNKINITSYSTEM_H

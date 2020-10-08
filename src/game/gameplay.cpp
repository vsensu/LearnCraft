//
// Created by reili on 2020/10/7.
//

#include "gameplay.h"

#include <game/sys/ChunkRenderSystem.h>
#include <game/comp/VoxelDataComponent.h>
#include <game/comp/RuntimeVoxelTextureLayerComponent.h>
#include <game/sys/ChunkInitSystem.h>
#include <game/sys/ChunkMeshInitSystem.h>
#include <imgui.h>

#include "entities.h"
#include "Terrain.h"

void Game::Init()
{
    // load textures
    texture_manager.RegisterTexture("Empty", "Textures/dirt.jpg");
    texture_manager.RegisterTexture("Dirt", "Textures/dirt.jpg");
    texture_manager.RegisterTexture("Grass.Top", "Textures/grass.jpg");
    texture_manager.RegisterTexture("Stone", "Textures/stone.jpg");
    GLuint texture = texture_manager.CreateTexture();
    ChunkRenderSystem::init(texture);

    // create core entities
    auto empty = registry.create(CoreEntity::Block_Empty);
    auto dirt = registry.create(CoreEntity::Block_Dirt);
    auto grass = registry.create(CoreEntity::Block_Grass);
    auto stone = registry.create(CoreEntity::Block_Stone);

    named_entities[NamedEntities::Block_Empty] = empty;
    named_entities[NamedEntities::Block_Dirt] = dirt;
    named_entities[NamedEntities::Block_Grass] = grass;
    named_entities[NamedEntities::Block_Stone] = stone;

    // set voxel traits
    registry.emplace<VoxelDataComponent>(empty, "Empty", static_cast<voxel_traits_t>(VoxelTrait::Empty));
    registry.emplace<VoxelDataComponent>(dirt, "Dirt", static_cast<voxel_traits_t>(VoxelTrait::Opaque));
    registry.emplace<VoxelDataComponent>(grass, "Grass", static_cast<voxel_traits_t>(VoxelTrait::Opaque));
    registry.emplace<VoxelDataComponent>(stone, "Stone", static_cast<voxel_traits_t>(VoxelTrait::Opaque));

    // Convert config to runtime
    registry.emplace<RuntimeVoxelTextureLayerComponent>(empty, VoxelTextureLayers{
            .front = 0, .back = 0,
            .left = 0, .right = 0,
            .top = 0, .bottom = 0,
    });
    auto dirt_layer = texture_manager.GetVoxelTextureLayer("Dirt");
    registry.emplace<RuntimeVoxelTextureLayerComponent>(dirt, VoxelTextureLayers{
            .front = dirt_layer, .back = dirt_layer,
            .left = dirt_layer, .right = dirt_layer,
            .top = dirt_layer, .bottom = dirt_layer,
    });
    registry.emplace<RuntimeVoxelTextureLayerComponent>(grass, VoxelTextureLayers{
            .front = dirt_layer, .back = dirt_layer,
            .left = dirt_layer, .right = dirt_layer,
            .top = texture_manager.GetVoxelTextureLayer("Grass.Top"),
            .bottom = dirt_layer,
    });
    auto stone_layer = texture_manager.GetVoxelTextureLayer("Stone");
    registry.emplace<RuntimeVoxelTextureLayerComponent>(stone, VoxelTextureLayers{
            .front = stone_layer, .back = stone_layer,
            .left = stone_layer, .right = stone_layer,
            .top = stone_layer, .bottom = stone_layer,
    });

    // generate terrain
    NoiseTool::GenerateHeightCache(-50, -50, 50, 50);
    int center_x = 0, center_z = 0;
    int initSize = 5;
    int chunk_vertical = 16;

    for (int x = center_x - initSize; x < center_x + initSize; ++x) {
        for (int z = center_z - initSize; z < center_z + initSize; ++z) {
            for (int y = 0; y < chunk_vertical; ++y) {
                ChunkIndex chunk_index{x, y, z};
                auto entity = registry.create();
                registry.emplace<ChunkInitComponent>(entity, ChunkInitComponent{.chunk_index = chunk_index});
            }
        }
    }

    // get player spawn location
    auto height = NoiseTool::GenerateHeightWithCache(0, 0);

    // create player
    player = registry.create();
    registry.emplace<PositionComponent>(player, PositionComponent{.val = Position{0, height, 0}});
}

void Game::FixedTick(double fixedDeltaTime)
{

}

void Game::Tick(double deltaTime)
{
    VoxelEntityGenerator generator;
    ChunkInitSystem::Tick(chunk_entity_map, generator, registry);
    ChunkMeshInitSystem::Tick(registry, chunk_entity_map);
}

void Game::RenderScene(Camera &camera)
{
    auto &pos = registry.get<PositionComponent>(player);
    pos.val = camera.GetPos();

    auto last_chunk_index = WorldUtils::GetChunkIndexViaLocation(player_last_pos);
    auto curr_chunk_index = WorldUtils::GetChunkIndexViaLocation(pos.val);
    if(last_chunk_index != curr_chunk_index && chunk_entity_map.find(curr_chunk_index) == chunk_entity_map.end())
    {
        auto entity = registry.create();
        std::cout << "create new chunk " << static_cast<int>(entity) << "\n";
        registry.emplace<ChunkInitComponent>(entity, ChunkInitComponent{.chunk_index=curr_chunk_index});
    }

    ChunkRenderSystem::Tick(camera, registry, 80.f);
    player_last_pos = pos.val;
}

void Game::RenderUI()
{
    ImGui::Begin("Game");
    ImGui::Text("draw vertex:%lu", ChunkRenderSystem::vertex_draw_count);
    ImGui::End();
}

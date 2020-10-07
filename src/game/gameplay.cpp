//
// Created by reili on 2020/10/7.
//

#include "gameplay.h"

#include <game/sys/chunk_render.h>
#include <game/comp/VoxelData.h>
#include <game/comp/RuntimeVoxelTextureLayer.h>
#include <game/sys/ChunkInitSystem.h>
#include <game/sys/ChunkMeshInitSystem.h>
#include <imgui.h>

#include "entities.h"
#include "Terrain.h"

void Game::Init()
{
    texture_manager.RegisterTexture("Empty", "Textures/dirt.jpg");
    texture_manager.RegisterTexture("Dirt", "Textures/dirt.jpg");
    texture_manager.RegisterTexture("Grass.Top", "Textures/grass.jpg");
    texture_manager.RegisterTexture("Stone", "Textures/stone.jpg");

    GLuint texture = texture_manager.CreateTexture();
    ChunkRenderSystem::init(texture);

    auto empty = registry.create(CoreEntity::Block_Empty);
    auto dirt = registry.create(CoreEntity::Block_Dirt);
    auto grass = registry.create(CoreEntity::Block_Grass);
    auto stone = registry.create(CoreEntity::Block_Stone);

    named_entities[NamedEntities::Block_Empty] = empty;
    named_entities[NamedEntities::Block_Dirt] = dirt;
    named_entities[NamedEntities::Block_Grass] = grass;
    named_entities[NamedEntities::Block_Stone] = stone;

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

    auto height = NoiseTool::GenerateHeightWithCache(0, 0);
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
    ChunkRenderSystem::Tick(camera, registry);
}

void Game::RenderUI()
{
    ImGui::Begin("Game");
    ImGui::Text("draw vertex:%lu", ChunkRenderSystem::vertex_draw_count);
    ImGui::End();
}

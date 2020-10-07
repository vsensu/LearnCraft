//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_GAMEAPPLICATION_H
#define LEARNCRAFT_GAMEAPPLICATION_H

#include <memory>
#include <vector>
#include <array>
#include <future>
#include <entt/entt.hpp>
#include <game/sys/chunk_render.h>

#include "common/Application.h"
#include "common/world/World.h"
#include "common/Camera.h"
#include "common/Shader.hpp"
#include "common/world/ChunkRender.h"
//#include "gameplay.h"

class GameApplication : public Application
{
public:
	using Application::Application;
private:
	void Init() override;
    void Update() override;
    void RenderScene() override;
    void RenderUI() override;
    void HandleKeyboard(GLFWwindow *window) override;

    std::shared_ptr<World> world_ { std::make_shared<World>() };
    Camera camera_;
    std::vector<ChunkMesh*> meshes;
    std::size_t vertex_count;
    std::size_t index_count;
    std::unordered_map<ChunkIndex, ChunkBuffGL*, hash_tuple> chunk_buffs;
    bool world_loaded_ {false};
    std::unordered_map<ChunkIndex, ChunkMesh*, hash_tuple> chunk_meshes_update_;

    std::future<void> init_world_;
    entt::registry registry;
    DefaultGenerator generator;
};

#endif //LEARNCRAFT_GAMEAPPLICATION_H

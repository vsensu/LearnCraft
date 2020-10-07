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
#include "gameplay.h"
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

    Camera camera_;
    std::size_t vertex_count;
    std::size_t index_count;
    bool world_loaded_ {false};

    std::future<void> init_world_;

    Game game;
};

#endif //LEARNCRAFT_GAMEAPPLICATION_H

//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_GAMEAPPLICATION_H
#define LEARNCRAFT_GAMEAPPLICATION_H

#include <memory>
#include <vector>
#include <array>

#include "common/Application.h"
#include "common/world/World.h"
#include "common/Camera.h"
#include "common/Shader.hpp"
#include "common/world/ChunkRender.h"

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
    std::shared_ptr<Shader<CreateShaderProgramFromString>> shader {nullptr};
    std::unordered_map<ChunkIndex, ChunkBuffGL*, hash_tuple> chunk_buffs;
    u32 vertex_draw_count_ { 0 };
};

#endif //LEARNCRAFT_GAMEAPPLICATION_H

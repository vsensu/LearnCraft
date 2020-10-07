//
// Created by vsensu on 2020/9/20.
//

#include "GameApplication.h"

#include <iostream>

#include <glm/glm.hpp>
#include <imgui.h>
#include <glad/glad.h>

#define DEBUG_GL_ERRORS

#include "common/gl_errors.h"
#include "common/world/WorldConfig.h"
#include "common/world/WorldUtils.h"
#include "common/world/Chunk.h"
#include "common/NoiseTool.h"
#include "common/world/ChunkGenerator.h"
#include "common/world/ChunkRender.h"
#include "gameplay.h"

namespace
{
    Camera *camera;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


void
GameApplication::Init()
{
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	SetFixedFPS(0); // unlimit

    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, mouse_callback);
    glfwSetScrollCallback(window_, scroll_callback);
    camera = &camera_;

    GLuint texture = world_->GetTextureManager().CreateTexture();
    chunkRenderManager.init(texture);

    auto InitWorld = [&]() {
        InitGameCore(registry);
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
        camera_.SetPos(glm::vec3{0, height + 5, 0});
        world_loaded_ = true;
    };

    InitWorld();
//    init_world_ = std::async(std::launch::async, InitWorld);
}

void GameApplication::Update()
{
	camera_.Update(delta_time_);

    VoxelEntityGenerator generator;
    ChunkInitSystem::Tick(generator, registry);
    ChunkMeshInitSystem::Tick(registry);
}


void GameApplication::RenderScene()
{
    glClear(GL_DEPTH_BUFFER_BIT);

    if(world_loaded_)
    {
        chunkRenderManager.Tick(camera_, registry);
    }
}

void GameApplication::RenderUI()
{
    ImGui::Begin("Debug");
    ImGui::Text("fps: %d", static_cast<int>(fps_ + 0.5));
    ImGui::Text("Press F1 to toggle cursor");
    if(!world_loaded_)
    {
        ImGui::Text("Loading ...");
    }
    static bool show = false;
    if(ImGui::Button("Show voxel info"))
	{
    	show = true;
	}

    if(show)
	{
    	world_->GetVoxelManager().ForEach([](voxel_t t, const VoxelTypeData &data){
			ImGui::Text(data.name.data());
//			if(t == static_cast<voxel_t>(CommonVoxel::Air))
//				try{
//					auto meta = std::any_cast<CustomData>(data.metadata);
//					ImGui::Text("hp: %d", meta.hp);
//				}
//				catch (...)
//				{
//				}
		});
	}
//    ImGui::Text("vertex: %d index: %d", mesh.vertices.size(), mesh.indices.size());

    auto pos = camera_.GetPos();
    ImGui::Text("Camera pos:%f,%f,%f", pos.x, pos.y, pos.z);
    auto forward = camera_.GetForward();
    ImGui::Text("Camera forward:%f,%f,%f", forward.x, forward.y, forward.z);
    ImGui::Text("draw vertex:%lu", chunkRenderManager.vertex_draw_count);
    ImGui::End();
}

void GameApplication::HandleKeyboard(GLFWwindow *window)
{
    static bool cursor_key_pressed = false;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_.MoveForward(delta_time_);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_.MoveBackward(delta_time_);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_.MoveLeft(delta_time_);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_.MoveRight(delta_time_);
    if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
        cursor_key_pressed = true;
    if(cursor_key_pressed && glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE)
    {
        cursor_key_pressed = false;
        static bool disable_cursor = true;
        disable_cursor = !disable_cursor;
        if(disable_cursor)
            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if(nullptr == camera)
        return;

    camera->MouseCallback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(nullptr == camera)
        return;

    camera->Zoom(yoffset);
}

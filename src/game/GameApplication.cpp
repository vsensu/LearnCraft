//
// Created by vsensu on 2020/9/20.
//

#include "GameApplication.h"

#include <iostream>

#include <imgui.h>
#include <glad/glad.h>

#define DEBUG_GL_ERRORS

#include "common/gl_errors.h"
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
    camera->Unlock();

    game.Init();

    auto height = NoiseTool::GenerateHeightWithCache(0, 0);
    camera_.SetPos(glm::vec3{0, height + 5, 0});
    world_loaded_ = true;
}

void GameApplication::Update()
{
	camera_.Update(delta_time_);

    game.Tick(delta_time_);
}


void GameApplication::RenderScene()
{
    glClear(GL_DEPTH_BUFFER_BIT);

    game.RenderScene(camera_);
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

    auto pos = camera_.GetPos();
    ImGui::Text("Camera pos:%f,%f,%f", pos.x, pos.y, pos.z);
    auto forward = camera_.GetForward();
    ImGui::Text("Camera forward:%f,%f,%f", forward.x, forward.y, forward.z);
    ImGui::End();

    game.RenderUI();
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
        if(disable_cursor){
            camera_.Unlock();
            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else{
            camera_.Lock();
            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    game.HandleKeyboard(window);
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

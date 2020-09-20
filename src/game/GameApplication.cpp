//
// Created by vsensu on 2020/9/20.
//

#include "GameApplication.h"

#include <entt/entt.hpp>

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void GameApplication::Update()
{

}


void GameApplication::RenderScene()
{

}

void GameApplication::RenderUI()
{
    ImGui::Text("fps: %d", static_cast<int>(fps_));
}

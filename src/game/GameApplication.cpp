//
// Created by vsensu on 2020/9/20.
//

#include "GameApplication.h"

#include <entt/entt.hpp>

#include <imgui.h>

void GameApplication::Update()
{

}


void GameApplication::RenderScene()
{

}

void GameApplication::RenderUI()
{
    ImGui::Text("fps: %d", static_cast<int>(fps_ + 0.5));
}

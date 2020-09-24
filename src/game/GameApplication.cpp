//
// Created by vsensu on 2020/9/20.
//

#include "GameApplication.h"

#include <iostream>

#include <entt/entt.hpp>
#include <imgui.h>


void
GameApplication::Init()
{
}

void GameApplication::Update()
{
}


void GameApplication::RenderScene()
{

}

void GameApplication::RenderUI()
{
    ImGui::Text("fps: %d", static_cast<int>(fps_ + 0.5));
    static bool show = false;
    if(ImGui::Button("Show vexel info"))
	{
    	show = true;
	}

    if(show)
	{
    	world_->voxel_manager.ForEach([](voxel_t t, const VoxelData &data){
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
}

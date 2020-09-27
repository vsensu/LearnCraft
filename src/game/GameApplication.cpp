//
// Created by vsensu on 2020/9/20.
//

#include "GameApplication.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#define DEBUG_GL_ERRORS

#include "common/gl_errors.h"
#include "common/world/WorldConfig.h"
#include "common/world/WorldUtils.h"
#include "common/world/Chunk.h"
#include "common/NoiseTool.h"
#include "common/world/ChunkGenerator.h"
#include "common/world/ChunkRender.h"

namespace
{
    Camera *camera;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

constexpr auto vs_code = R"(
#version 330 core

layout (location = 0) in uint inVertexData;
out vec3 pos;

uniform vec3 chunkPosition;

uniform mat4 projView;
uniform mat4 model;

uniform float texUnit;

out vec3 normal;
vec3 normals[6] = vec3[6](
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f),
    vec3(-1.0f, 0.0f, 0.0f),
    vec3(0.0f, -1.0f, 0.0f),
    vec3(0.0f, 0.0f, -1.0f)
);

//out vec3 objectColor;
vec3 objectColors[4] = vec3[4](
	vec3(1, 1, 1),
	vec3(1.0f, 0.5f, 0.31f),
	vec3(0, 1, 0),
	vec3(0.8, 0.8, 0.8)
);

out vec2 passTexCoord;

vec2 texCoords[4] = vec2[4](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f)
);

void main() {
    float x = float(inVertexData & 0xFu);
    float y = float((inVertexData & 0xF0u) >> 4u);
    float z = float((inVertexData & 0xF00u) >> 8u);
    x += chunkPosition.x;
    y += chunkPosition.y;
    z += chunkPosition.z;

    gl_Position = projView * model * vec4(x, y, z, 1.0);
    pos = vec3(model * vec4(x, y, z, 1.0));

    uint normalIndex = ((inVertexData & 0x7000u) >> 12u);
    normal = normals[normalIndex];

    //Texture coords
    uint index = (inVertexData & 0x18000u) >> 15u;
    uint layer = (inVertexData & 0xFFFE0000u) >> 17u;

//	objectColor = objectColors[layer];

    passTexCoord = vec2(texCoords[index].x, texCoords[index].y * texUnit + layer * texUnit);
//    passTexCoord = texCoords[index];
}
)";

constexpr auto fs_code = R"(
#version 330 core

in vec3 pos;
in vec3 normal;

//in vec3 objectColor;
in vec2 passTexCoord;

out vec4 FragColor;

uniform sampler2D voxelTexture;
//uniform sampler2DArray textureArray;

void main() {
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 objectColor = texture(voxelTexture, passTexCoord).rgb;
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightPos = vec3(500.0f, 500.0f, 500.0f);
    vec3 lightDir = normalize(lightPos - pos);
    float diff = max(dot(normal, lightDir), 0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0f);
//    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//    outColour = passBasicLight * texture(textureArray, passTexCoord);
//    if (outColour.a == 0) {
//        discard;
//    }
}
)";

void
GameApplication::Init()
{
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	SetFixedFPS(0); // unlimit

    world_->GetTextureManager().CreateTexture();

	NoiseTool::GenerateHeightCache(-100, -100, 100, 100);
    camera = &camera_;
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, mouse_callback);
    glfwSetScrollCallback(window_, scroll_callback);

    shader = std::make_shared<Shader<CreateShaderProgramFromString>>(vs_code, fs_code);

    int center_x = 0, center_z = 0;
    int initSize = 5;
    int chunk_vertical = 16;

    DefaultGenerator generator;
    for(int x = center_x-initSize; x < center_x + initSize; ++x)
	{
    	for(int z = center_z-initSize; z < center_z+initSize; ++z)
		{
    		for(int y = 0; y < chunk_vertical; ++y)
    		{
    			ChunkIndex chunk_index {x, y, z};
				auto *chunk = new Chunk(*world_, chunk_index);
                generator.CreateChunkData(WorldUtils::ChunkIndexToPosition(chunk_index), *chunk);
                world_->AddChunk(chunk_index, chunk);
    		}
		}
	}

	for(int x = center_x-initSize; x < center_x + initSize; ++x)
	{
		for(int z = center_z-initSize; z < center_z+initSize; ++z)
		{
			for(int y = 0; y < chunk_vertical; ++y)
			{
				ChunkIndex chunk_index {x, y, z};
				auto *mesh = new ChunkMesh;
				auto *chunk = world_->GetChunkData(chunk_index);
				ChunkMeshUtils::CreateChunkMesh(*world_, *mesh, *chunk);
				vertex_count += mesh->vertices.size();
				index_count += mesh->indices.size();
				auto *chunk_buff = new ChunkBuffGL;
				chunk_buff->CreateBuff(mesh->vertices, mesh->indices);
				chunk_buffs[chunk_index] = chunk_buff;
			}
		}
	}

	auto height = NoiseTool::GenerateHeightWithCache(0, 0);
    camera_.SetPos(glm::vec3{0, height + 5, 0});

}

void GameApplication::Update()
{
}


void GameApplication::RenderScene()
{
    glClear(GL_DEPTH_BUFFER_BIT);
    shader->Use();
    // 绑定纹理
    world_->GetTextureManager().Bind();

    shader->LoadUniform("projView", camera_.Perspective(800.f/600) * camera_.View());
    shader->LoadUniform("model", glm::mat4(1.f));
    shader->LoadUniform("texUnit", 0.25f);

    for(const auto &chunk_buff : chunk_buffs)
	{
		shader->LoadUniform("chunkPosition", WorldUtils::ChunkIndexToPosition(chunk_buff.first));
		chunk_buff.second->draw();
	}
}

void GameApplication::RenderUI()
{
    ImGui::Begin("Debug");
    ImGui::Text("fps: %d", static_cast<int>(fps_ + 0.5));
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
    camera->MouseCallback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->Zoom(yoffset);
}

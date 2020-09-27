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

namespace
{
    Camera *camera;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const MeshFace FRONT_FACE = {
        {1, 1, 1,
         0, 1, 1,
         0, 0, 1,
         1, 0, 1},
         2};

const MeshFace LEFT_FACE = {
        {0, 1, 1,
         0, 1, 0,
         0, 0, 0,
         0, 0, 1},
        3};

const MeshFace BACK_FACE = {
        {0, 1, 0,
         1, 1, 0,
         1, 0, 0,
         0, 0, 0},
         5};

const MeshFace RIGHT_FACE = {
        {1, 1, 0,
         1, 1, 1,
         1, 0, 1,
         1, 0, 0},
        0};

const MeshFace TOP_FACE = {
        {1, 1, 0,
         0, 1, 0,
         0, 1, 1,
         1, 1, 1},
        1};

const MeshFace BOTTOM_FACE = {
        {0, 0, 0,
         1, 0, 0,
         1, 0, 1,
         0, 0, 1},
         4};

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

void add_face(ChunkMesh &mesh, const MeshFace &face, const VoxelIndex &voxelIndex, u16 texture)
{
    std::size_t index = 0;
    for(std::size_t i = 0; i < 4; ++i)
    {
        u8 x = face.vertices[index++] + voxelIndex.x;
        u8 y = face.vertices[index++] + voxelIndex.y;
        u8 z = face.vertices[index++] + voxelIndex.z;

        u32 vertex =
                x | y << 4 | z << 8 | face.normal << 12 | i << 15 | texture << 17;

        mesh.vertices.push_back(vertex);
    }

    auto index_start = mesh.vertices.size() - 4;
    mesh.indices.push_back(index_start);
    mesh.indices.push_back(index_start + 1);
    mesh.indices.push_back(index_start + 2);
    mesh.indices.push_back(index_start + 2);
    mesh.indices.push_back(index_start + 3);
    mesh.indices.push_back(index_start);
}

void CreateChunkData(const Position &chunkPosition, ChunkData &chunkData)
{
	const int dirt_base = 60;
	for(int x = 0; x < WorldConfig::kChunkSizeX; ++x)
	{
		for(int z = 0; z < WorldConfig::kChunkSizeZ; ++z)
		{
			auto horizontal = NoiseTool::GenerateHeightWithCache(chunkPosition.x + x, chunkPosition.z + z);
			for(int y = 0; y < WorldConfig::kChunkSizeY; ++y)
			{
				auto& voxel = chunkData.chunk_data[WorldUtils::voxel_index_to_data_index(VoxelIndex{ x, y, z})];
				int global_height = chunkPosition.y + y;
				if (global_height > horizontal)
				{
					voxel = static_cast<voxel_t>(CommonVoxel::Air);
				}
				else if (global_height == horizontal)
				{
					voxel = static_cast<voxel_t>(CommonVoxel::Grass);
				}
				else if (voxel >= dirt_base)
				{
					voxel = static_cast<voxel_t>(CommonVoxel::Dirt);
				}
				else
				{
					voxel = static_cast<voxel_t>(CommonVoxel::Stone);
				}
			}
		}
	}
}

void CreateChunkMesh(World &world, ChunkMesh &mesh, const ChunkData &chunkData)
{
	for (int x = 0; x < WorldConfig::kChunkSizeX; ++x)
	{
		for (int y = 0; y < WorldConfig::kChunkSizeY; ++y)
		{
			for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z)
			{
				auto voxel_index = VoxelIndex{ x, y, z };

				auto texture = static_cast<u16>(chunkData.chunk_data[WorldUtils::voxel_index_to_data_index(voxel_index)]);

				if(world.IsVoxelTypeAir(chunkData.chunk_index, voxel_index))
					continue;

				if (!world.IsVoxelTypeSolidUnbound(chunkData.chunk_index, UnboundVoxelIndex(x, y, z + 1)))
				{
					add_face(mesh, FRONT_FACE, voxel_index, texture);
				}

				if (!world.IsVoxelTypeSolidUnbound(chunkData.chunk_index, UnboundVoxelIndex(x, y, z - 1)))
				{
					add_face(mesh, BACK_FACE, voxel_index, texture);
				}

				if (!world.IsVoxelTypeSolidUnbound(chunkData.chunk_index, UnboundVoxelIndex(x, y-1, z)))
				{
					add_face(mesh, BOTTOM_FACE, voxel_index, texture);
				}

				if (!world.IsVoxelTypeSolidUnbound(chunkData.chunk_index, UnboundVoxelIndex(x, y+1, z)))
				{
					add_face(mesh, TOP_FACE, voxel_index, texture);
				}

				if (!world.IsVoxelTypeSolidUnbound(chunkData.chunk_index, UnboundVoxelIndex(x-1, y, z)))
				{
					add_face(mesh, LEFT_FACE, voxel_index, texture);
				}

				if (!world.IsVoxelTypeSolidUnbound(chunkData.chunk_index, UnboundVoxelIndex(x+1, y, z)))
				{
					add_face(mesh, RIGHT_FACE, voxel_index, texture);
				}
			}
		}
	}
}

void
GameApplication::Init()
{
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	SetFixedFPS(0); // unlimit

    // 创建纹理对象
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // 加载生成纹理
    int w, h, nrChannels;
    std::vector<std::string> textures = {
            "Textures/dirt.jpg",
            "Textures/dirt.jpg",
            "Textures/grass.jpg",
            "Textures/stone.jpg",
    };
    unsigned char texture_data[16*16*4*3];
    for(int i = 0; i < textures.size(); ++i)
    {
        unsigned char *data = stbi_load(textures[i].c_str(), &w, &h, &nrChannels, 0);
        memcpy(texture_data + i*16*16*3, data, 16*16*3);
        if(data)
        {
            stbi_image_free(data);
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0 /*mipmap*/, GL_RGB, 16, 16*textures.size(), 0/*legacy*/, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

	NoiseTool::GenerateHeightCache(-100, -100, 100, 100);
    camera = &camera_;
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, mouse_callback);
    glfwSetScrollCallback(window_, scroll_callback);

    shader = std::make_shared<Shader<CreateShaderProgramFromString>>(vs_code, fs_code);

    int center_x = 0, center_z = 0;
    int initSize = 5;
    int chunk_vertical = 16;

    for(int x = center_x-initSize; x < center_x + initSize; ++x)
	{
    	for(int z = center_z-initSize; z < center_z+initSize; ++z)
		{
    		for(int y = 0; y < chunk_vertical; ++y)
    		{
    			ChunkIndex chunk_index {x, y, z};
				auto *chunk = new Chunk(*world_);
				auto *chunk_data = new ChunkData;
				chunk_data->chunk_index = chunk_index;
				CreateChunkData(WorldUtils::ChunkIndexToPosition(chunk_index), *chunk_data);
				chunk->data = chunk_data;
				world_->AddChunkData(chunk_index, chunk);
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
				auto *chunk_data = world_->GetChunkData(chunk_index)->data;
				CreateChunkMesh(*world_, *mesh, *chunk_data);
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
    glBindTexture(GL_TEXTURE_2D, texture);

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
    ImGui::Text("fps: %d", static_cast<int>(fps_ + 0.5));
    static bool show = false;
    if(ImGui::Button("Show voxel info"))
	{
    	show = true;
	}

    if(show)
	{
    	world_->voxel_manager.ForEach([](voxel_t t, const VoxelTypeData &data){
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
}

void GameApplication::HandleKeyboard(GLFWwindow *window)
{
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
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    camera->MouseCallback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->Zoom(yoffset);
}

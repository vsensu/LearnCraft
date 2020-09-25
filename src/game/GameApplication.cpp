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

#define DEBUG_GL_ERRORS

#include "common/gl_errors.h"

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

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec3 passTexCoord;
out vec3 normal;

vec2 texCoords[4] = vec2[4](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f)
);

vec3 normals[6] = vec3[6](
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f),
    vec3(-1.0f, 0.0f, 0.0f),
    vec3(0.0f, -1.0f, 0.0f),
    vec3(0.0f, 0.0f, -1.0f)
);

void main() {
    float x = float(inVertexData & 0xFu);
    float y = float((inVertexData & 0xF0u) >> 4u);
    float z = float((inVertexData & 0xF00u) >> 8u);
    x += chunkPosition.x;
    y += chunkPosition.y;
    z += chunkPosition.z;

    gl_Position = proj * view * model * vec4(x, y, z, 1.0);
    pos = vec3(model * vec4(x, y, z, 1.0));

    uint normalIndex = ((inVertexData & 0x7000u) >> 12u);
    normal = normals[normalIndex];

    //Texture coords
    uint index = (inVertexData & 0x18000u) >> 15u;
    uint layer = (inVertexData & 0xFFFE0000u) >> 17u;

    passTexCoord = vec3(texCoords[index], float(layer));
}
)";

constexpr auto fs_code = R"(
#version 330 core

in vec3 pos;
in vec3 normal;
in vec3 passTexCoord;

out vec4 FragColor;

//uniform sampler2DArray textureArray;

void main() {
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 objectColor = vec3(1.0f, 0.5f, 0.31f);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightPos = vec3(3.0f, 3.0f, 3.0f);
    vec3 lightDir = normalize(lightPos - pos);
    float diff = max(dot(normal, lightDir), 0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0f);
//    outColour = passBasicLight * texture(textureArray, passTexCoord);
//    if (outColour.a == 0) {
//        discard;
//    }
}
)";

void add_face(ChunkMesh &mesh, const MeshFace &face, const VoxelLocalPosition &localPosition, u16 texture)
{
    std::size_t index = 0;
    for(std::size_t i = 0; i < 4; ++i)
    {
        u8 x = face.vertices[index++] + localPosition.x;
        u8 y = face.vertices[index++] + localPosition.y;
        u8 z = face.vertices[index++] + localPosition.z;

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

void
GameApplication::Init()
{
    glEnable(GL_DEPTH_TEST);

    camera = &camera_;
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, mouse_callback);
    glfwSetScrollCallback(window_, scroll_callback);

    shader = std::make_shared<Shader<CreateShaderProgramFromString>>(vs_code, fs_code);

    add_face(mesh, FRONT_FACE, VoxelLocalPosition(0, 0, 0), 0);
    add_face(mesh, BACK_FACE, VoxelLocalPosition(0, 0, 0), 0);
    add_face(mesh, LEFT_FACE, VoxelLocalPosition(0, 0, 0), 0);
    add_face(mesh, RIGHT_FACE, VoxelLocalPosition(0, 0, 0), 0);
    add_face(mesh, TOP_FACE, VoxelLocalPosition(0, 0, 0), 0);
    add_face(mesh, BOTTOM_FACE, VoxelLocalPosition(0, 0, 0), 0);

    // 顶点数组对象, 顶点缓冲对象, 索引缓冲对象
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // 绑定
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_t) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
    constexpr GLuint location = 0;
    constexpr GLint floatCount = 1;
    glVertexAttribIPointer(location, floatCount, GL_UNSIGNED_INT, 0, (GLvoid*)0);
    glEnableVertexAttribArray(location);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

}

void GameApplication::Update()
{
}


void GameApplication::RenderScene()
{
    glClear(GL_DEPTH_BUFFER_BIT);
    shader->Use();
    glBindVertexArray(vao);

    shader->LoadUniform("chunkPosition",glm::vec3(0, 0, 0));
    shader->LoadUniform("proj", camera_.Perspective(800.f/600));
    shader->LoadUniform("view", camera_.View());
    shader->LoadUniform("model", glm::mat4(1.f));

    glCheck(glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr));
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
    ImGui::Text("vertex: %d index: %d", mesh.vertices.size(), mesh.indices.size());
    for(std::size_t i = 0; i < mesh.vertices.size(); ++i)
    {
        auto inVertexData = mesh.vertices[i];
        float x = float(inVertexData & 0xFu);
        float y = float((inVertexData & 0xF0u) >> 4u);
        float z = float((inVertexData & 0xF00u) >> 8u);
        ImGui::Text("%f, %f, %f", x, y, z);
    }

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

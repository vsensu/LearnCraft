//
// Created by reili on 2020/10/8.
//

#ifndef LEARNCRAFT_DRAWDEBUG_H
#define LEARNCRAFT_DRAWDEBUG_H

#include <vector>
#include <memory>

#include <glad/glad.h>
#include <common/world/WorldTypes.h>
#include <common/world/ChunkRender.h>
#include <common/Shader.hpp>

constexpr auto box_vs_code = R"(
#version 330 core

layout (location = 0) in uint inVertexData;

uniform vec3 pos;

uniform mat4 projView;
uniform mat4 model;

void main() {
    float x = float(inVertexData & 0xFu);
    float y = float((inVertexData & 0xF0u) >> 4u);
    float z = float((inVertexData & 0xF00u) >> 8u);

    gl_Position = projView * model * vec4(pos.x + x, pos.y+y, pos.z+z, 1.0);

    //Texture coords
    uint index = (inVertexData & 0x18000u) >> 15u;
    uint layer = (inVertexData & 0xFFFE0000u) >> 17u;
}
)";

constexpr auto box_fs_code = R"(
#version 330 core

out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0f);
}
)";

struct VoxelBuffer
{
    void CreateBuff(const std::vector<u32> &vertices, const std::vector<u32> &indices)
    {
        index_count = indices.size();
        // 顶点数组对象, 顶点缓冲对象, 索引缓冲对象
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        // 绑定
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(u32) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices.size(), indices.data(), GL_STATIC_DRAW);
        constexpr GLuint location = 0;
        constexpr GLint floatCount = sizeof(u32) / sizeof(float);
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

    void draw()
    {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }

    GLuint vao, vbo, ebo;
    std::size_t index_count;
};

struct DrawDebug
{
    inline static std::shared_ptr<Shader<CreateShaderProgramFromString>> box_shader {nullptr};
    struct CompressedMesh
    {
        std::vector<u32> vertices;
        std::vector<u32> indices;
    };

    static void add_face(CompressedMesh &mesh, const MeshFace &face)
    {
        std::size_t index = 0;
        for(std::size_t i = 0; i < 4; ++i)
        {
            u8 x = face.vertices[index++];
            u8 y = face.vertices[index++];
            u8 z = face.vertices[index++];

            u32 vertex =
                    x | y << 4 | z << 8 | face.normal << 12 | i << 15 | 1 << 17;

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

    inline static VoxelBuffer box;
    static void Init()
    {
        box_shader = std::make_shared<Shader<CreateShaderProgramFromString>>(box_vs_code, box_fs_code);

        CompressedMesh box_mesh;
        add_face(box_mesh, MeshFaces::kBackFace);
        add_face(box_mesh, MeshFaces::kFrontFace);
        add_face(box_mesh, MeshFaces::kLeftFace);
        add_face(box_mesh, MeshFaces::kRightFace);
        add_face(box_mesh, MeshFaces::kTopFace);
        add_face(box_mesh, MeshFaces::kBottomFace);
        box.CreateBuff(box_mesh.vertices, box_mesh.indices);
    }
    static void SetProjView(const glm::mat4 &projView)
    {
        proj_view = projView;
    }

    struct Transform
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

    static void DrawDebugBox(const Position &pos, const glm::vec3 &size=glm::vec3(1.0f));
    static void Render();
    inline static inline glm::mat4 proj_view;
    inline static std::vector<Position> box_list;
};

#endif //LEARNCRAFT_DRAWDEBUG_H

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

//struct Vertex
//{
//    x,y,z; 4+4+4 = 12;
//    2;  // uv index
//    15; // texture index
//    3;  // normal index
//};

using vertex_t = u32;
using index_t = u32;

struct ChunkMesh
{
    std::vector<vertex_t> vertices;
    std::vector<index_t> indices;
};

struct MeshFace
{
    std::array<u8, 12> vertices;
    u8 normal; // +x:0 +y:1 +z:2 -x:3 -y:4 -z:5
};

struct ChunkBuffGL
{
	void CreateBuff(const std::vector<vertex_t> &vertices, const std::vector<index_t> &indices)
	{
		index_count = indices.size();
		// 顶点数组对象, 顶点缓冲对象, 索引缓冲对象
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		// 绑定
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
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

	void draw()
	{
		glBindVertexArray(vao);
		glCheck(glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr));
	}

	GLuint vao, vbo, ebo;
	std::size_t index_count;
};

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
    GLuint texture;
};

#endif //LEARNCRAFT_GAMEAPPLICATION_H

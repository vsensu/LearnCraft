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

    std::shared_ptr<World> world_ { std::make_shared<World>(32, 32, 32) };
    Camera camera_;
    GLuint vao, vbo, ebo;
    ChunkMesh mesh;
    std::shared_ptr<Shader<CreateShaderProgramFromString>> shader {nullptr};
};

#endif //LEARNCRAFT_GAMEAPPLICATION_H

//
// Created by reili on 2020/10/1.
//

#ifndef LEARNCRAFT_CHUNKRENDERSYSTEM_H
#define LEARNCRAFT_CHUNKRENDERSYSTEM_H

#include <entt/entt.hpp>
#include <glad/glad.h>
#include <common/world/ChunkGenerator.h>
#include <game/comp/PositionComponent.h>
#include <game/comp/ChunkRenderComponent.h>
#include <game/comp/ChunkIndexComponent.h>

#include "common/world/WorldUtils.h"
#include "common/world/WorldTypes.h"

#include "common/Shader.hpp"

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

struct ChunkRenderSystem
{
    inline static std::shared_ptr<Shader<CreateShaderProgramFromString>> shader {nullptr};
    inline static GLuint texture;
    inline static u32 vertex_draw_count { 0 };

    static void init(GLuint tex)
    {
        shader = std::make_shared<Shader<CreateShaderProgramFromString>>(vs_code, fs_code);
        texture = tex;
    }

    static void Tick(Camera &camera, entt::registry &registry)
    {
        shader->Use();
        glBindTexture(GL_TEXTURE_2D, texture);

        shader->LoadUniform("projView", camera.Perspective(800.f/600) * camera.View());
        shader->LoadUniform("model", glm::mat4(1.f));
        shader->LoadUniform("texUnit", 0.25f);

        const ViewFrustum& frustum = camera.GetFrustum();
        static float sightRange = 80.0f;
        vertex_draw_count = 0;

        registry.view<ChunkIndexComponent, ChunkRenderComponent>().each([&](const auto &chunk_index, auto &render) {
            auto pos = WorldUtils::ChunkIndexToPosition(chunk_index.val);
            if(WorldUtils::IsPointInSightRange(camera.GetPos(), pos, sightRange))
            {
                if(WorldUtils::chunkIsInFrustum(frustum, pos))
                {
                    shader->LoadUniform("chunkPosition", pos);
                    glBindVertexArray(render.vao);
                    glCheck(glDrawElements(GL_TRIANGLES, render.index_count, GL_UNSIGNED_INT, nullptr));
                    vertex_draw_count += render.index_count;
                }
            }
        });

        vertex_draw_count = (vertex_draw_count * 3u) >> 1;
    }
};

#endif //LEARNCRAFT_CHUNKRENDERSYSTEM_H

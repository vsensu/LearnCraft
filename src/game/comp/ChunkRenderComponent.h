//
// Created by reili on 2020/10/1.
//

#ifndef LEARNCRAFT_CHUNKRENDERCOMPONENT_H
#define LEARNCRAFT_CHUNKRENDERCOMPONENT_H

#include <glad/glad.h>
#include <cstdlib>

struct ChunkRenderComponent
{
    GLuint vao;
    std::size_t index_count;
};

#endif //LEARNCRAFT_CHUNKRENDERCOMPONENT_H

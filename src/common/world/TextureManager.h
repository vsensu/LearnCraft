//
// Created by reili on 2020/9/27.
//

#ifndef LEARNCRAFT_TEXTUREMANAGER_H
#define LEARNCRAFT_TEXTUREMANAGER_H

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include "WorldTypes.h"

struct VoxelTextureNames
{
    std::string front;
    std::string back;
    std::string left;
    std::string right;
    std::string top;
    std::string bottom;
};

//struct VoxelTexturePaths
//{
//    std::string front;
//    std::string back;
//    std::string left;
//    std::string right;
//    std::string top;
//    std::string bottom;
//};

struct VoxelTextureLayers
{
    tex_t front;
    tex_t back;
    tex_t left;
    tex_t right;
    tex_t top;
    tex_t bottom;
};

// key:name value:path
class TextureManager
{
public:
    [[nodiscard]] bool RegisterTexture(const std::string &name, const std::string &path)
    {
        name_tex_map_[name] = textures.size();
        textures.push_back(path);
        return true;
    }

    tex_t GetVoxelTextureLayer(const std::string &name)
    {
        return name_tex_map_[name];
    }

    GLuint CreateTexture()
    {
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

        unsigned char *texture_data = new unsigned char[16*16*textures.size()*3];
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
        return texture;
    }

    inline auto GetTextureNum() const { return textures.size(); }

protected:
    std::vector<std::string> textures;
    std::unordered_map<std::string, tex_t> name_tex_map_;
    GLuint texture;
};

#endif //LEARNCRAFT_TEXTUREMANAGER_H

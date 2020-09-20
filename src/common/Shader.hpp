//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_SHADER_H
#define LEARNCRAFT_SHADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include <glad/glad.h>

struct ShaderUtils
{
    static GLuint CreateShaderProgramFromFile(const std::string& vsPath, const std::string& fsPath)
    {
        // Read vertex shader code from file
        std::ifstream vShaderFile(vsPath);
        if(!vShaderFile.is_open())
        {
            std::cerr << "Failed to load vertex shader" << std::endl;
            terminate();
        }
        vShaderFile.seekg(0, std::ios::end);
        std::string vertexCode;
        vertexCode.resize(vShaderFile.tellg());
        vShaderFile.seekg(0);
        vShaderFile.read(vertexCode.data(), vertexCode.size());

        // Read fragment shader code from file
        std::ifstream fShaderFile(fsPath);
        if (!fShaderFile.is_open())
        {
            std::cerr << "Failed to load fragment shader" << std::endl;
            terminate();
        }
        fShaderFile.seekg(0, std::ios::end);
        std::string fragmentCode;
        fragmentCode.resize(fShaderFile.tellg());
        fShaderFile.seekg(0);
        fShaderFile.read(fragmentCode.data(), fragmentCode.size());

        return CreateShaderProgramFromString(vertexCode, fragmentCode);
    }

    static GLuint CreateShaderProgramFromString(const std::string& vsCode, const std::string& fsCode)
    {
        // Compile vertex shader
        const GLchar* vShaderCode = vsCode.c_str();
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
        glCompileShader(vertexShader);
        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        GLchar infoLog[1024];
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Error compiling shader type " << GL_VERTEX_SHADER << ": " << infoLog << std::endl;
        }

        // Compile fragment shader
        const GLchar* fShaderCode = fsCode.c_str();
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Error compiling shader type " << GL_FRAGMENT_SHADER << ": " << infoLog << std::endl;
        }

        // Create Shader program
        GLuint ShaderProgram = glCreateProgram();
        // Attach compiled shader object to the program object.
        // This is very similar to specifying the list of objects for linking in a makefile.
        // Since we don't have a makefile here we emulate this behavior programatically.
        // Only the attached objects take part of the linking process.
        glAttachShader(ShaderProgram, vertexShader);
        glAttachShader(ShaderProgram, fragmentShader);
        glLinkProgram(ShaderProgram);
        glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ShaderProgram, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Error linking shader program: " << infoLog << std::endl;
        }
        // You may ask yourself why do we need to validate a program after it has been successfully linked.
        // The difference is that linking checks for errors based on the combination of shaders while
        // the call above check whether the program can execute given the current pipeline state.
        // In a complex application with multiple shaders and lots of state changes it is better to validate before every draw call.
        // In our simple app we check it just once.
        // Also, you may want to do this check only during development and avoid this overhead in the final product.
        glValidateProgram(ShaderProgram);

        // Delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return ShaderProgram;
    }
};

// Policy classes begin
struct CreateShaderProgramFromFile
{
    auto Create(const std::string& vsPath, const std::string& fsPath)
    {
        return ShaderUtils::CreateShaderProgramFromFile(vsPath, fsPath);
    }
};

struct CreateShaderProgramFromString
{
    auto Create(const std::string& vsCode, const std::string& fsCode)
    {
        return ShaderUtils::CreateShaderProgramFromString(vsCode, fsCode);
    }
};
// Policy classes end

template <typename CreatePolicy>
struct Shader
{
    CreatePolicy createPolicy;
    GLuint shaderProgram;
    Shader(const std::string& vs, const std::string& fs)
    : shaderProgram(createPolicy.Create(vs, fs))
    {
    }

    void Use() const
    {
        glUseProgram(shaderProgram);
    }

    template<typename UniformType>
    void SetUniform(const std::string &name, UniformType value) const
    {
    }

    template<>
    void SetUniform(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), static_cast<int>(value));
    }

    template<>
    void SetUniform(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }

    template<>
    void SetUniform(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }

    template<>
    void SetUniform(const std::string &name, GLfloat *value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, value);
    }

    template<typename UniformType>
    void SetUniform(const std::string &name, UniformType v1, UniformType v2, UniformType v3, UniformType v4)
    {

    }

    template<>
    void SetUniform(const std::string &name, float v1, float v2, float v3, float v4)
    {
        glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), v1, v2, v3, v4);
    }
};

#endif //LEARNCRAFT_SHADER_H

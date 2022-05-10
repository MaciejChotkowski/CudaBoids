#pragma once

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/mat4x4.hpp>

class Shader
{
private:
    unsigned int LoadShaderFromFile(const char *pathToFile, GLenum typeOfShader);
    unsigned int ShaderID;

public:
    Shader(const char *vertexPath, const char *fragmentPath);
    ~Shader();
    void addMat4(const char *name, glm::mat4 &data);
    void addVec3(const char *name, const glm::vec3 &data);
    unsigned int GetShaderID();
};
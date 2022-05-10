#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Loader.h"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <map>

class Mesh
{
private:
    unsigned int VAO, VBO, EBO;
    void Start();
    unsigned int TextureID;
    int Frame;
    float lastTime;
    int AnimationFrames;
    float FrameRate;
    int InstanceCount;
    float Scale;
    float Pitch, Yaw;
    unsigned int ShaderProgramID;
    std::vector<std::vector<float>> vertices;
    std::vector<std::vector<int>> indices;
    glm::vec3 Position;
    void setFrame(int f, int shader);

public:
    Mesh(const char *PathToFile, unsigned int shaderProgramID);
    Mesh(const char *PathToFiles, unsigned int shaderProgramID, int AnimationFrames, float framesPerSecond);
    ~Mesh();
    void LoadTextureFromFile(const char *PathToFile);
    void Draw(glm::mat4 model = glm::mat4(1.0f));
    void setPosition(glm::vec3 pos);
    glm::vec3 getPosition();
    void Move(glm::vec3 v);
    void setRotation(float pitch, float yaw);
    void setScale(float scale);
    void MakeInstanced(int instanceCount);
};
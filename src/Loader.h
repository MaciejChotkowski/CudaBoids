#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ext/stb_image.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <fstream>

namespace Loader
{
    void LoadObject(const char *pathToFile, std::vector<float> *vertices, std::vector<int> *indices);
    unsigned int LoadTexture(const char *pathToFile, unsigned int shaderProgramID);
};
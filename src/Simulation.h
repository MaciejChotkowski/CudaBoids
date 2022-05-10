#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <queue>

#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "TimeMonitor.h"
#include "GUI.h"
#include "SimulationValues.h"
#include "NumericValues.h"
#include "Shoal.h"

#include "Computations.cuh"
#include "cuda_gl_interop.h"

#include <numeric>

class Simulation
{
private:
    Shader *InstancedShader;
    Shader *FlatShader;
    Shader *RegularShader;
    TimeMonitor *timeMonitor;
    GUI *Gui;
    GLFWwindow *window;
    Mesh *aquarium;
    Mesh *aquarium2;
    Mesh *cursor;
    Mesh *sand;
    Shoal *shoalOfPrey;
    Shoal *shoalOfPredators;
    Camera *Cam;
    int ScreenWidth, ScreenHeight;
    float deltaTime;
    bool MouseIsLocked;
    bool EscIsPressed;
    glm::vec3 ClearColor;
    SimulationValues Values;
    void StartWindow(const char *name);
    void ProcessPositions();
    void InitNumericValues();
    void LoadFiles();
    void Render();
    void processInput(GLFWwindow *window);
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);

public:
    Simulation(int screenWidth, int screenHeight, char const *name);
    ~Simulation();
    void Update();
    bool isRunning();
};
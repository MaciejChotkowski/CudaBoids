#pragma once

#include "ext/imgui.h"
#include "ext/imgui_impl_glfw.h"
#include "ext/imgui_impl_opengl3.h"

#include "Mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "SimulationValues.h"
#include "NumericValues.h"
#include "Shoal.h"

class GUI
{
private:
    SimulationValues *Values;
    void PrintPreySettings();
    void PrintPredatorsSettings();
    void PrintInstructions();

public:
    GUI(GLFWwindow *window, SimulationValues *values);
    ~GUI();
    void Render(int fps, Shoal *prey);
};
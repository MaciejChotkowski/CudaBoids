#include "GUI.h"

void GUI::Render(int fps, Shoal *prey)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_FirstUseEver);
    ImGui::Begin("Options menu");
    ImGui::Text("Press Esc to unlock mouse");
    std::string fpsString = "fps: " + std::to_string(fps);
    ImGui::Text("%s", fpsString.c_str());

    if (Values->Settings.RunningOnCPU)
    {
        if (ImGui::Button("GPU"))
        {
            Values->Settings.RunningOnCPU = false;
            prey->UpdateCudaBuffers();
        }
    }
    else
    {
        if (ImGui::Button("CPU"))
        {
            Values->Settings.RunningOnCPU = true;
            Values->Settings.CursorVisible = false;
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Display mode"))
    {
        Values->Settings.DisplayMode++;
        if (Values->Settings.DisplayMode > 2)
            Values->Settings.DisplayMode = 0;

        switch (Values->Settings.DisplayMode)
        {
        case 0:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case 1:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 2:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        }
    }

    ImGui::SameLine();
    if (Values->Settings.Paused)
    {
        if (ImGui::Button("Play"))
        {
            Values->Settings.Paused = false;
        }
    }
    else
    {
        if (ImGui::Button("Pause"))
        {
            Values->Settings.Paused = true;
        }
    }

    ImGui::SameLine();
    if (Values->Settings.SphericalContainer)
    {
        if (ImGui::Button("Cuboid"))
        {
            Values->Settings.SphericalContainer = false;
            Values->Environment.CuboidAquarium->setScale(Values->Environment.SizeOfContainer);
        }
    }
    else
    {
        if (ImGui::Button("Sphere"))
        {
            Values->Settings.SphericalContainer = true;
            Values->Environment.SphereAquarium->setScale(Values->Environment.SizeOfContainer);
        }
    }

    if (!Values->Settings.RunningOnCPU)
    {
        ImGui::SameLine();
        if (Values->Settings.CursorVisible)
        {
            if (ImGui::Button("Hide worm"))
            {
                Values->Settings.CursorVisible = false;
            }
        }
        else
        {
            if (ImGui::Button("Show worm"))
            {
                Values->Settings.CursorVisible = true;
            }
        }
    }

    if (ImGui::SliderInt("Container", &Values->Environment.SizeOfContainer, MIN_SIZE_OF_CONTAINER, MAX_SIZE_OF_CONTAINER))
    {
        if (Values->Settings.SphericalContainer)
        {
            Values->Environment.SphereAquarium->setScale(Values->Environment.SizeOfContainer);
        }
        else
        {
            Values->Environment.CuboidAquarium->setScale(Values->Environment.SizeOfContainer);
        }
    }

    if (Values->Settings.CursorVisible)
    {
        ImGui::SliderInt("Worm distance", &Values->Environment.CursorDistance, MIN_CURSOR_DISTANCE, MAX_CURSOR_DISTANCE);
    }

    PrintInstructions();
    PrintPreySettings();
    PrintPredatorsSettings();

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

GUI::GUI(GLFWwindow *window, SimulationValues *values)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    Values = values;
    gladLoadGL();
}

GUI::~GUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::PrintPreySettings()
{
    if (ImGui::CollapsingHeader("Prey options"))
    {
        if (ImGui::SliderInt("Number##Prey", &Values->Prey.Number, MIN_PREY_NUMBER, MAX_PREY_NUMBER))
        {
            Values->Prey.FishMesh->MakeInstanced(Values->Prey.Number);
        }
        ImGui::SliderInt("Speed##Prey", &Values->Prey.Speed, MIN_PREY_SPEED, MAX_PREY_SPEED);
        ImGui::Text("Perception radii:");
        ImGui::SliderInt("Big##Prey", &Values->Prey.Perception.BigPerceptionRadius, MIN_PREY_BIG_PERCEPTION_RADIUS, MAX_PREY_BIG_PERCEPTION_RADIUS);
        ImGui::SliderInt("Small##Prey", &Values->Prey.Perception.SmallPerceptionRadius, MIN_PREY_SMALL_PERCEPTION_RADIUS, Values->Prey.Perception.BigPerceptionRadius);
        ImGui::SliderInt("Food##Prey", &Values->Prey.Perception.FoodPerceptionRadius, MIN_PREY_FOOD_PERCEPTION_RADIUS, Values->Environment.SizeOfContainer);
        ImGui::SliderInt("Danger##Prey", &Values->Prey.Perception.DangerPerceptionRadius, MIN_PREY_FOOD_PERCEPTION_RADIUS, Values->Environment.SizeOfContainer);
        ImGui::Text("Forces:");
        ImGui::SliderInt("Alignment##Prey", &Values->Prey.Forces.AlignmentForce, MIN_PREY_ALIGNMENT_FORCE, MAX_PREY_ALIGNMENT_FORCE);
        ImGui::SliderInt("Separation##Prey", &Values->Prey.Forces.SeparationForce, MIN_PREY_SEPARATION_FORCE, MAX_PREY_SEPARATION_FORCE_);
        ImGui::SliderInt("Cohesion##Prey", &Values->Prey.Forces.CohesionForce, MIN_PREY_COHESION_FORCE, MAX_PREY_COHESION_FORCE);
        ImGui::SliderInt("Hunger##Prey", &Values->Prey.Forces.HungerForce, MIN_PREY_HUNGER_FORCE, MAX_PREY_HUNGER_FORCE);
        ImGui::SliderInt("Fear##Prey", &Values->Prey.Forces.FearForce, MIN_PREY_FEAR_FORCE, MAX_PREY_FEAR_FORCE);
    }
}

void GUI::PrintPredatorsSettings()
{
    if (ImGui::CollapsingHeader("Predators options"))
    {
        if (ImGui::SliderInt("Number##Predators", &Values->Predators.Number, MIN_PREDATORS_NUMBER, MAX_PREDATORS_NUMBER))
        {
            Values->Predators.FishMesh->MakeInstanced(Values->Predators.Number);
        }
        ImGui::SliderInt("Speed##Predators", &Values->Predators.Speed, MIN_PREDATORS_SPEED, MAX_PREDATORS_SPEED);
        ImGui::Text("Perception radii:");
        ImGui::SliderInt("Big##Predators", &Values->Predators.Perception.BigPerceptionRadius, MIN_PREDATORS_BIG_PERCEPTION_RADIUS, MAX_PREDATORS_BIG_PERCEPTION_RADIUS);
        ImGui::SliderInt("Small##Predators", &Values->Predators.Perception.SmallPerceptionRadius, MIN_PREDATORS_SMALL_PERCEPTION_RADIUS, Values->Predators.Perception.BigPerceptionRadius);
        ImGui::SliderInt("Food##Predators", &Values->Predators.Perception.FoodPerceptionRadius, MIN_PREDATORS_FOOD_PERCEPTION_RADIUS, Values->Environment.SizeOfContainer);
        ImGui::Text("Forces:");
        ImGui::SliderInt("Alignment##Predators", &Values->Predators.Forces.AlignmentForce, MIN_PREDATORS_ALIGNMENT_FORCE, MAX_PREDATORS_ALIGNMENT_FORCE);
        ImGui::SliderInt("Separation##Predators", &Values->Predators.Forces.SeparationForce, MIN_PREDATORS_SEPARATION_FORCE, MAX_PREDATORS_SEPARATION_FORCE);
        ImGui::SliderInt("Cohesion##Predators", &Values->Predators.Forces.CohesionForce, MIN_PREDATORS_COHESION_FORCE, MAX_PREDATORS_COHESION_FORCE);
        ImGui::SliderInt("Hunger##Predators", &Values->Predators.Forces.HungerForce, MIN_PREDATORS_HUNGER_FORCE, MAX_PREDATORS_HUNGER_FORCE);
    }
}

void GUI::PrintInstructions()
{
    if (ImGui::CollapsingHeader("Instructions"))
    {
        ImGui::Text("WASD moves 3D camera.");
        ImGui::Text("LShift moves camera down.");
        ImGui::Text("Space moves camera up.");
        ImGui::Text("Mouse movement rotates camera.");
        ImGui::Text("Esc locks and unlocks mouse.");
        ImGui::Text("Switching computations to CPU is not \nadvised with high number of fish.");
        ImGui::Text("Prey chases the worm and escapes from \npredators.");
        ImGui::Text("Predators chase only prey.");
        ImGui::Text("Both types display flocking behavior \namong its own kind.");
    }
}
#include "Simulation.h"

Simulation::Simulation(int scrW, int scrH, char const *name)
{
    ClearColor = glm::vec3(89.0 / 255, 184.0 / 255, 240.0 / 255);
    MouseIsLocked = true;
    EscIsPressed = false;

    ScreenWidth = scrW;
    ScreenHeight = scrH;
    Cam = new Camera();

    StartWindow(name);
    InitNumericValues();
    LoadFiles();

    aquarium->setScale(Values.Environment.SizeOfContainer);
    aquarium2->setScale(Values.Environment.SizeOfContainer);
    Values.Environment.CuboidAquarium = aquarium;
    Values.Environment.SphereAquarium = aquarium2;
    sand->setPosition(glm::vec3(0, -1050, 100));
    Values.Prey.FishMesh = shoalOfPrey->FishMesh;
    Values.Predators.FishMesh = shoalOfPredators->FishMesh;

    Compute::init(MAX_PREY_NUMBER, MAX_PREDATORS_NUMBER);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)ScreenWidth / (float)ScreenHeight, 0.1f, Values.Environment.SizeOfContainer * 4.0f);
    InstancedShader->addMat4("projection", projection);
    FlatShader->addMat4("projection", projection);
    RegularShader->addMat4("projection", projection);
    InstancedShader->addVec3("clearColor", ClearColor);
    RegularShader->addVec3("clearColor", ClearColor);

    Gui = new GUI(window, &Values);
    timeMonitor = new TimeMonitor();
}

Simulation::~Simulation()
{
    std::cout << std::endl
              << "Closing simulation" << std::endl;

    Compute::free();

    delete Gui;
    delete Cam;
    delete timeMonitor;

    delete sand;
    delete aquarium;
    delete aquarium2;
    delete cursor;

    delete InstancedShader;
    delete FlatShader;
    delete RegularShader;

    glfwTerminate();
}

// Simulation's main loop.
void Simulation::Update()
{
    timeMonitor->Monitor();
    deltaTime = timeMonitor->getDeltaTime();
    processInput(window);

    Cam->Move(window, deltaTime);
    glm::mat4 view = Cam->getView();
    InstancedShader->addMat4("view", view);
    FlatShader->addMat4("view", view);
    RegularShader->addMat4("view", view);

    if (!Values.Settings.Paused)
        ProcessPositions();

    Render();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Simulation::isRunning()
{
    return !glfwWindowShouldClose(window);
}

void Simulation::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !EscIsPressed)
    {
        if (MouseIsLocked)
        {
            Cam->SwitchCameraRotation(false);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            Cam->SwitchCameraRotation(true);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        MouseIsLocked = !MouseIsLocked;
        EscIsPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
    {
        EscIsPressed = false;
    }
}

void Simulation::StartWindow(const char *name)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(ScreenWidth, ScreenHeight, name, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        glfwSetWindowShouldClose(window, true);
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        glfwSetWindowShouldClose(window, true);
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glfwSetWindowUserPointer(window, Cam);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Simulation::ProcessPositions()
{
    if (Values.Settings.RunningOnCPU)
    {
        Compute::ComputeWithCPU(&shoalOfPrey->positions, &shoalOfPrey->velocities, deltaTime, &Values);
    }
    else
    {
        Compute::ComputeWithCUDA(shoalOfPrey->d_positions, shoalOfPrey->d_velocities, shoalOfPredators->d_positions,
                                 shoalOfPredators->d_velocities, deltaTime, &Values);
    }
}

// All the things that need to be in some way displayed are being displayed here.
void Simulation::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    sand->Draw();

    if (Values.Settings.CursorVisible)
    {
        cursor->setPosition(Cam->getInFrontPosition(Values.Environment.CursorDistance));
        cursor->Draw();
        Values.Environment.CursorPosition = cursor->getPosition();
    }

    if (Values.Settings.SphericalContainer)
    {
        aquarium2->Draw();
    }
    else
    {
        aquarium->Draw();
    }

    shoalOfPrey->Draw(Values.Prey.Number, Values.Settings.RunningOnCPU);

    if (!Values.Settings.RunningOnCPU)
        shoalOfPredators->Draw(Values.Predators.Number, false);

    Gui->Render(timeMonitor->GetFPS(), shoalOfPrey);
}

// Initialisation of magic numbers.
void Simulation::InitNumericValues()
{
    Values.Prey.Speed = START_PREY_SPEED;
    Values.Prey.Number = START_PREY_NUMBER;
    Values.Prey.Forces.CohesionForce = START_PREY_COHESION_FORCE;
    Values.Prey.Forces.AlignmentForce = START_PREY_ALIGNMENT_FORCE;
    Values.Prey.Forces.SeparationForce = START_PREY_SEPARATION_FORCE_;
    Values.Prey.Forces.HungerForce = START_PREY_HUNGER_FORCE;
    Values.Prey.Forces.FearForce = START_PREY_FEAR_FORCE;
    Values.Prey.Perception.BigPerceptionRadius = START_PREY_BIG_PERCEPTION_RADIUS;
    Values.Prey.Perception.SmallPerceptionRadius = START_PREY_SMALL_PERCEPTION_RADIUS;
    Values.Prey.Perception.FoodPerceptionRadius = START_PREY_FOOD_PERCEPTION_RADIUS;
    Values.Prey.Perception.DangerPerceptionRadius = START_PREY_DANGER_PERCEPTION_RADIUS;

    Values.Predators.Speed = START_PREDATORS_SPEED;
    Values.Predators.Number = START_PREDATORS_NUMBER;
    Values.Predators.Forces.CohesionForce = START_PREDATORS_COHESION_FORCE;
    Values.Predators.Forces.AlignmentForce = START_PREDATORS_ALIGNMENT_FORCE;
    Values.Predators.Forces.SeparationForce = START_PREDATORS_SEPARATION_FORCE;
    Values.Predators.Forces.HungerForce = START_PREDATORS_HUNGER_FORCE;
    Values.Predators.Perception.BigPerceptionRadius = START_PREDATORS_BIG_PERCEPTION_RADIUS;
    Values.Predators.Perception.SmallPerceptionRadius = START_PREDATORS_SMALL_PERCEPTION_RADIUS;
    Values.Predators.Perception.FoodPerceptionRadius = START_PREDATORS_FOOD_PERCEPTION_RADIUS;

    Values.Settings.Paused = START_PAUSED;
    Values.Settings.DisplayMode = START_DISPLAY_MODE;
    Values.Settings.RunningOnCPU = START_RUNNING_ON_CPU;
    Values.Settings.SphericalContainer = START_SPHERICAL_CONTAINER;
    Values.Settings.CursorVisible = START_CURSOR_VISIBLE;

    Values.Environment.SizeOfContainer = START_SIZE_OF_CONTAINER;
    Values.Environment.CursorDistance = START_CURSOR_DISTANCE;
}

// All objects that need access to a file with a given path are initialised here.
void Simulation::LoadFiles()
{
    InstancedShader = new Shader("shd/InstancedVertexShader.glsl", "shd/InstancedFragmentShader.glsl");
    FlatShader = new Shader("shd/FlatVertexShader.glsl", "shd/FlatFragmentShader.glsl");
    RegularShader = new Shader("shd/RegularVertexShader.glsl", "shd/RegularFragmentShader.glsl");
    shoalOfPrey = new Shoal("smallFish", 30, 60, InstancedShader->GetShaderID(),
                            START_PREY_NUMBER, MAX_PREY_NUMBER, Values.Environment.SizeOfContainer);
    shoalOfPredators = new Shoal("bigFish", 30, 60, InstancedShader->GetShaderID(),
                                 START_PREDATORS_NUMBER, MAX_PREDATORS_NUMBER, Values.Environment.SizeOfContainer);

    cursor = new Mesh("obj/worm/anim/worm", RegularShader->GetShaderID(), 60, 60);
    cursor->LoadTextureFromFile("obj/worm/worm.png");

    aquarium = new Mesh("obj/aquarium/aquarium_cuboid", RegularShader->GetShaderID());
    aquarium->LoadTextureFromFile("obj/aquarium/aquarium.png");
    aquarium2 = new Mesh("obj/aquarium/aquarium_sphere", RegularShader->GetShaderID());
    aquarium2->LoadTextureFromFile("obj/aquarium/aquarium.png");

    sand = new Mesh("obj/sand/sand", RegularShader->GetShaderID());
    sand->LoadTextureFromFile("obj/sand/sand.png");
}

void Simulation::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Simulation::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    Camera *Cam = static_cast<Camera *>(glfwGetWindowUserPointer(window));
    Cam->ProcessMouseMovement(xpos, ypos);
}
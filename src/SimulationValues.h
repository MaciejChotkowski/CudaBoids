#pragma once

#include "Mesh.h"

// A set of structs for containing frequently accessed simulation data.

struct SettingsValues
{
    bool Paused;
    int DisplayMode;
    bool RunningOnCPU;
    bool SphericalContainer;
    bool CursorVisible;
};

struct EnvironmentValues
{
    int SizeOfContainer;
    Mesh *CuboidAquarium;
    Mesh *SphereAquarium;
    glm::vec3 CursorPosition;
    int CursorDistance;
};

struct ForcesValues
{
    int CohesionForce;
    int AlignmentForce;
    int SeparationForce;
    int HungerForce;
    int FearForce;
};

struct PerceptionRadiiValues
{
    int BigPerceptionRadius;
    int SmallPerceptionRadius;
    int FoodPerceptionRadius;
    int DangerPerceptionRadius;
};

struct SpecificShoalValues
{
    int Speed;
    int Number;
    Mesh *FishMesh;
    ForcesValues Forces;
    ForcesValues MinForces;
    ForcesValues MaxForces;
    PerceptionRadiiValues Perception;
};

struct SimulationValues
{
    SpecificShoalValues Prey;
    SpecificShoalValues Predators;
    SettingsValues Settings;
    EnvironmentValues Environment;
};
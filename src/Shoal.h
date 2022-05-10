#pragma once

#include <vector>

#include "Mesh.h"
#include "SimulationValues.h"

#include "cuda_gl_interop.h"

class Shoal
{
private:
    struct cudaGraphicsResource *cuda_pos;
    struct cudaGraphicsResource *cuda_vel;

public:
    Mesh *FishMesh;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    unsigned int positionsVBO, velocitiesVBO;
    glm::vec3 *d_positions;
    glm::vec3 *d_velocities;
    void Draw(int NumberOfFish, bool UpdateVectors);
    Shoal(const char *name, int numberOfFrames, float framesPerSecond, unsigned int ShaderID,
          int NumberOfBoids, int MaxNumberOfBoids, int SizeOfContainer);
    ~Shoal();
    void UpdateCudaBuffers();
};
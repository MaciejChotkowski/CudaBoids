#pragma once

#include "SimulationValues.h"

#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cuda_gl_interop.h>

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Compute
{
    void init(int preyNumber, int predatorsNumber);
    void free();
    void ComputeWithCPU(std::vector<glm::vec3> *pos, std::vector<glm::vec3> *vel, float dt, SimulationValues *values);
    void ComputeWithCUDA(glm::vec3 *PreyPos, glm::vec3 *PreyVel, glm::vec3 *PredPos, glm::vec3 *PredVel,
                         float dt, SimulationValues *values);

    const int threadsPerBlock = 32 * 8;
}
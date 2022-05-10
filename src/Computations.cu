#include "Computations.cuh"
glm::vec3 *d_HelperVelocitiesPrey;
glm::vec3 *d_HelperVelocitiesPredators;
SimulationValues *d_values;

void Compute::init(int preyNumber, int predatorsNumber)
{
    cudaMalloc((void **)&d_HelperVelocitiesPrey, preyNumber * sizeof(glm::vec3));
    cudaMalloc((void **)&d_HelperVelocitiesPredators, predatorsNumber * sizeof(glm::vec3));
    cudaMalloc((void **)&d_values, sizeof(SimulationValues));
}
void Compute::free()
{
    cudaFree(d_HelperVelocitiesPrey);
    cudaFree(d_HelperVelocitiesPredators);
    cudaFree(d_values);
}

// Applies the principles of flocking behavior to a given shoal,
// defined as set of positions and vectors.
__device__ void ComputeFlockingBehaviorVelocities(glm::vec3 *positions, glm::vec3 *velocities, glm::vec3 *helperVelocities,
                                                  SpecificShoalValues *values, int id, float dt)
{
    glm::vec3 averageVelocity = glm::vec3(0, 0, 0);
    glm::vec3 averagePosition = glm::vec3(0, 0, 0);
    glm::vec3 smallAveragePosition = glm::vec3(0, 0, 0);
    int count = 0;
    int smallCount = 0;
    for (int i = 0; i < values->Number; i++)
    {
        float distance = glm::distance(positions[id], positions[i]);
        if (id != i && distance < values->Perception.BigPerceptionRadius)
        {
            averageVelocity += velocities[i];
            averagePosition += positions[i];
            count++;
            if (distance < values->Perception.SmallPerceptionRadius)
            {
                smallAveragePosition += positions[i];
                smallCount++;
            }
        }
    }

    helperVelocities[id] = velocities[id];
    if (count > 0)
    {
        averageVelocity /= count;
        averagePosition /= count;
        glm::vec3 SteeringAlignment = averageVelocity - velocities[id];
        glm::vec3 SteeringCohesion = averagePosition - positions[id];
        SteeringAlignment = glm::normalize(SteeringAlignment);
        SteeringAlignment *= values->Forces.AlignmentForce;
        SteeringCohesion = glm::normalize(SteeringCohesion);
        SteeringCohesion *= values->Forces.CohesionForce;
        helperVelocities[id] += SteeringAlignment * dt + SteeringCohesion * dt;
    }
    if (smallCount > 0)
    {
        smallAveragePosition /= smallCount;
        glm::vec3 SteeringSeparation = positions[id] - smallAveragePosition;
        SteeringSeparation = glm::normalize(SteeringSeparation);
        SteeringSeparation *= values->Forces.SeparationForce;
        helperVelocities[id] += SteeringSeparation * dt;
    }

    helperVelocities[id] = glm::normalize(helperVelocities[id]);
    helperVelocities[id] *= values->Speed;
}

// Used to make smaller type of fish follow worm when it's visible.
__device__ void ComputePreyHungerVelocities(glm::vec3 *positions, glm::vec3 *helperVelocities,
                                            SimulationValues *values, int id, float dt)
{
    if (values->Settings.CursorVisible)
    {
        if (glm::distance(positions[id], values->Environment.CursorPosition) <= values->Prey.Perception.FoodPerceptionRadius)
        {
            glm::vec3 SteeringHunger = values->Environment.CursorPosition - positions[id];
            SteeringHunger = glm::normalize(SteeringHunger);
            SteeringHunger *= values->Prey.Forces.HungerForce;
            helperVelocities[id] += SteeringHunger * dt;
        }
    }
}

// Used to make smaller fish escape from bigger fish when in proximity.
__device__ void ComputePreyFearVelocities(glm::vec3 *positions, glm::vec3 *helperVelocities,
                                          SimulationValues *values, int id, float dt, glm::vec3 *predatorPositions)
{
    glm::vec3 averagePosition = glm::vec3(0, 0, 0);
    int count = 0;
    for (int i = 0; i < values->Predators.Number; i++)
    {
        float distance = glm::distance(positions[id], predatorPositions[i]);
        {
            if (distance < values->Prey.Perception.DangerPerceptionRadius)
            {
                averagePosition += predatorPositions[i];
                count++;
            }
        }
    }
    if (count > 0)
    {
        averagePosition /= count;
        glm::vec3 SteeringFear = positions[id] - averagePosition;
        SteeringFear = glm::normalize(SteeringFear);
        SteeringFear *= values->Prey.Forces.FearForce;
        helperVelocities[id] += SteeringFear * dt;
    }
}

// Used to make bigger fish chase smaller fish when in proximity.
__device__ void ComputePredatorHungerVelocities(glm::vec3 *positions, glm::vec3 *helperVelocities,
                                                SimulationValues *values, int id, float dt, glm::vec3 *preyPositions)
{
    glm::vec3 averagePosition = glm::vec3(0, 0, 0);
    int count = 0;
    for (int i = 0; i < values->Prey.Number; i++)
    {
        float distance = glm::distance(positions[id], preyPositions[i]);
        {
            if (distance < values->Predators.Perception.FoodPerceptionRadius)
            {
                averagePosition += preyPositions[i];
                count++;
            }
        }
    }
    if (count > 0)
    {
        averagePosition /= count;
        glm::vec3 SteeringHunger = averagePosition - positions[id];
        SteeringHunger = glm::normalize(SteeringHunger);
        SteeringHunger *= values->Predators.Forces.HungerForce;
        helperVelocities[id] += SteeringHunger * dt;
    }
}

// Applies all types of behaviors to smaller fish.
__global__ void ComputePreyVelocities(glm::vec3 *positions, glm::vec3 *velocities, glm::vec3 *helperVelocities,
                                      float dt, SimulationValues *values, glm::vec3 *predatorPositions)
{
    int id = threadIdx.x + blockIdx.x * blockDim.x;
    if (id < values->Prey.Number)
    {
        ComputeFlockingBehaviorVelocities(positions, velocities, helperVelocities, &values->Prey, id, dt);
        ComputePreyHungerVelocities(positions, helperVelocities, values, id, dt);
        ComputePreyFearVelocities(positions, helperVelocities, values, id, dt, predatorPositions);
    }
}

// Applies all types of behaviors to bigger fish.
__global__ void ComputePredatorsVelocities(glm::vec3 *positions, glm::vec3 *velocities, glm::vec3 *helperVelocities,
                                           float dt, SimulationValues *values, glm::vec3 *preyPositions)
{
    int id = threadIdx.x + blockIdx.x * blockDim.x;
    if (id < values->Predators.Number)
    {
        ComputeFlockingBehaviorVelocities(positions, velocities, helperVelocities, &values->Predators, id, dt);
        ComputePredatorHungerVelocities(positions, helperVelocities, values, id, dt, preyPositions);
    }
}

// After calculating the velocities this kernel copies them from temporary vector and adds the
// to current position. Next, container bounds are checked.
__global__ void SetPositions(glm::vec3 *positions, glm::vec3 *velocities, glm::vec3 *helperVelocities,
                             SpecificShoalValues *shoalValues, float dt, SimulationValues *values)
{
    int id = threadIdx.x + blockIdx.x * blockDim.x;
    if (id < shoalValues->Number)
    {
        helperVelocities[id] = glm::normalize(helperVelocities[id]);
        helperVelocities[id] *= shoalValues->Speed;
        velocities[id] = helperVelocities[id];
        positions[id] += helperVelocities[id] * dt;

        if (values->Settings.SphericalContainer)
        {
            if (glm::length(positions[id]) > values->Environment.SizeOfContainer / 2)
            {
                positions[id] = glm::normalize(positions[id]);
                positions[id] *= -values->Environment.SizeOfContainer / 2;
            }
        }
        else
        {
            if (positions[id].x < -values->Environment.SizeOfContainer / 2)
                positions[id].x = values->Environment.SizeOfContainer / 2;
            if (positions[id].x > values->Environment.SizeOfContainer / 2)
                positions[id].x = -values->Environment.SizeOfContainer / 2;
            if (positions[id].y < -values->Environment.SizeOfContainer / 2)
                positions[id].y = values->Environment.SizeOfContainer / 2;
            if (positions[id].y > values->Environment.SizeOfContainer / 2)
                positions[id].y = -values->Environment.SizeOfContainer / 2;
            if (positions[id].z < -values->Environment.SizeOfContainer / 4)
                positions[id].z = values->Environment.SizeOfContainer / 4;
            if (positions[id].z > values->Environment.SizeOfContainer / 4)
                positions[id].z = -values->Environment.SizeOfContainer / 4;
        }
    }
}

// All types of functionality computed concurrently on GPU.
void Compute::ComputeWithCUDA(glm::vec3 *PreyPos, glm::vec3 *PreyVel, glm::vec3 *PredPos, glm::vec3 *PredVel,
                              float dt, SimulationValues *values)
{
    cudaMemcpy(d_values, values, sizeof(SimulationValues), cudaMemcpyHostToDevice);

    int numberOfPreyBlocks = values->Prey.Number / threadsPerBlock + 1;
    int numberOfPredatorsBlocks = values->Predators.Number / threadsPerBlock + 1;

    ComputePreyVelocities<<<numberOfPreyBlocks, threadsPerBlock>>>(PreyPos, PreyVel, d_HelperVelocitiesPrey, dt, d_values, PredPos);
    ComputePredatorsVelocities<<<numberOfPredatorsBlocks, threadsPerBlock>>>(PredPos, PredVel, d_HelperVelocitiesPredators, dt, d_values, PreyPos);
    cudaDeviceSynchronize();

    SetPositions<<<numberOfPreyBlocks, threadsPerBlock>>>(PreyPos, PreyVel, d_HelperVelocitiesPrey, &d_values->Prey, dt, d_values);
    SetPositions<<<numberOfPredatorsBlocks, threadsPerBlock>>>(PredPos, PredVel, d_HelperVelocitiesPredators, &d_values->Predators, dt, d_values);
    cudaDeviceSynchronize();
}

// All types of functionality computed sequentially on CPU.
void Compute::ComputeWithCPU(std::vector<glm::vec3> *pos, std::vector<glm::vec3> *vel, float dt, SimulationValues *values)
{
    for (int i = 0; i < values->Prey.Number; i++)
    {
        int count = 0;
        int smallCount = 0;
        glm::vec3 avgVel = glm::vec3(0, 0, 0);
        glm::vec3 avgPos = glm::vec3(0, 0, 0);
        glm::vec3 smallAvgPos = glm::vec3(0, 0, 0);
        for (int j = 0; j < values->Prey.Number; j++)
        {
            float distance = glm::distance((*pos)[i], (*pos)[j]);
            if (i != j && distance < values->Prey.Perception.BigPerceptionRadius)
            {
                count++;
                avgVel += (*vel)[j];
                avgPos += (*vel)[j];
                if (distance < values->Prey.Perception.BigPerceptionRadius)
                {
                    smallCount++;
                    smallAvgPos += (*pos)[j];
                }
            }
        }
        if (count > 0)
        {
            avgVel /= count;
            glm::vec3 SteeringAlignment = avgVel - (*vel)[i];
            SteeringAlignment = glm::normalize(SteeringAlignment);
            SteeringAlignment *= values->Prey.Forces.AlignmentForce;

            avgPos /= count;
            glm::vec3 SteeringCohesion = avgPos - (*pos)[i];
            SteeringCohesion = glm::normalize(SteeringCohesion);
            SteeringCohesion *= values->Prey.Forces.CohesionForce;

            (*vel)[i] += SteeringCohesion * dt;
            (*vel)[i] += SteeringAlignment * dt;
        }
        if (smallCount > 0)
        {
            smallAvgPos /= smallCount;
            glm::vec3 SteeringSeparation = (*pos)[i] - smallAvgPos;
            SteeringSeparation = glm::normalize(SteeringSeparation);
            SteeringSeparation *= values->Prey.Forces.SeparationForce;
            (*vel)[i] += SteeringSeparation * dt;
        }

        (*vel)[i] = glm::normalize((*vel)[i]);
        (*vel)[i] *= values->Prey.Speed;

        (*pos)[i] += (*vel)[i] * dt;
        if ((*pos)[i].x < -values->Environment.SizeOfContainer / 2)
            (*pos)[i].x = values->Environment.SizeOfContainer / 2;
        if ((*pos)[i].x > values->Environment.SizeOfContainer / 2)
            (*pos)[i].x = -values->Environment.SizeOfContainer / 2;
        if ((*pos)[i].y < -values->Environment.SizeOfContainer / 2)
            (*pos)[i].y = values->Environment.SizeOfContainer / 2;
        if ((*pos)[i].y > values->Environment.SizeOfContainer / 2)
            (*pos)[i].y = -values->Environment.SizeOfContainer / 2;
        if ((*pos)[i].z < -values->Environment.SizeOfContainer / 4)
            (*pos)[i].z = values->Environment.SizeOfContainer / 4;
        if ((*pos)[i].z > values->Environment.SizeOfContainer / 4)
            (*pos)[i].z = -values->Environment.SizeOfContainer / 4;
    }
}
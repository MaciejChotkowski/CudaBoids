#include "Shoal.h"

// This class is a comfortable wrapper for a mesh that is meant for instanced rendering.
// It holds all information about a single shoal and pointers to its vectors on GPU.
// Correct structure of object directories is required.
Shoal::Shoal(const char *name, int numberOfFrames, float framesPerSecond, unsigned int ShaderID,
             int NumberOfBoids, int MaxNumberOfBoids, int SizeOfContainer)
{
    std::vector<glm::vec3> colors;

    std::string animationPath = "obj/" + std::string(name) + "/anim/" + name;
    std::string texturePath = "obj/" + std::string(name) + "/" + std::string(name) + ".png";
    FishMesh = new Mesh(animationPath.c_str(), ShaderID, numberOfFrames, framesPerSecond);
    FishMesh->MakeInstanced(NumberOfBoids);
    FishMesh->LoadTextureFromFile(texturePath.c_str());

    for (int i = 0; i < MaxNumberOfBoids; i++)
    {
        positions.push_back(glm::vec3(rand() % SizeOfContainer - SizeOfContainer / 2, rand() % SizeOfContainer - SizeOfContainer / 2, rand() % SizeOfContainer - SizeOfContainer / 2));
        velocities.push_back(glm::vec3(rand() % 10000 - 5000, rand() % 10000 - 5000, rand() % 10000 - 5000));
        colors.push_back(glm::vec3((float)(rand() % 400 + 300) / 1000, (float)(rand() % 400 + 300) / 1000, (float)(rand() % 400 + 300) / 1000));
    }

    unsigned int col;
    glGenBuffers(1, &col);
    glBindBuffer(GL_ARRAY_BUFFER, col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MaxNumberOfBoids, &colors[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, col);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(3, 1);

    glGenBuffers(1, &positionsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MaxNumberOfBoids, &positions[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(4, 1);

    glGenBuffers(1, &velocitiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, velocitiesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MaxNumberOfBoids, &velocities[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, velocitiesVBO);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(5, 1);

    UpdateCudaBuffers();
}

void Shoal::UpdateCudaBuffers()
{
    cudaGraphicsGLRegisterBuffer(&cuda_pos, positionsVBO, cudaGraphicsMapFlagsWriteDiscard);
    cudaGraphicsGLRegisterBuffer(&cuda_vel, velocitiesVBO, cudaGraphicsMapFlagsWriteDiscard);
    size_t num_bytes;
    cudaGraphicsMapResources(1, &cuda_pos, 0);
    cudaGraphicsResourceGetMappedPointer((void **)&d_positions, &num_bytes, cuda_pos);
    cudaGraphicsMapResources(1, &cuda_vel, 0);
    cudaGraphicsResourceGetMappedPointer((void **)&d_velocities, &num_bytes, cuda_vel);
}

Shoal::~Shoal()
{
    delete FishMesh;
}

void Shoal::Draw(int NumberOfFish, bool UpdateVectors)
{
    if (UpdateVectors)
    {
        glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * NumberOfFish, &positions[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, velocitiesVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * NumberOfFish, &velocities[0], GL_STATIC_DRAW);
    }

    FishMesh->Draw();
}
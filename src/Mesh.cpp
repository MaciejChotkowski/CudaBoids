#include "Mesh.h"

// Constructor for static, non-animated meshes.
Mesh::Mesh(const char *PathToFiles, unsigned int shaderProgramID, int animationFrames, float framesPerSecond)
{
    AnimationFrames = animationFrames;
    FrameRate = 1.0f / framesPerSecond;
    ShaderProgramID = shaderProgramID;

    for (int i = 1; i <= animationFrames; i++)
    {
        std::vector<float> vs;
        std::vector<int> is;
        std::ostringstream ss;
        ss << std::setw(6) << std::setfill('0') << i;
        std::string s = PathToFiles + std::string("_") + ss.str() + ".obj";
        Loader::LoadObject(s.c_str(), &vs, &is);
        vertices.push_back(vs);
        indices.push_back(is);
    }

    Start();
}

// Constructor for animated meshes.
Mesh::Mesh(const char *PathToFile, unsigned int shaderProgramID)
{
    AnimationFrames = 0;
    FrameRate = 1;
    ShaderProgramID = shaderProgramID;

    std::vector<float> vs;
    std::vector<int> is;
    std::string path = PathToFile + std::string(".obj");
    Loader::LoadObject(path.c_str(), &vs, &is);
    vertices.push_back(vs);
    indices.push_back(is);

    Start();
}

void Mesh::Start()
{
    Scale = 1;
    InstanceCount = 0;
    Frame = 0;
    lastTime = 0;
    Position = glm::vec3(0, 0, 0);
    Pitch = 0;
    Yaw = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices[0].size(), vertices[0].data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices[0].size(), indices[0].data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Draw(glm::mat4 model)
{
    glUseProgram(ShaderProgramID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    glBindVertexArray(VAO);

    if (AnimationFrames > 0 && glfwGetTime() - lastTime > FrameRate)
    {
        lastTime = glfwGetTime();
        Frame++;
        if (Frame > AnimationFrames - 1)
            Frame = 0;
        setFrame(Frame, ShaderProgramID);
    }

    model = glm::translate(model, Position);
    model = glm::scale(model, glm::vec3(Scale, Scale, Scale));
    glUseProgram(ShaderProgramID);
    glUniformMatrix4fv(glGetUniformLocation(ShaderProgramID, "model"), 1, GL_FALSE, &model[0][0]);
    if (InstanceCount > 0)
    {
        glDrawElementsInstanced(GL_TRIANGLES, indices[Frame].size(), GL_UNSIGNED_INT, 0, InstanceCount);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, indices[Frame].size(), GL_UNSIGNED_INT, 0);
    }
}

void Mesh::LoadTextureFromFile(const char *PathToFile)
{
    TextureID = Loader::LoadTexture(PathToFile, ShaderProgramID);
}

// Changes current mesh to a given frame.
void Mesh::setFrame(int f, int shader)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices[Frame].size(), vertices[Frame].data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices[Frame].size(), indices[Frame].data(), GL_STATIC_DRAW);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::setPosition(glm::vec3 pos)
{
    Position = pos;
}

void Mesh::Move(glm::vec3 v)
{
    Position += v;
}

void Mesh::MakeInstanced(int instanceCount)
{
    InstanceCount = instanceCount;
}

void Mesh::setRotation(float pitch, float yaw)
{
    Pitch = pitch;
    Yaw = yaw;
}

void Mesh::setScale(float scale)
{
    Scale = scale;
}

glm::vec3 Mesh::getPosition()
{
    return Position;
}
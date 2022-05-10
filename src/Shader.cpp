#include "Shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    unsigned int vertexID = LoadShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragmentID = LoadShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);

    ShaderID = glCreateProgram();
    glAttachShader(ShaderID, vertexID);
    glAttachShader(ShaderID, fragmentID);
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
    glLinkProgram(ShaderID);
}

Shader::~Shader()
{
    glDeleteProgram(ShaderID);
}

unsigned int Shader::LoadShaderFromFile(const char *pathToFile, GLenum typeOfShader)
{
    std::string Code;
    std::ifstream File;
    File.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        File.open(pathToFile);
        std::stringstream Stream;
        Stream << File.rdbuf();
        File.close();
        Code = Stream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "Shader error" << std::endl;
    }
    const char *CodeChar = Code.c_str();
    unsigned int id = glCreateShader(typeOfShader);
    glShaderSource(id, 1, &CodeChar, NULL);
    glCompileShader(id);
    return id;
}

// Adds a uniform 4x4 matrix to shader.
void Shader::addMat4(const char *name, glm::mat4 &data)
{
    glUseProgram(ShaderID);
    glUniformMatrix4fv(glGetUniformLocation(ShaderID, name), 1, GL_FALSE, &data[0][0]);
}

// Adds a uniform 3-dimensional vector to shader.
void Shader::addVec3(const char *name, const glm::vec3 &data)
{
    glUseProgram(ShaderID);
    glUniform3fv(glGetUniformLocation(ShaderID, name), 1, &data[0]);
}

unsigned int Shader::GetShaderID()
{
    return ShaderID;
}
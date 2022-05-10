#include "Loader.h"

void Loader::LoadObject(const char *pathToFile, std::vector<float> *vertices, std::vector<int> *indices)
{
    std::cout << "Loading object: " << pathToFile << std::endl;
    vertices->clear();
    indices->clear();
    std::ifstream file(pathToFile);
    if (!file.is_open())
    {
        std::cout << "Couldn't open " << pathToFile << std::endl;
        return;
    }
    std::string line;
    std::vector<glm::vec3> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec3> vn;
    std::vector<glm::vec3> in;

    float tmpX, tmpY, tmpZ;

    while (std::getline(file, line))
    {
        std::istringstream stream(line);
        std::string string;
        stream >> string;

        if (string == "v")
        {
            stream >> tmpX >> tmpY >> tmpZ;
            v.push_back(glm::vec3(tmpX, tmpY, tmpZ));
        }
        else if (string == "vt")
        {
            stream >> tmpX >> tmpY;
            vt.push_back(glm::vec2(tmpX, tmpY));
        }
        else if (string == "vn")
        {
            stream >> tmpX >> tmpY >> tmpZ;
            vn.push_back(glm::vec3(tmpX, tmpY, tmpZ));
        }
        else if (string == "f")
        {
            for (int i = 0; i < 3; i++)
            {
                std::string tmp;
                stream >> tmp;
                int first = tmp.find("/");
                int last = tmp.rfind("/");
                std::string s1 = tmp.substr(0, first);
                std::string s2 = tmp.substr(first + 1, last - first - 1);
                std::string s3 = tmp.substr(last + 1);
                int j = 0;
                glm::vec3 tmp_in(glm::vec3(atoi(s1.c_str()) - 1, atoi(s2.c_str()) - 1, atoi(s3.c_str()) - 1));
                for (j = 0; j < in.size(); j++)
                {
                    if (in[j] == tmp_in)
                    {
                        break;
                    }
                }
                if (j == in.size())
                {
                    in.push_back(tmp_in);
                }
                indices->push_back(j);
            }
        }
    }
    file.close();
    for (auto it = in.begin(); it != in.end(); it++)
    {
        glm::vec3 threeIndexValues = *it;
        int vIndex = threeIndexValues.x;
        int vtIndex = threeIndexValues.y;
        int vnIndex = threeIndexValues.z;

        vertices->push_back(v[vIndex].x);
        vertices->push_back(v[vIndex].y);
        vertices->push_back(v[vIndex].z);

        vertices->push_back(vt[vtIndex].x);
        vertices->push_back(vt[vtIndex].y);

        vertices->push_back(vn[vnIndex].x);
        vertices->push_back(vn[vnIndex].y);
        vertices->push_back(vn[vnIndex].z);
    }
}

unsigned int Loader::LoadTexture(const char *pathToFile, unsigned int shaderProgramID)
{
    unsigned int textureID;
    std::cout << "Loading texture: " << pathToFile << std::endl;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(pathToFile, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glUseProgram(shaderProgramID);
    glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
    return textureID;
}
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Camera
{
private:
    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 Right;
    glm::vec3 Up;
    glm::vec3 WorldUp;
    glm::vec3 Front;
    glm::mat4 view;
    float lastX, lastY;
    float yaw, pitch;
    bool RotationIsEnabled;
    bool firstMouse;
    int Speed;

public:
    Camera();
    glm::mat4 getView();
    void ProcessMouseMovement(double xpos, double ypos);
    void Move(GLFWwindow *window, float dt);
    void SwitchCameraRotation(bool onOrOff);
    void SetPosition(glm::vec3 position);
    glm::vec3 getInFrontPosition(double distance = 0);
    glm::vec3 getPosition();
};
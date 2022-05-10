#include "Camera.h"

Camera::Camera()
{
    RotationIsEnabled = true;
    Position = glm::vec3(0, 0, -1100);
    lastX = 800;
    lastY = 450;
    yaw = 90;
    pitch = 0;
    WorldUp = glm::vec3(0, 1, 0);
    Up = WorldUp;
    Right = glm::vec3(1, 0, 0);
    Front = glm::vec3(0, 0, 1);
    firstMouse = true;
    Speed = 500;
}

// Calculates the matrix necessary for view space transformation.
glm::mat4 Camera::getView()
{
    return glm::lookAt(Position, Position + Front, Up);
}

// Rotates camera based on received mouse movement.
void Camera::ProcessMouseMovement(double xpos, double ypos)
{
    if (RotationIsEnabled)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
}

void Camera::Move(GLFWwindow *window, float dt)
{
    glm::vec3 v = glm::vec3(0, 0, 0);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        v += glm::vec3(0, 0, 1);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        v += glm::vec3(0, 0, -1);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        v += glm::vec3(1, 0, 0);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        v += glm::vec3(-1, 0, 0);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        v += glm::vec3(0, 1, 0);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        v += glm::vec3(0, -1, 0);

    glm::mat4 rotMat(1);
    rotMat = glm::rotate(rotMat, glm::radians(yaw - 90), glm::vec3(0, 1, 0));
    glm::vec4 tmp = glm::vec4(v, 1);
    tmp = tmp * rotMat;
    glm::vec3 newV = glm::vec3(tmp.x, tmp.y, tmp.z);
    newV *= Speed;
    Position = Position + newV * dt;
}

void Camera::SwitchCameraRotation(bool onOrOff)
{
    RotationIsEnabled = onOrOff;
    if (RotationIsEnabled == true)
    {
        firstMouse = true;
    }
}

void Camera::SetPosition(glm::vec3 position)
{
    Position = -position;
}

// Returns vector of position that's directly in front of camera in given distance.
glm::vec3 Camera::getInFrontPosition(double distance)
{
    glm::vec3 offset = Front;
    offset *= distance;
    return Position + offset;
}

glm::vec3 Camera::getPosition()
{
    return Position;
}
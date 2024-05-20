#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Camera 
{
public:
    Camera(GLFWwindow* window, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : window(window), position(position), up(up), yaw(yaw), pitch(pitch), front(glm::vec3(0.0f, 0.0f, -1.0f)),
        movementSpeed(2.5f), sensitivity(0.1f) 
    {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    void processKeyboard(float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= right * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += right * velocity;
    }


    void processMouseMovement(GLFWwindow* window)
    {
        if (mouseDown == false)
        {
            return;
        }
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (firstMouse) {
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            firstMouse = false;
        }

        float xoffset = static_cast<float>(xpos) - lastX;
        lastX = static_cast<float>(xpos);

        xoffset *= sensitivity;

        yaw += xoffset;

        // Constrain yaw to keep it within bounds
        if (yaw > 360.0f) yaw -= 360.0f;
        if (yaw < 0.0f) yaw += 360.0f;

        updateCameraVectors();
    }

    void SetMouseDown(bool isDown)
    {
        mouseDown = isDown;
    }

private:
    GLFWwindow* window;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float yaw;
    float pitch;
    float movementSpeed;
    float lastX = 400.0f;
    float lastY = 300.0f;
    float sensitivity = 0.1f;
    bool firstMouse = false;
    bool mouseDown;

    glm::vec2 dragStart;

    void updateCameraVectors() 
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);
        right = glm::normalize(glm::cross(front, up));
        up = glm::normalize(glm::cross(right, front));
    }
};
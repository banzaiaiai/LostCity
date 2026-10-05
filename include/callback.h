#pragma once

#include "camera.h"

struct GLFWwindow;

struct AppState {
    Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
    float lastX = 400.0f;
    float lastY = 300.0f;
    bool  firstMouse = true;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};

namespace Callback {
    void processInput(GLFWwindow* window, AppState& state); // appelée à la main dans la boucle
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
}

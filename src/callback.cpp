#include "callback.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace {
    AppState& getState(GLFWwindow* window) {
        return *static_cast<AppState*>(glfwGetWindowUserPointer(window));
    }
}

namespace Callback {

void processInput(GLFWwindow* window, AppState& s) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        s.camera.ProcessKeyboard(FORWARD, s.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        s.camera.ProcessKeyboard(BACKWARD, s.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        s.camera.ProcessKeyboard(LEFT, s.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        s.camera.ProcessKeyboard(RIGHT, s.deltaTime);
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    AppState& s = getState(window);
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (s.firstMouse) {
        s.lastX = xpos;
        s.lastY = ypos;
        s.firstMouse = false;
    }
    float xoffset = xpos - s.lastX;
    float yoffset = s.lastY - ypos;
    s.lastX = xpos;
    s.lastY = ypos;

    s.camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double, double yoffset) {
    getState(window).camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

} // namespace Callback

#ifndef APPSTATE_H
#define APPSTATE_H

#include "camera.h"
#include "glm/ext/vector_float3.hpp"
#include <string>
#include <vector>

struct GLFWwindow;

struct AppState {
	bool gammaKeyPressed{false};
	bool gammaEnabled{false};
	glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
	Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
	float lastX{400.0f};
	float lastY{300.0f};
	bool firstMouse{true};
	float deltaTime{0.0f};
	float lastFrame{0.0f};
	GLFWwindow *window{nullptr};
	const unsigned int SCR_WIDTH{800};
	const unsigned int SCR_HEIGHT{600};
	bool blin{true};
};

namespace Callback {
extern AppState state;
void processInput(GLFWwindow *window,
                  AppState &state);  // appelée à la main dans la boucle
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path, bool state = false);
unsigned int loadCubemap(std::vector<std::string> faces);
}  // namespace Callback
#endif  // !APPSTATE_H

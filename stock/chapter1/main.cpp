#include "VAO.h"
#include "VBO.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "texture.h"
#include <cmath>
#include <glad/glad.h>  // AVANT glfw

#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// image loader
#define STB_IMAGE_IMPLEMENTATION
#include "callback.h"
#include "camera.h"
#include "shader.h"
#include "stb_image.h"

#ifndef SHADER_DIR
#define SHADER_DIR "shaders/"
#endif  // SHADER_DIR
// Set globa variables
// size window
const unsigned int SRC_WIDTH = 800;
const unsigned int SRC_HEIGHT = 600;

GLFWwindow *window;

int init(AppState state);

// ---------- Main ----------
int main() {

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// Creation window
	AppState state;
	state.lastX = SRC_WIDTH / 2.0f;
	state.lastY = SRC_HEIGHT / 2.0f;
	window =
	    glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "OpenGL", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glfwSetWindowUserPointer(window, &state);
	glfwSetFramebufferSizeCallback(window, Callback::framebuffer_size_callback);
	glfwSetCursorPosCallback(window, Callback::mouse_callback);
	glfwSetScrollCallback(window, Callback::scroll_callback);

	// get input of mouse
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Échec de GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	// configure global openglState
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprgram
	Shader ourShader(SHADER_DIR "basic.vert", SHADER_DIR "basic.frag");

	// set up the vertices
	float vertices[] = {
	    // positions          // texture coords
	    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
	    0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
	    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
	    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
	    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
	    -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,
	    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
	    0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
	    0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
	    0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
	    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
	    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
	glm::vec3 cubePositions[] = {
	    glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
	    glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
	    glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
	    glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
	    glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

	// VAO & VBO init
	VAO vao;
	VBO vbo;
	vao.load();
	vao.bind();
	vbo.load(vertices, sizeof(vertices));

	const unsigned int stride = 5 * sizeof(float);
	vao.setAttribute(0, 3, stride, 0);
	vao.setAttribute(1, 2, stride, 3 * sizeof(float));

	// loader texture
	Texture texture1{};
	texture1.load("container.jpg", true);
	// loader texture
	Texture texture2{};
	texture2.load("awesomeface.png", true);

	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		state.deltaTime = currentFrame - state.lastFrame;
		state.lastFrame = currentFrame;

		Callback::processInput(window, state);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		texture1.use(0);
		texture2.use(1);

		ourShader.use();

		// pass projection matrix to shader (note that in this case it could
		// change every frame)
		glm::mat4 projection = glm::perspective(
		    glm::radians(state.camera.Zoom),
		    (float)SRC_WIDTH / (float)SRC_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = state.camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		vao.bind();
		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(
			    model, (float)glfwGetTime() * glm::radians((i + 1) * 50.0f),
			    glm::vec3(1.0f, 0.3f, 0.5f));
			ourShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

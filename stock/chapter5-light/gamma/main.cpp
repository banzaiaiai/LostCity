#include "VAO.h"
#include "VBO.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iterator>
#include <map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <callback.h>
#include <camera.h>
#include <filesystem.h>
#include <model.h>
#include <shader.h>

#include <iostream>
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool blinn = true;

GLFWwindow *window;
int main() {
	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	Callback::state.lastX = SCR_WIDTH / 2.0f;
	Callback::state.lastY = SCR_HEIGHT / 2.0f;
	glfwSetWindowUserPointer(window, &Callback::state);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, Callback::framebuffer_size_callback);
	glfwSetCursorPosCallback(window, Callback::mouse_callback);
	glfwSetScrollCallback(window, Callback::scroll_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl Callback::state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,
	            GL_ONE_MINUS_SRC_ALPHA);  // build and compile shaders
	// -------------------------
	Shader shader(SHADER_DIR "1.1.depth_testing.vs",
	              SHADER_DIR "1.1.depth_testing.fs");

	// Shader shaderSingleColor(SHADER_DIR"1.1.depth_testing.vs",
	// SHADER_DIR"2.stencil_single_color.fs");

	// set up the vertices
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// clang-format off
    float planeVertices[] = {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
	// clang-format on
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
	             GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
	                      (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
	                      (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
	                      (void *)(6 * sizeof(float)));
	glBindVertexArray(0);

	// load textures
	// -------------
	// load textures
	// -------------

	unsigned int floorTexture = Callback::loadTexture(
	    FileSystem::getPath("resources/textures/wood.png").c_str(), false);
	unsigned int floorTextureGammaCorrected = Callback::loadTexture(
	    FileSystem::getPath("resources/textures/wood.png").c_str(), true);

	// shader configuration
	// --------------------
	// load cube texture
	// --------------------
	// --------------------
	shader.use();
	shader.setInt("floorTexture", 0);

	// lighting info
	// -------------
	glm::vec3 lightPositions[] = {
	    glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
	    glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f)};
	glm::vec3 lightColors[] = {glm::vec3(0.25), glm::vec3(0.50),
	                           glm::vec3(0.75), glm::vec3(1.00)};
	std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		Callback::state.deltaTime = currentFrame - Callback::state.lastFrame;
		Callback::state.lastFrame = currentFrame;

		// input
		// -----
		Callback::processInput(window, Callback::state);

		// render
		// ------
		// first pass
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw scene as normal
		// glfw: swap buffers and poll IO events (keys pressed/released,
		// mouse moved etc.)
		// -------------------------------------------------------------------------------
		// draw planet

		// configure transformation matrices
		// configure transformation matrices
		shader.use();
		glm::mat4 projection = glm::perspective(
		    glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,
		    1000.0f);
		glm::mat4 view = Callback::state.camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		// set light uniforms
		glUniform3fv(glGetUniformLocation(shader.ID, "lightPositions"), 4,
		             &lightPositions[0][0]);
		glUniform3fv(glGetUniformLocation(shader.ID, "lightColors"), 4,
		             &lightColors[0][0]);
		shader.setVec3("viewPos", Callback::state.camera.Position);
		shader.setInt("gamma", Callback::state.gammaEnabled);
		// floor
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Callback::state.gammaEnabled
		                                 ? floorTextureGammaCorrected
										 : floorTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// draw elements

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glfwTerminate();

	return 0;
}

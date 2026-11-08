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
#ifndef SHADER_DIR
#define SHADER_DIR "shaders/"
#endif  // SHADER_DIR
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

	AppState state;
	state.lastX = SCR_WIDTH / 2.0f;
	state.lastY = SCR_HEIGHT / 2.0f;
	glfwSetWindowUserPointer(window, &state);
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

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader shaderAsteo(SHADER_DIR "1.1.depth_testing.vs",
	                   SHADER_DIR "1.1.depth_testing.fs");
	Shader shaderPlanet(SHADER_DIR "planet.vs", SHADER_DIR "planet.fs");

	// Shader shaderSingleColor(SHADER_DIR"1.1.depth_testing.vs",
	// SHADER_DIR"2.stencil_single_color.fs");

	// set up the vertices
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// clang-format off
	// clang-format on
	Model planet(FileSystem::getPath("resources/objects/planet/planet.obj"));
	Model rock(FileSystem::getPath("resources/objects/rock/rock.obj"));
	// asteoride matrix
	unsigned int amount = 10000;
	glm::mat4 *modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime());  // initialize random seed
	float radius = 50.0;
	float offset = 2.5f;
	for (unsigned int i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with radius [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement =
		    (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f;  // keep height of field smaller than x/z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));
		// 2. scale: scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));
		// 3. rotation: add random rotation around a (semi)random rotation axis
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}
	// vertex buffer object
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0],
	             GL_STATIC_DRAW);
	for (unsigned int i = 0; i < rock.meshes.size(); i++) {
		rock.meshes[i].Vao.bind();
		// vertex attributes
		std::size_t v4s = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void *)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * v4s,
		                      (void *)(1 * v4s));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * v4s,
		                      (void *)(2 * v4s));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * v4s,
		                      (void *)(3 * v4s));
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glBindVertexArray(0);
	}
	// load textures
	// -------------

	// load cube texture
	// --------------------
	// --------------------

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		state.deltaTime = currentFrame - state.lastFrame;
		state.lastFrame = currentFrame;

		// input
		// -----
		Callback::processInput(window, state);

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
		glm::mat4 projection = glm::perspective(
		    glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,
		    1000.0f);
		glm::mat4 view = state.camera.GetViewMatrix();
		shaderAsteo.use();
		shaderAsteo.setMat4("projection", projection);
		shaderAsteo.setMat4("view", view);
		shaderPlanet.use();
		shaderPlanet.setMat4("projection", projection);
		shaderPlanet.setMat4("view", view);  // draw meteorites
		                                     // draw planet
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		shaderPlanet.setMat4("model", model);
		planet.Draw(shaderPlanet);

		// draw meteorites
		shaderAsteo.use();
		shaderAsteo.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		if (rock.textures_loaded.empty()) {
			std::cerr
			    << "rock: aucune texture diffuse chargee, verifie rock.mtl"
			    << std::endl;
		} else {
			glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);
		}
		for (unsigned int i = 0; i < rock.meshes.size(); i++) {
			rock.meshes[i].Vao.bind();
			glDrawElementsInstanced(
			    GL_TRIANGLES,
			    static_cast<unsigned int>(rock.meshes[i].indices.size()),
			    GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	return 0;
}

#include "VAO.h"
#include "VBO.h"
#include <GLFW/glfw3.h>
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
	if (!glfwInit()) return -1;
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	// build and compile shaders
	// -------------------------
	Shader shader(SHADER_DIR "1.1.depth_testing.vs",
	              SHADER_DIR "1.1.depth_testing.fs");
	Shader screenShader(SHADER_DIR "quade.vert", SHADER_DIR "quade.frag");
	Shader skyboxShader(SHADER_DIR "cube.vert", SHADER_DIR "cube.frag");
	// Shader shaderSingleColor(SHADER_DIR"1.1.depth_testing.vs",
	// SHADER_DIR"2.stencil_single_color.fs");

	// set up the vertices
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
	    // back face
	    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  // bottom-left
	    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,    // top-right
	    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,   // bottom-right
	    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,    // top-right
	    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  // bottom-left
	    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,   // top-left
		                                  // front face
	    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   // bottom-left
	    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,    // bottom-right
	    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,     // top-right
	    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,     // top-right
	    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,    // top-left
	    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   // bottom-left
		                                  // left face
	    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    // top-right
	    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-left
	    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // bottom-left
	    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // bottom-left
	    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   // bottom-right
	    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    // top-right
		                                  // right face
	    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,     // top-left
	    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,   // bottom-right
	    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,    // top-right
	    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,   // bottom-right
	    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,     // top-left
	    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,    // bottom-left
		                                  // bottom face
	    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // top-right
	    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,   // top-left
	    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,    // bottom-left
	    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,    // bottom-left
	    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   // bottom-right
	    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // top-right
		                                  // top face
	    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,   // top-left
	    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,     // bottom-right
	    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,    // top-right
	    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,     // bottom-right
	    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,   // top-left
	    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f     // bottom-left
	};
	float planeVertices[] = {
	    // positions          // texture Coords (note we set these higher than 1
	    // (together with GL_REPEAT as texture wrapping mode). this will cause
	    // the floor texture to repeat)
	    5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
	    0.0f, 0.0f,  -5.0f, -0.5f, 5.0f,  0.0f,  2.0f,

	    5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  5.0f,  -0.5f, -5.0f,
	    0.0f, 2.0f,  -5.0f, -0.5f, -5.0f, 2.0f,  2.0f};
	float quadVertices[] = {
	    // vertex attributes for a quad that fills the entire screen in
	    // Normalized Device Coordinates. NOTE that this plane is now much
	    // smaller and at the top of the screen
	    // positions   // texCoords
	    -0.4f, 1.0f, 0.0f, 1.0f, -0.4f, 0.6f,
	    0.0f,  0.0f, 0.4f, 0.6f, 1.0f,  0.0f,

	    -0.4f, 1.0f, 0.0f, 1.0f, 0.4f,  0.6f,
	    1.0f,  0.0f, 0.4f, 1.0f, 1.0f,  1.0f};
	float transparentVertices[] = {
	    // positions         // texture Coords (swapped y coordinates because
	    // texture is flipped upside down)
	    0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -0.5f, 0.0f,
	    0.0f, 1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

	    0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 1.0f, -0.5f, 0.0f,
	    1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f};

	// VAO & VBO init for lightSpotShader
	VAO cubeVao;
	VBO cubeVbo;
	cubeVao.load();
	cubeVbo.load(vertices, sizeof(vertices));
	cubeVao.bind();
	cubeVbo.bind();
	int stride{5 * sizeof(float)};
	cubeVao.setAttribute(0, 3, stride, 0);
	cubeVao.setAttribute(1, 2, stride, 3 * sizeof(float));
	// plane VAO
	VAO planeVao;
	VBO planeVbo;
	planeVao.load();
	planeVbo.load(planeVertices, sizeof(planeVertices));
	planeVao.bind();
	planeVao.setAttribute(0, 3, stride, 0);
	planeVao.setAttribute(1, 2, stride, 3 * sizeof(float));
	// Grass VAO & VBO
	VAO grassVao;
	VBO grassVbo;
	grassVao.load();
	grassVbo.load(transparentVertices, sizeof(transparentVertices));
	grassVao.bind();
	grassVbo.bind();
	grassVao.setAttribute(0, 3, stride, 0);
	grassVao.setAttribute(1, 2, stride, 3 * sizeof(float));

	// grass
	// Position
	vector<glm::vec3> vegetation;
	vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));
	// Quad VAO
	VAO quadVao;
	VBO quadVbo;
	quadVao.load();
	quadVbo.load(quadVertices, sizeof(quadVertices));
	quadVao.bind();
	quadVbo.bind();
	stride = sizeof(float) * 4;
	quadVao.setAttribute(0, 2, stride, 0);
	quadVao.setAttribute(1, 2, stride, 2 * sizeof(float));
	// Skybox VAO
	VAO skykoxVao;
	VBO skyboxVbo;
	skykoxVao.load();
	skyboxVbo.load(skyBoxVertices, sizeof(skyBoxVertices));
	skykoxVao.bind();
	skyboxVbo.bind();
	stride = sizeof(float) * 3;
	quadVao.setAttribute(0, 3, stride, 0);
	// load textures
	// -------------
	unsigned int cubeTexture = Callback::loadTexture(
	    FileSystem::getPath("resources/textures/marble.jpg").c_str());
	unsigned int floorTexture = Callback::loadTexture(
	    FileSystem::getPath("resources/textures/metal.png").c_str());
	unsigned int grassTexture = Callback::loadTexture(
	    FileSystem::getPath("resources/textures/window.png").c_str());
	// load cube texture
	// --------------------
	vector<std::string> faces{
	    "ressources/skybox/right.jpg", "ressources/skybox/left.jpg",
	    "ressources/skybox/top.jpg",   "ressources/skybox/bottom.jpg",
	    "ressources/skybox/front.jpg", "ressources/skybox/back.jpg"};
	unsigned int cubemapTexture =
	    Callback::loadCubemap(faces);  // shader configuration
	// --------------------
	shader.use();
	shader.setInt("texture1", 0);

	screenShader.use();
	screenShader.setInt("screenTexture", 0);
	// draw in wireframe
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Framebuffer
	// Create Framebuffer
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create textture to bind to fbo
	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB,
	             GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
	                       textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't
	// be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH,
	                      SCR_HEIGHT);  // use a single renderbuffer object for
	                                    // both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
	                          GL_RENDERBUFFER, rbo);  // now actually attach it
	// now that we actually created the framebuffer and added all attachments we
	// want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw objects
		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		state.camera.Yaw +=
		    180.0f;  // rotate the camera's yaw 180 degrees around
		state.camera.ProcessMouseMovement(
		    0, 0,
		    false);  // call this to make sure it updates its camera vectors,
			         // note that we disable pitch constrains for this specific
			         // case (otherwise we can't reverse camera's pitch values)
		glm::mat4 view = state.camera.GetViewMatrix();
		state.camera.Yaw -=
		    180.0f;  // reset it back to its original orientation
		state.camera.ProcessMouseMovement(0, 0, true);
		glm::mat4 projection = glm::perspective(
		    glm::radians(state.camera.Zoom),
		    (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		// cubes
		cubeVao.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// floor
		planeVao.bind();
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		model = glm::mat4(1.0f);
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// vegetation
		std::map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < vegetation.size(); i++) {
			float distance = glm::length(state.camera.Position - vegetation[i]);
			sorted[distance] = vegetation[i];
		}
		grassVao.bind();
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin();
		     it != sorted.rend(); ++it) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glBindVertexArray(0);

		// second pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		model = glm::mat4(1.0f);
		view = state.camera.GetViewMatrix();
		shader.setMat4("view", view);

		// cubes
		cubeVao.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// floor
		planeVao.bind();
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		model = glm::mat4(1.0f);
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// vegetation
		for (unsigned int i = 0; i < vegetation.size(); i++) {
			float distance = glm::length(state.camera.Position - vegetation[i]);
			sorted[distance] = vegetation[i];
		}
		grassVao.bind();
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin();
		     it != sorted.rend(); ++it) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glBindVertexArray(0);

		glDisable(GL_DEPTH_TEST);
		screenShader.use();
		quadVao.bind();
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse
		// moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteFramebuffers(1, &fbo);
	glfwTerminate();

	return 0;
}

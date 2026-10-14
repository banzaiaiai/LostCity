#include "callback.h"
#include <stb_image.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>
#include <vector>

namespace {
AppState &getState(GLFWwindow *window) {
	return *static_cast<AppState *>(glfwGetWindowUserPointer(window));
}
}  // namespace

namespace Callback {

void processInput(GLFWwindow *window, AppState &s) {
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

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
	AppState &s = getState(window);
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

void scroll_callback(GLFWwindow *window, double, double yoffset) {
	getState(window).camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format = GL_RGB;  // valeur par défaut (sinon non initialisée)
		if (nrComponents == 1) format = GL_RED;
		else if (nrComponents == 3) format = GL_RGB;
		else if (nrComponents == 4) format = GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT,
		              1);  // évite une image déformée/crash si largeur*3 n'est
		                   // pas multiple de 4

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
		             GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(
		    GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		    format == GL_RGBA
		        ? GL_CLAMP_TO_EDGE
				: GL_REPEAT);  // for this tutorial: use GL_CLAMP_TO_EDGE to
				               // prevent semi-transparent borders. Due to
				               // interpolation it takes texels from next repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		                format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		                GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << path << " : "
		          << stbi_failure_reason() << std::endl;
	}
	return textureID;
}
unsigned int loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char *data =
		    stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
			             height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		} else {
			std::cout << "Cubemap failed to load at path: " << faces[i]
			          << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureID;
}
}  // namespace Callback

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "callback.h"  // Callback::loadTexture (temporaire, à déplacer dans Texture)
#include "filesystem.h"
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
// clang-format on

// Idéalement défini par CMake (target_compile_definitions).
#ifndef SHADER_DIR
#define SHADER_DIR "shaders/"
#endif

namespace {

// clang-format off
// positions            // normals         // texcoords
constexpr float kPlaneVertices[] = {
     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
     10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f};
// clang-format on

constexpr int kLightCount = 4;

}  // namespace

Game::Game() = default;

// Le destructeur de Game appelle shutdown() : les ressources GL sont
// supprimées AVANT la destruction de la fenêtre / du contexte.
Game::~Game() { shutdown(); }

// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------

bool Game::init() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return false;
	}
	m_glfwReady = true;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	m_window = glfwCreateWindow(m_width, m_height, "Game", nullptr, nullptr);
	if (!m_window) {
		std::cerr << "Failed to create GLFW window\n";
		return false;  // le destructeur fera glfwTerminate()
	}
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);  // vsync

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		std::cerr << "Failed to initialize GLAD\n";
		return false;
	}
	m_glReady = true;

	// La taille du framebuffer peut différer de celle de la fenêtre (HiDPI).
	glfwGetFramebufferSize(m_window, &m_width, &m_height);
	glViewport(0, 0, m_width, m_height);
	updateProjection();

	registerCallbacks();
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	setupGlState();
	loadResources();
	return true;
}

void Game::registerCallbacks() {
	glfwSetWindowUserPointer(m_window, this);

	// Lambdas sans capture -> convertibles en pointeurs de fonction C.
	// Définies dans une méthode de Game, elles ont accès aux membres privés.
	glfwSetFramebufferSizeCallback(m_window,
	                               [](GLFWwindow *w, int width, int height) {
		                               fromWindow(w)->onResize(width, height);
	                               });
	glfwSetCursorPosCallback(m_window, [](GLFWwindow *w, double x, double y) {
		fromWindow(w)->onMouseMove(x, y);
	});
	glfwSetScrollCallback(m_window,
	                      [](GLFWwindow *w, double /*xoff*/, double yoff) {
		                      fromWindow(w)->onScroll(yoff);
	                      });
	glfwSetKeyCallback(m_window,
	                   [](GLFWwindow *w, int key, int /*scancode*/, int action,
	                      int /*mods*/) { fromWindow(w)->onKey(key, action); });
}

Game *Game::fromWindow(GLFWwindow *window) {
	return static_cast<Game *>(glfwGetWindowUserPointer(window));
}

void Game::setupGlState() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::loadResources() {
	// --- shader ---
	m_shader = std::make_unique<Shader>(SHADER_DIR "1.1.depth_testing.vs",
	                                    SHADER_DIR "1.1.depth_testing.fs");

	// --- plan (VAO / VBO) ---
	glGenVertexArrays(1, &m_planeVAO);
	glGenBuffers(1, &m_planeVBO);
	glBindVertexArray(m_planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kPlaneVertices), kPlaneVertices,
	             GL_STATIC_DRAW);
	const GLsizei stride = 8 * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
	                      reinterpret_cast<void *>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
	                      reinterpret_cast<void *>(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
	                      reinterpret_cast<void *>(6 * sizeof(float)));
	glBindVertexArray(0);

	// --- textures ---
	const std::string woodPath =
	    FileSystem::getPath("resources/textures/wood.png");
	m_floorTex = Callback::loadTexture(woodPath.c_str(), false);
	m_floorTexGamma = Callback::loadTexture(woodPath.c_str(), true);

	// --- uniforms constants : envoyés une seule fois ---
	const glm::vec3 lightPositions[kLightCount] = {
	    glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
	    glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f)};
	const glm::vec3 lightColors[kLightCount] = {
	    glm::vec3(0.25f), glm::vec3(0.50f), glm::vec3(0.75f), glm::vec3(1.00f)};

	m_shader->use();
	m_shader->setInt("floorTexture", 0);
	glUniform3fv(glGetUniformLocation(m_shader->ID, "lightPositions"),
	             kLightCount, glm::value_ptr(lightPositions[0]));
	glUniform3fv(glGetUniformLocation(m_shader->ID, "lightColors"), kLightCount,
	             glm::value_ptr(lightColors[0]));
}

// ---------------------------------------------------------------------------
// Boucle principale
// ---------------------------------------------------------------------------

void Game::run() {
	if (!m_window || !m_shader) {
		std::cerr << "Game::run() appelé sans init() réussi\n";
		return;
	}

	// Évite un énorme deltaTime à la première frame (chargement des
	// ressources).
	m_lastFrame = static_cast<float>(glfwGetTime());

	while (!glfwWindowShouldClose(m_window)) {
		tickTime();
		processInput();
		update(m_dt);
		render();
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

void Game::tickTime() {
	const float now = static_cast<float>(glfwGetTime());
	m_dt = now - m_lastFrame;
	m_lastFrame = now;
}

// Entrées "continues" (touche maintenue). Les bascules (B, G, Échap)
// sont gérées par onKey() pour ne se déclencher qu'une fois par appui.
void Game::processInput() {
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		m_camera.ProcessKeyboard(FORWARD, m_dt);
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		m_camera.ProcessKeyboard(BACKWARD, m_dt);
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
		m_camera.ProcessKeyboard(LEFT, m_dt);
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
		m_camera.ProcessKeyboard(RIGHT, m_dt);
}

void Game::update(float /*dt*/) {
	// Logique de jeu (entités, physique, IA...) : rien pour l'instant.
}

void Game::render() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_shader->use();
	m_shader->setMat4("projection", m_projection);
	m_shader->setMat4("view", m_camera.GetViewMatrix());
	m_shader->setVec3("viewPos", m_camera.Position);
	m_shader->setInt("gamma", m_gammaEnabled);
	m_shader->setInt("blinn", m_blinn);

	// sol
	glBindVertexArray(m_planeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_gammaEnabled ? m_floorTexGamma : m_floorTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ---------------------------------------------------------------------------
// Événements
// ---------------------------------------------------------------------------

void Game::onResize(int width, int height) {
	if (width == 0 || height == 0)
		return;  // fenêtre minimisée
	m_width = width;
	m_height = height;
	glViewport(0, 0, m_width, m_height);
	updateProjection();
}

void Game::onMouseMove(double x, double y) {
	const float xpos = static_cast<float>(x);
	const float ypos = static_cast<float>(y);

	if (m_firstMouse) {
		m_lastX = xpos;
		m_lastY = ypos;
		m_firstMouse = false;
	}
	const float xoffset = xpos - m_lastX;
	const float yoffset = m_lastY - ypos;  // inversé : y va de haut en bas
	m_lastX = xpos;
	m_lastY = ypos;

	m_camera.ProcessMouseMovement(xoffset, yoffset);
}

void Game::onScroll(double yoffset) {
	m_camera.ProcessMouseScroll(static_cast<float>(yoffset));
	updateProjection();  // le zoom modifie le FOV
}

void Game::onKey(int key, int action) {
	if (action != GLFW_PRESS)
		return;

	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
		break;
	case GLFW_KEY_B:
		m_blinn = !m_blinn;
		std::cout << (m_blinn ? "Blinn-Phong" : "Phong") << '\n';
		break;
	case GLFW_KEY_G:
		m_gammaEnabled = !m_gammaEnabled;
		std::cout << (m_gammaEnabled ? "Gamma on" : "Gamma off") << '\n';
		break;
	default:
		break;
	}
}

void Game::updateProjection() {
	if (m_height == 0)
		return;
	m_projection = glm::perspective(glm::radians(m_camera.Zoom),
	                                static_cast<float>(m_width) /
	                                    static_cast<float>(m_height),
	                                0.1f, 1000.0f);
}

// ---------------------------------------------------------------------------
// Nettoyage (idempotent)
// ---------------------------------------------------------------------------

void Game::shutdown() {
	// 1. ressources GL : le contexte doit encore exister
	if (m_glReady) {
		glDeleteVertexArrays(1, &m_planeVAO);
		glDeleteBuffers(1, &m_planeVBO);
		glDeleteTextures(1, &m_floorTex);
		glDeleteTextures(1, &m_floorTexGamma);
		if (m_shader)
			glDeleteProgram(m_shader->ID);
		m_planeVAO = m_planeVBO = m_floorTex = m_floorTexGamma = 0;
		m_glReady = false;
	}
	m_shader.reset();

	// 2. fenêtre + contexte
	if (m_window) {
		glfwDestroyWindow(m_window);
		m_window = nullptr;
	}

	// 3. GLFW (symétrique de glfwInit() dans init())
	if (m_glfwReady) {
		glfwTerminate();
		m_glfwReady = false;
	}
}

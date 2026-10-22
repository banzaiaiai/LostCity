// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "mesh.h"
#include "meshFactory.h"
#include "filesystem.h"
#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
// clang-format on

namespace {  // constante
constexpr float c_eyeHeight = 0.5f;
}
namespace {  // var of all the room
constexpr float kRoomW = 20.0f;
constexpr float kRoomD = 20.0f;
constexpr float kRoomH = 3.0f;
constexpr float kWallThickness = 0.5f;  // épais = pas de "tunneling"
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
	m_renderer.setViewport(m_width, m_height);
	updateProjection();

	registerCallbacks();
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

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

void Game::loadResources() {
	// --- renderer (shader + états GL) ---
	m_renderer.init(SHADER_DIR "1.1.depth_testing.vs",
	                SHADER_DIR "1.1.depth_testing.fs");

	// --- lumières (envoyées une seule fois) ---
	m_renderer.setLights({
	    {glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(0.25f)},
	    {glm::vec3(-1.0f, 2.0f, 0.0f), glm::vec3(0.50f)},
	    {glm::vec3(1.0f, 2.0f, 0.0f), glm::vec3(0.75f)},
	    {glm::vec3(3.0f, 2.0f, 0.0f), glm::vec3(1.00f)},
	});

	// --- sol : un plan 20x20 abaissé de 0.5, texture répétée 10x10 ---
	// Game::loadResources()
	const Texture wood = m_renderer.loadTexture(
	    FileSystem::getPath("resources/textures/wood.png"));
	const Texture wall = m_renderer.loadTexture(FileSystem::getPath(
	    "resources/textures/metal.png"));  // votre texture de mur
	std::vector<Texture> textures{wall, wood};
	m_room = std::make_unique<Mesh>(
	    MeshFactory::room(kRoomW, kRoomD, kRoomH, textures));
	m_roomModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	buildRectColliders();
	m_eyeHeight = m_roomModel[3].y + c_eyeHeight;  // sol + hauteur des yeux
	m_camera.Position.y = m_eyeHeight;
	// load model
	m_dead_body = std::make_unique<Model>(
	    FileSystem::getPath("resources/objects/hospital_stuff/dead_body.obj"));
	m_deadBodyModel = glm::translate(m_deadBodyModel, m_camera.Position);
	m_deadBodyModel = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
}

// ---------------------------------------------------------------------------
// Boucle principale
// ---------------------------------------------------------------------------

void Game::run() {
	if (!m_window || !m_renderer.isReady()) {
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
	m_dt = std::min(now - m_lastFrame, 0.05f);
	m_lastFrame = now;
}

// Entrées "continues" (touche maintenue). Les bascules (B, G, Échap)
// sont gérées par onKey() pour ne se déclencher qu'une fois par appui.
void Game::processInput() {
	// Directions mises à plat : regarder en haut/bas ne ralentit pas la marche.
	glm::vec3 front = m_camera.Front;
	front.y = 0.0f;
	glm::vec3 right = m_camera.Right;
	right.y = 0.0f;
	if (glm::length(front) > 1e-4f)
		front = glm::normalize(front);
	if (glm::length(right) > 1e-4f)
		right = glm::normalize(right);

	glm::vec3 wish(0.0f);
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		wish += front;
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		wish -= front;
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
		wish += right;
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
		wish -= right;

	if (glm::length(wish) > 1e-4f)
		movePlayer(glm::normalize(wish), m_dt);

	m_camera.Position.y = m_eyeHeight;  // hauteur fixe
}

void Game::update(float /*dt*/) {
	// Logique de jeu (entités, physique, IA...) : rien pour l'instant.
}

void Game::render() {
	m_renderer.beginFrame(m_camera, m_projection);
	m_renderer.draw(*m_room, m_roomModel);
	// Game.cpp, dans render()
	if (m_dead_body)
		m_renderer.draw(*m_dead_body, m_deadBodyModel);
}

// ---------------------------------------------------------------------------
// Événements
// ---------------------------------------------------------------------------

void Game::onResize(int width, int height) {
	if (width == 0 || height == 0)
		return;  // fenêtre minimisée
	m_width = width;
	m_height = height;
	m_renderer.setViewport(m_width, m_height);
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
		m_renderer.setBlinn(!m_renderer.blinn());
		std::cout << (m_renderer.blinn() ? "Blinn-Phong" : "Phong") << '\n';
		break;
	case GLFW_KEY_G:
		m_renderer.setGamma(!m_renderer.gamma());
		std::cout << (m_renderer.gamma() ? "Gamma on" : "Gamma off") << '\n';
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
	// 1. resources GL : le contexte doit encore exister
	if (m_glReady) {
		m_room.reset();
		m_dead_body.reset();
		m_renderer.shutdown();
		m_glReady = false;
	}

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
void Game::buildRectColliders() {
	const float hw = kRoomW * 0.5f;
	const float hd = kRoomD * 0.5f;
	const float t = kWallThickness;
	const glm::vec2 c(m_roomModel[3].x,
	                  m_roomModel[3].z);  // centre de la pièce

	// Les murs sont posés à l'EXTÉRIEUR de la surface intérieure de la pièce.
	m_walls = {
	    {c + glm::vec2(-hw - t, -hd - t), c + glm::vec2(hw + t, -hd)},  // nord
	    {c + glm::vec2(-hw - t, hd), c + glm::vec2(hw + t, hd + t)},    // sud
	    {c + glm::vec2(-hw - t, -hd), c + glm::vec2(-hw, hd)},          // ouest
	    {c + glm::vec2(hw, -hd), c + glm::vec2(hw + t, hd)},            // est
	};
}

bool Game::collides(const glm::vec2 &p) const {
	const float r = m_playerRadius;
	for (const AABB &w : m_walls) {
		// mur "gonflé" de r : on teste juste un point dans un rectangle
		if (p.x > w.min.x - r && p.x < w.max.x + r && p.y > w.min.y - r &&
		    p.y < w.max.y + r)
			return true;
	}
	return false;
}

void Game::movePlayer(const glm::vec3 &dir, float dt) {
	const glm::vec2 step =
	    glm::vec2(dir.x, dir.z) * m_camera.MovementSpeed * dt;
	glm::vec2 pos(m_camera.Position.x, m_camera.Position.z);

	// Axe X d'abord, puis axe Z : permet de glisser le long des murs.
	const glm::vec2 tryX(pos.x + step.x, pos.y);
	if (!collides(tryX))
		pos = tryX;

	const glm::vec2 tryZ(pos.x, pos.y + step.y);
	if (!collides(tryZ))
		pos = tryZ;

	m_camera.Position.x = pos.x;
	m_camera.Position.z = pos.y;
}

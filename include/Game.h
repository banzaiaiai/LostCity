#ifndef GAME_H
#define GAME_H

#include "camera.h"
#include <glm/glm.hpp>
#include <memory>

struct GLFWwindow;
class Shader;

class Game {
  public:
	Game();
	~Game();

	// Game donne `this` à GLFW (user pointer) : copie et déplacement interdits.
	Game(const Game &) = delete;
	Game &operator=(const Game &) = delete;
	Game(Game &&) = delete;
	Game &operator=(Game &&) = delete;

	bool init();
	void run();

  private:
	// --- les phases d'une frame ---
	void tickTime();
	void processInput();
	void update(float dt);
	void render();

	// --- initialisation / nettoyage ---
	void registerCallbacks();
	void setupGlState();
	void loadResources();
	void shutdown();

	// --- événements GLFW (appelés depuis les callbacks) ---
	void onResize(int width, int height);
	void onMouseMove(double x, double y);
	void onScroll(double yoffset);
	void onKey(int key, int action);
	void updateProjection();

	static Game *fromWindow(GLFWwindow *window);

  private:
	// fenêtre
	GLFWwindow *m_window{nullptr};
	int m_width{800};
	int m_height{600};
	bool m_glfwReady{false};
	bool m_glReady{false};

	// état du jeu
	Camera m_camera{glm::vec3(0.0f, 0.0f, 3.0f)};
	glm::mat4 m_projection{1.0f};
	bool m_blinn{true};
	bool m_gammaEnabled{false};

	// temps
	float m_dt{0.0f};
	float m_lastFrame{0.0f};

	// entrée souris
	float m_lastX{0.0f};
	float m_lastY{0.0f};
	bool m_firstMouse{true};

	// ressources GL
	std::unique_ptr<Shader> m_shader;
	unsigned int m_planeVAO{0};
	unsigned int m_planeVBO{0};
	unsigned int m_floorTex{0};
	unsigned int m_floorTexGamma{0};
};
#endif  // !GAME_H

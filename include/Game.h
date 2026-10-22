#ifndef GAME_H
#define GAME_H

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include <glm/glm.hpp>
#include <memory>

struct GLFWwindow;
// Rectangle dans le plan XZ (x -> x, y -> z), vu de dessus.
struct AABB {
	glm::vec2 min;
	glm::vec2 max;
};
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
	void loadResources();
	void shutdown();

	// --- événements GLFW (appelés depuis les callbacks) ---
	void onResize(int width, int height);
	void onMouseMove(double x, double y);
	void onScroll(double yoffset);
	void onKey(int key, int action);
	void updateProjection();

	static Game *fromWindow(GLFWwindow *window);

  private:  // colision
	void buildRectColliders();
	void movePlayer(const glm::vec3 &dir, float dt);
	bool collides(const glm::vec2 &p) const;

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

	// temps
	float m_dt{0.0f};
	float m_lastFrame{0.0f};

	// entrée souris
	float m_lastX{0.0f};
	float m_lastY{0.0f};
	bool m_firstMouse{true};

	// rendu
	Renderer m_renderer;

	// scène (provisoire : deviendra des Room / Entity)
	// unique_ptr : un Mesh ne peut être construit qu'une fois le contexte GL
	// créé.
	std::unique_ptr<Mesh> m_room;
	glm::mat4 m_roomModel{1.0f};

	// colision
	std::vector<AABB> m_walls;
	float m_playerRadius{0.3f};
	float m_eyeHeight{1.6f};
};
#endif  // !GAME_H

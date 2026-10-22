#ifndef RENDERER_H
#define RENDERER_H

#include "mesh.h"  // Mesh, Texture, Vertex (+ shader.h)
#include "model.h"
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Camera;

struct Light {
	glm::vec3 position;
	glm::vec3 color;
};

// Le Renderer cache l'OpenGL "bas niveau" (shader, uniforms, états GL).
// Le reste du jeu ne fait plus que :
//
//   renderer.beginFrame(camera, projection);
//   renderer.draw(mesh, modelMatrix);   // autant de fois que voulu
//
// Les textures sont portées par le Mesh (mesh.textures) : le Renderer ne fait
// que les charger (loadTexture) et choisir la bonne variante linéaire / sRGB.
//
// À initialiser (init) après la création du contexte OpenGL et à arrêter
// (shutdown) avant la destruction de la fenêtre.
class Renderer {
  public:
	// Doit correspondre à la taille des tableaux dans le fragment shader.
	static constexpr int kMaxLights = 4;

	Renderer();
	~Renderer();

	Renderer(const Renderer &) = delete;
	Renderer &operator=(const Renderer &) = delete;

	bool init(const char *vertexPath, const char *fragmentPath);
	void shutdown();  // idempotent
	bool isReady() const { return m_shader != nullptr; }

	void setViewport(int width, int height);

	// --- ressources ---
	// Charge (une seule fois par chemin) la texture en version linéaire ET
	// sRGB. Retourne un Texture prêt à être mis dans un Mesh ; à chaque draw,
	// le Renderer remplace son id par la variante correspondant au gamma.
	Texture loadTexture(const std::string &path,
	                    const std::string &type = "texture_diffuse");

	// --- état de la scène ---
	void setLights(const std::vector<Light> &lights);
	void setGamma(bool enabled) { m_gamma = enabled; }
	void setBlinn(bool enabled) { m_blinn = enabled; }
	bool gamma() const { return m_gamma; }
	bool blinn() const { return m_blinn; }
	void setClearColor(const glm::vec3 &color) { m_clearColor = color; }

	// --- dessin ---
	void beginFrame(const Camera &camera, const glm::mat4 &projection);
	void draw(Mesh &mesh, const glm::mat4 &model = glm::mat4(1.0f));
	void draw(Model &model, const glm::mat4 &transform = glm::mat4(1.0f));

  private:
	struct TexVariants {
		unsigned int linear{0};
		unsigned int srgb{0};
	};

	std::unique_ptr<Shader> m_shader;
	std::map<std::string, TexVariants> m_textureCache;         // par chemin
	std::unordered_map<unsigned int, TexVariants> m_variants;  // par id GL

	glm::vec3 m_clearColor{0.1f};
	bool m_gamma{false};
	bool m_blinn{true};
};

#endif  // !RENDERER_H

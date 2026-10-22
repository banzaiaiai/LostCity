// clang-format off
#include <glad/glad.h>
#include "renderer.h"
#include "callback.h"  // Callback::loadTexture (temporaire)
#include "camera.h"
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
// clang-format on

Renderer::Renderer() = default;
Renderer::~Renderer() { shutdown(); }

bool Renderer::init(const char *vertexPath, const char *fragmentPath) {
	if (m_shader)
		return true;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_shader = std::make_unique<Shader>(vertexPath, fragmentPath);
	return true;
}

void Renderer::shutdown() {
	for (const auto &entry : m_textureCache) {
		glDeleteTextures(1, &entry.second.linear);
		glDeleteTextures(1, &entry.second.srgb);
	}
	m_textureCache.clear();
	m_variants.clear();

	if (m_shader) {
		glDeleteProgram(m_shader->ID);
		m_shader.reset();
	}
}

void Renderer::setViewport(int width, int height) {
	glViewport(0, 0, width, height);
}

Texture Renderer::loadTexture(const std::string &path,
                              const std::string &type) {
	auto it = m_textureCache.find(path);
	if (it == m_textureCache.end()) {
		TexVariants v;
		v.linear = Callback::loadTexture(path.c_str(), false);
		v.srgb = Callback::loadTexture(path.c_str(), true);
		it = m_textureCache.emplace(path, v).first;
		m_variants[v.linear] = v;
		m_variants[v.srgb] = v;
	}

	Texture tex;
	tex.type = type;
	tex.path = path;
	tex.id = m_gamma ? it->second.srgb : it->second.linear;
	return tex;
}

void Renderer::setLights(const std::vector<Light> &lights) {
	if (!m_shader)
		return;

	if (lights.size() > static_cast<std::size_t>(kMaxLights))
		std::cerr << "Renderer: " << lights.size() << " lumières, seules les "
		          << kMaxLights << " premières sont utilisées\n";

	// Les emplacements inutilisés restent à 0 (couleur noire = aucune
	// contribution).
	glm::vec3 positions[kMaxLights]{};
	glm::vec3 colors[kMaxLights]{};
	const int count =
	    std::min<int>(static_cast<int>(lights.size()), kMaxLights);
	for (int i = 0; i < count; ++i) {
		positions[i] = lights[i].position;
		colors[i] = lights[i].color;
	}

	m_shader->use();
	glUniform3fv(glGetUniformLocation(m_shader->ID, "lightPositions"),
	             kMaxLights, glm::value_ptr(positions[0]));
	glUniform3fv(glGetUniformLocation(m_shader->ID, "lightColors"), kMaxLights,
	             glm::value_ptr(colors[0]));
}

void Renderer::beginFrame(const Camera &camera, const glm::mat4 &projection) {
	glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!m_shader)
		return;

	m_shader->use();
	m_shader->setMat4("projection", projection);
	m_shader->setMat4("view", camera.GetViewMatrix());
	m_shader->setVec3("viewPos", camera.Position);
	m_shader->setInt("gamma", m_gamma);
	m_shader->setInt("blinn", m_blinn);
}

void Renderer::draw(Mesh &mesh, const glm::mat4 &model) {
	if (!m_shader)
		return;

	// Choisit la variante linéaire / sRGB des textures chargées par le
	// Renderer ; les textures venant d'ailleurs ne sont pas touchées.
	for (Texture &t : mesh.textures) {
		const auto it = m_variants.find(t.id);
		if (it != m_variants.end())
			t.id = m_gamma ? it->second.srgb : it->second.linear;
	}

	m_shader->use();
	m_shader->setMat4("model", model);
	mesh.Draw(*m_shader);  // lie les textures (unité 0..n) puis dessine
}

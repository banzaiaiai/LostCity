#include "meshFactory.h"
#include <utility>

namespace MeshFactory {

// Ajoute un quad (4 sommets + 2 triangles). p0..p3 doivent être dans le sens
// anti-horaire vus du côté vers lequel pointe `normal`.
// Texcoords : p0 -> (0,0), p1 -> (uMax,0), p2 -> (uMax,vMax), p3 -> (0,vMax).
void addQuad(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices,
             const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
             const glm::vec3 &p3, const glm::vec3 &normal, float uMax,
             float vMax) {
	const unsigned int base = static_cast<unsigned int>(vertices.size());

	vertices.push_back({p0, normal, glm::vec2(0.0f, 0.0f)});
	vertices.push_back({p1, normal, glm::vec2(uMax, 0.0f)});
	vertices.push_back({p2, normal, glm::vec2(uMax, vMax)});
	vertices.push_back({p3, normal, glm::vec2(0.0f, vMax)});

	indices.insert(indices.end(),
	               {base, base + 1, base + 2, base, base + 2, base + 3});
}

Mesh plane(float width, float depth, float uvRepeatX, float uvRepeatY,
           std::vector<Texture> textures) {
	const float hw = width * 0.5f;
	const float hd = depth * 0.5f;
	const glm::vec3 up(0.0f, 1.0f, 0.0f);

	std::vector<Vertex> vertices = {
	    {glm::vec3(hw, 0.0f, hd), up, glm::vec2(uvRepeatX, 0.0f)},
	    {glm::vec3(-hw, 0.0f, hd), up, glm::vec2(0.0f, 0.0f)},
	    {glm::vec3(-hw, 0.0f, -hd), up, glm::vec2(0.0f, uvRepeatY)},
	    {glm::vec3(hw, 0.0f, -hd), up, glm::vec2(uvRepeatX, uvRepeatY)},
	};
	std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

	return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

Mesh room(float width, float depth, float height, std::vector<Texture> textures,
          float uvPerUnit) {
	const float hw = width * 0.5f;
	const float hd = depth * 0.5f;
	const float hh = height * 0.5f;

	// répétitions de texture sur chaque dimension
	const float uw = width * uvPerUnit;
	const float ud = depth * uvPerUnit;
	const float uh = height * uvPerUnit;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	vertices.reserve(24);
	indices.reserve(36);

	// sol (y = -hh), normale +Y
	addQuad(vertices, indices, glm::vec3(-hw, -hh, hd), glm::vec3(hw, -hh, hd),
	        glm::vec3(hw, -hh, -hd), glm::vec3(-hw, -hh, -hd),
	        glm::vec3(0.0f, 1.0f, 0.0f), uw, ud);

	// plafond (y = +hh), normale -Y
	addQuad(vertices, indices, glm::vec3(-hw, hh, -hd), glm::vec3(hw, hh, -hd),
	        glm::vec3(hw, hh, hd), glm::vec3(-hw, hh, hd),
	        glm::vec3(0.0f, -1.0f, 0.0f), uw, ud);

	// mur du fond (z = -hd), normale +Z
	addQuad(vertices, indices, glm::vec3(-hw, -hh, -hd),
	        glm::vec3(hw, -hh, -hd), glm::vec3(hw, hh, -hd),
	        glm::vec3(-hw, hh, -hd), glm::vec3(0.0f, 0.0f, 1.0f), uw, uh);

	// mur de devant (z = +hd), normale -Z
	addQuad(vertices, indices, glm::vec3(hw, -hh, hd), glm::vec3(-hw, -hh, hd),
	        glm::vec3(-hw, hh, hd), glm::vec3(hw, hh, hd),
	        glm::vec3(0.0f, 0.0f, -1.0f), uw, uh);

	// mur gauche (x = -hw), normale +X
	addQuad(vertices, indices, glm::vec3(-hw, -hh, hd),
	        glm::vec3(-hw, -hh, -hd), glm::vec3(-hw, hh, -hd),
	        glm::vec3(-hw, hh, hd), glm::vec3(1.0f, 0.0f, 0.0f), ud, uh);

	// mur droit (x = +hw), normale -X
	addQuad(vertices, indices, glm::vec3(hw, -hh, -hd), glm::vec3(hw, -hh, hd),
	        glm::vec3(hw, hh, hd), glm::vec3(hw, hh, -hd),
	        glm::vec3(-1.0f, 0.0f, 0.0f), ud, uh);

	return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

}  // namespace MeshFactory

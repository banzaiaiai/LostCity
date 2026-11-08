#ifndef MODEL_H
#define MODEL_H
#include "glm/ext/vector_float3.hpp"
#include "mesh.h"
#include "shader.h"
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <string>

class Renderer;  // déclaration anticipée (pas d'include, pour éviter une
                 // boucle)
struct Bounds {
	glm::vec3 min{0.0f};
	glm::vec3 max{0.0f};
	glm::vec3 center() const { return (min + max) * 0.5f; }
	glm::vec3 size() const { return max - min; }
};
class Model {
  public:
	Model(string path, Renderer &renderer);
	void Draw(Shader &shader);

	vector<Mesh> meshes;  // textures_loaded est supprimé
	Bounds bounds() const;

	glm::mat4 normalizeMatrix(float height = 0.0f) const;

  private:
	string directory;
	Renderer *m_renderer{nullptr};  // valide uniquement pendant le chargement

	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
	                                     string typeName);
};
#endif  // MODEL_H

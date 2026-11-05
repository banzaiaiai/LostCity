#ifndef MODEL_H
#define MODEL_H
#include "mesh.h"
#include "shader.h"
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <string>

class Renderer;  // déclaration anticipée (pas d'include, pour éviter une
                 // boucle)

class Model {
  public:
	Model(string path, Renderer &renderer);
	void Draw(Shader &shader);

	vector<Mesh> meshes;  // textures_loaded est supprimé

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

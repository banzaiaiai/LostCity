#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "model.h"
#include "shader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include <stb_image.h>

#include "renderer.h"

Model::Model(string path, Renderer &renderer) : m_renderer(&renderer) {
	loadModel(path);
	m_renderer = nullptr;  // le Model sera déplacé, on ne garde pas de pointeur
}

void Model::Draw(Shader &shader) {
	for (unsigned int i{0}; i < meshes.size(); i++) { meshes[i].Draw(shader); }
}

void Model::loadModel(string path) {
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(
	    path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
	              aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
	    !scene->mRootNode) {
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}
void Model::processNode(aiNode *node, const aiScene *scene) {
	// process all the node’s meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		if (mesh->mTextureCoords[0])  // does the mesh contain texture
		                              // coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		} else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	// process indices
	for (unsigned int i{0}; i < mesh->mNumFaces; i++) {
		aiFace face{mesh->mFaces[i]};
		for (unsigned int j{0}; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material{scene->mMaterials[mesh->mMaterialIndex]};
		vector<Texture> diffuseMaps{loadMaterialTextures(
		    material, aiTextureType_DIFFUSE, "texture_diffuse")};
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = loadMaterialTextures(
		    material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(),
		                specularMaps.end());
	}
	return Mesh(vertices, indices, textures);
}
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            string typeName) {
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// Le Renderer charge (linéaire + sRGB), met en cache par chemin
		// et enregistre les ids dans m_variants.
		textures.push_back(
		    m_renderer->loadTexture(directory + '/' + str.C_Str(), typeName));
	}
	return textures;
}
Bounds Model::bounds() const {
	Bounds b;
	bool first = true;
	for (const Mesh &m : meshes)
		for (const Vertex &v : m.vertices) {
			if (first) {
				b.min = b.max = v.Position;
				first = false;
			}
			b.min = glm::min(b.min, v.Position);
			b.max = glm::max(b.max, v.Position);
		}
	return b;
}

glm::mat4 Model::normalizeMatrix(float height) const {
	const Bounds b = bounds();
	const glm::vec3 c = b.center();
	// centre en X/Z, bas du modèle à y = 0
	const glm::vec3 offset(-c.x, -b.min.y, -c.z);

	float s = 1.0f;
	if (height > 0.0f && b.size().y > 1e-6f)
		s = height / b.size().y;

	return glm::scale(glm::mat4(1.0f), glm::vec3(s)) *
	       glm::translate(glm::mat4(1.0f), offset);
}

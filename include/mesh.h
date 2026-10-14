#ifndef MESH_H
#define MESH_H

#include "VAO.h"
#include "VBO.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "shader.h"
#include <assimp/types.h>
#include <string>
#include <vector>

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    string type;
    string path;
    unsigned int id;
   };

class Mesh {
public: // Constructor 
    Mesh(std::vector<Vertex> vertices, vector<unsigned int> indices,
         vector<Texture> textures);
public : // mesh data
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
public : // mesh specifi method
    void Draw(Shader &shader);
private:
    // render data
    VAO Vao;
    VBO Vbo;
    unsigned int Ebo {0};
    void setupMesh();
};

#endif 

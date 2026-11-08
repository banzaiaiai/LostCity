#include "mesh.h"
#include "VBO.h"
#include <cstddef>
#include <vector>

Mesh::Mesh(vector<Vertex> vertices, std::vector<unsigned int> indices,
    std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

void Mesh::setupMesh()
{
    Vao.load();
    Vao.bind();
     
    Vbo.load(vertices.data(),vertices.size()*sizeof(Vertex),GL_STATIC_DRAW);

    // #TODO implement Ebo
    glGenBuffers(1, &Ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() *sizeof(unsigned int), 
        indices.data(), GL_STATIC_DRAW);
    // vertex positions
    Vao.setAttribute(0,3, sizeof(Vertex),offsetof(Vertex,Position));
    // vertex normals
    Vao.setAttribute(1,3, sizeof(Vertex),offsetof(Vertex, Normal));
    // vertex texture coords
    Vao.setAttribute(2,2, sizeof(Vertex),offsetof(Vertex, TexCoords));

    Vao.unbind();
}

void Mesh::Draw(Shader &shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate texture unit first
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);
        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);
    // draw mesh
    Vao.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

#ifndef MESH_FACTORY_H
#define MESH_FACTORY_H

#include "mesh.h"
#include <vector>

// Fabrique de géométries simples pour VOTRE classe Mesh
// (Vertex / indices / Texture). Toutes les formes sont centrées sur l'origine :
// on les place dans le monde avec la matrice `model` passée à Renderer::draw.
namespace MeshFactory {

// Plan horizontal (dans XZ), normale +Y.
// Pour un mur : le faire pivoter / le translater avec `model`.
Mesh plane(float width, float depth, float uvRepeatX = 1.0f,
           float uvRepeatY = 1.0f, std::vector<Texture> textures = {});

// Salle fermée vue de l'INTÉRIEUR : sol, plafond et 4 murs dans un seul Mesh
// (24 sommets, 36 indices), toutes les normales pointent vers l'intérieur.
// La salle est centrée sur l'origine : x dans [-width/2, width/2],
// y dans [-height/2, height/2], z dans [-depth/2, depth/2].
// `uvPerUnit` : nombre de répétitions de texture par unité de longueur
// (0.5 = la texture se répète toutes les 2 unités, comme votre sol 20 -> 10).
Mesh room(float width, float depth, float height,
          std::vector<Texture> textures = {}, float uvPerUnit = 0.5f);

}  // namespace MeshFactory

#endif  // !MESH_FACTORY_H

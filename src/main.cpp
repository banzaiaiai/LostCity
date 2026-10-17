// clang-format off
#include "Game.h"

// TODO: déplacer dans un stb_image.cpp dédié (une seule définition dans le projet)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// clang-format on

#include <iostream>

int main() {
	Game game;
	if (!game.init()) {
		std::cerr << "Erreur d'initialisation du jeu\n";
		return -1;  // ~Game() nettoie ce qui a été initialisé
	}
	game.run();
	return 0;
}

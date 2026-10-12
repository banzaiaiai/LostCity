#include "texture.h"
#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>

Texture::Texture() {}

Texture::~Texture() {
    if (Id != 0) glDeleteTextures(1, &Id);
}

Texture::Texture(Texture &&other) noexcept
    : Id(other.Id), width(other.width), height(other.height), nrChannels(other.nrChannels) {
    other.Id = 0;
}

Texture &Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        if (Id != 0) glDeleteTextures(1, &Id);
        Id = other.Id;
        width = other.width;
        height = other.height;
        nrChannels = other.nrChannels;
        other.Id = 0;
    }
    return *this;
}

bool Texture::load(const char *file, bool flip) {
    stbi_set_flip_vertically_on_load(flip);   // toujours défini explicitement

    unsigned char *data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "Failed to load texture: " << file << std::endl;
        return false;
    }

    GLenum format = GL_RGB;
    if (nrChannels == 1)      format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;

    if (Id == 0) glGenTextures(1, &Id);
    glBindTexture(GL_TEXTURE_2D, Id);

    // Important pour les images dont la largeur*canaux n'est pas multiple de 4
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return true;
}

void Texture::use(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, Id);
}

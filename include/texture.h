#ifndef TEXTURE_H
#define TEXTURE_H


class Texture {
public: // mandatory
    Texture();
    ~Texture();

    // Non copiable (on possède une ressource OpenGL), mais déplaçable
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;
    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;
public : //initialiseur
    bool load(const char *file, bool flip = false);
    // unit = 0 pour GL_TEXTURE0, 1 pour GL_TEXTURE1, etc.
    void use(unsigned int unit = 0) const;
private:
    unsigned int Id = 0;
    int width = 0;
    int height = 0;
    int nrChannels = 0;
};

#endif // !TEXTURE_H



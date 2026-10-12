#ifndef VBO_H
#define VBO_H

#include <cstddef>
#include <glad/glad.h>

class VBO {
public:
    VBO() = default;
    ~VBO();

    VBO(const VBO &) = delete;
    VBO &operator=(const VBO &) = delete;
    VBO(VBO &&other) noexcept;
    VBO &operator=(VBO &&other) noexcept;

    // Crée le buffer et envoie les données au GPU
    bool load(const void *data, std::size_t size, GLenum usage = GL_STATIC_DRAW);
    void bind() const;
    static void unbind();

    unsigned int id() const { return Id; }

private:
    void release();
    unsigned int Id = 0;
};

#endif // VBO_H

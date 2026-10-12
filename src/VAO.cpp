#include <glad/glad.h>
#include "VAO.h"

VAO::~VAO() { release(); }

VAO::VAO(VAO &&other) noexcept : Id(other.Id) {
    other.Id = 0;
}

VAO &VAO::operator=(VAO &&other) noexcept {
    if (this != &other) {
        release();
        Id = other.Id;
        other.Id = 0;
    }
    return *this;
}

void VAO::release() {
    if (Id != 0) {
        glDeleteVertexArrays(1, &Id);
        Id = 0;
    }
}

bool VAO::load() {
    release();
    glGenVertexArrays(1, &Id);
    return Id != 0;
}

void VAO::bind() const { glBindVertexArray(Id); }

void VAO::unbind() { glBindVertexArray(0); }

void VAO::setAttribute(unsigned int index, int size,
                       int stride, std::size_t offset) const {
    bind();
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void *>(offset));
    glEnableVertexAttribArray(index);
}

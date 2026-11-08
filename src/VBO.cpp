#include "VBO.h"

VBO::~VBO() { release(); }

VBO::VBO(VBO &&other) noexcept : Id(other.Id) {
    other.Id = 0;
}

VBO &VBO::operator=(VBO &&other) noexcept {
    if (this != &other) {
        release();
        Id = other.Id;
        other.Id = 0;
    }
    return *this;
}

void VBO::release() {
    if (Id != 0) {
        glDeleteBuffers(1, &Id);
        Id = 0;
    }
}

bool VBO::load(const void *data, std::size_t size, GLenum usage) {
    release();
    glGenBuffers(1, &Id);
    if (Id == 0) return false;
    glBindBuffer(GL_ARRAY_BUFFER, Id);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    return true;
}

void VBO::bind() const { glBindBuffer(GL_ARRAY_BUFFER, Id); }

void VBO::unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

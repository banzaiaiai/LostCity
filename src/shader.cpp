#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
    : ID(createProgram(vertexPath, fragmentPath)) {}

void Shader::use() const {
    glUseProgram(ID);
}

std::string Shader::readFile(const char* path) {
    std::ifstream file; 
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path); 
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    } catch (const std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ (" << path << ")\n"
                  << e.what() << std::endl;
        return {};
    }
}

unsigned int Shader::compileShader(GLenum type, const char* path) {
    std::string code = readFile(path);
    const char* src = code.c_str();

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED (" << path << ")\n"
                  << infoLog << std::endl;
    }
    return shader;
}

unsigned int Shader::createProgram(const char* vertPath, const char* fragPath) {
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertPath);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragPath);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

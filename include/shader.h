#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, const float x, const float y, const float z) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    static std::string readFile(const char* path);
    static unsigned int compileShader(GLenum type, const char* path);
    static unsigned int createProgram(const char* vertPath, const char* fragPath);
};

#endif

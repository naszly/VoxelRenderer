
#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

void Shader::setInt(const char *name, int value) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniform1i(loc, value);
    }
}

void Shader::setIVec2(const char *name, const glm::ivec2 &vec) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniform2iv(loc, 1, glm::value_ptr(vec));
    }
}

void Shader::setIVec3(const char *name, const glm::ivec3 &vec) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniform3iv(loc, 1, glm::value_ptr(vec));
    }
}

void Shader::setIVec4(const char *name, const glm::ivec4 &vec) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniform4iv(loc, 1, glm::value_ptr(vec));
    }
}

void Shader::setFloat(const char *name, float value) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniform1f(loc, value);
    }
}

void Shader::setVec2(const char *name, const glm::vec2 &vec) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniform2fv(loc, 1, glm::value_ptr(vec));
    }
}

void Shader::setVec3(const char *name, const glm::vec3 &vec) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniform3fv(loc, 1, glm::value_ptr(vec));
    }
}

void Shader::setVec4(const char *name, const glm::vec4 &vec) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniform4fv(loc, 1, glm::value_ptr(vec));
    }
}

void Shader::setMat2(const char *name, const glm::mat2 &mat) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

void Shader::setMat3(const char *name, const glm::mat3 &mat) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

void Shader::setMat4(const char *name, const glm::mat4 &mat) const {
    int loc;
    if (findUniformLocation(name, &loc)) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

GLenum Shader::getShaderType(const char *path) {
    std::string extension = path;
    extension = extension.substr(extension.find_last_of('.') + 1);

    if (extension == "vert") {
        return GL_VERTEX_SHADER;
    } else if (extension == "frag") {
        return GL_FRAGMENT_SHADER;
    } else if (extension == "geom") {
        return GL_GEOMETRY_SHADER;
    } else if (extension == "tesc") {
        return GL_TESS_CONTROL_SHADER;
    } else if (extension == "tese") {
        return GL_TESS_EVALUATION_SHADER;
    } else if (extension == "comp") {
        return GL_COMPUTE_SHADER;
    } else {
        std::cout << "ERROR::SHADER::UNKNOWN_SHADER_TYPE: " << path << std::endl;
        return -1;
    }
}

void Shader::setBuffer(const char *name, const Buffer &buffer, unsigned int binding) const {
    GLuint uniformBlockIndex = glGetUniformBlockIndex(m_id, name);
    glUniformBlockBinding(m_id, uniformBlockIndex, binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer.getId());
}

GLuint Shader::createFromFile(const char *path) {
    std::string code;
    std::ifstream shaderFile;
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        shaderFile.open(path);
        std::stringstream shaderStream;
        // read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();
        // convert stream into string
        code = shaderStream.str();
    }
    catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    const char *shaderCode = code.c_str();

    return compileShader(shaderCode, getShaderType(path));
}

GLuint Shader::compileShader(const char *code, GLenum type) {
    unsigned int shader;
    // shader
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader, "SHADER");

    return shader;
}

void Shader::checkCompileErrors(unsigned int shader, std::string_view type) {
    int success;
    char infoLog[4096];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 4096, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 4096, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
    }
    assert(success);
}

void Shader::getUniforms() {
    GLint i;
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 48; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &count);

    for (i = 0; i < count; i++) {
        glGetActiveUniform(m_id, (GLuint) i, bufSize, &length, &size, &type, name);
        m_uniformLocations[name] = i;
    }
}

bool Shader::findUniformLocation(const char *str, int *location) const {
    auto iterator = m_uniformLocations.find(str);

    if (iterator == m_uniformLocations.end()) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << str << std::endl;
        return false;
    } else {
        *location = iterator->second;
        return true;
    }
}

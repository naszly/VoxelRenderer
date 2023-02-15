#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

class Shader {
public:
    Shader() = default;

    ~Shader() {
        deleteShader();
    }

    template<typename... Args>
    void init(Args... args) {
        createFromFiles(args...);
        getUniforms();
    }

    void use() const {
        glUseProgram(m_id);
    }

    void deleteShader() const {
        glDeleteProgram(m_id);
    }

    void setInt(const char *name, int value) const;

    void setIVec2(const char *name, const glm::ivec2 &vec) const;

    void setIVec3(const char *name, const glm::ivec3 &vec) const;

    void setIVec4(const char *name, const glm::ivec4 &vec) const;

    void setFloat(const char *name, float value) const;

    void setVec2(const char *name, const glm::vec2 &vec) const;

    void setVec3(const char *name, const glm::vec3 &vec) const;

    void setVec4(const char *name, const glm::vec4 &vec) const;

    void setMat2(const char *name, const glm::mat2 &mat) const;

    void setMat3(const char *name, const glm::mat3 &mat) const;

    void setMat4(const char *name, const glm::mat4 &mat) const;

private:
    GLuint m_id{0};

    std::unordered_map<std::string, int> m_uniformLocations;

    static GLenum getShaderType(const char *path);

    template<typename... Args>
    void createFromFiles(Args... args) {
        std::initializer_list<GLuint> shaders = {
            createFromFile(args)...
        };

        m_id = glCreateProgram();

        for (auto shader: shaders)
            glAttachShader(m_id, shader);

        glLinkProgram(m_id);
        checkCompileErrors(m_id, "PROGRAM");

        for (auto shader: shaders)
            glDeleteShader(shader);
    }

    static GLuint createFromFile(const char *path);

    static GLuint compileShader(const char *code, GLenum type);

    static void checkCompileErrors(unsigned int shader, std::string_view type);

    void getUniforms();

    bool findUniformLocation(const char *str, int *location) const;
};

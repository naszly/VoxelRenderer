#pragma once

#include <GL/glew.h>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <iostream>

class TextureArray {
public:
    template<typename... Textures>
    explicit TextureArray(Textures... texturePaths) {
        std::vector<GLubyte> data;
        loadTextures(data, texturePaths...);

        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, m_channels == 4 ? GL_RGBA8 : GL_RGB8, m_width, m_height, m_layers);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, m_width, m_height, m_layers,
                        m_channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data.data());
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    ~TextureArray() {
        glDeleteTextures(1, &m_id);
    }

    TextureArray(const TextureArray &other) = delete;

    TextureArray &operator=(const TextureArray &other) = delete;

    [[nodiscard]] GLuint getId() const {
        return m_id;
    }

    [[nodiscard]] int getWidth() const {
        return m_width;
    }

    [[nodiscard]] int getHeight() const {
        return m_height;
    }

    [[nodiscard]] int getChannels() const {
        return m_channels;
    }

    [[nodiscard]] int getLayers() const {
        return m_layers;
    }

private:
    GLuint m_id{0};
    int m_width{0};
    int m_height{0};
    int m_channels{0};
    int m_layers{0};

    static unsigned char *loadTexture(std::string_view path, int &width, int &height, int &channels);

    static void freeTexture(unsigned char *data);

    template<typename... TexturePaths>
    void loadTextures(std::vector<GLubyte> &data, std::string_view texturePath, TexturePaths... texturePaths) {
        int width = 0;
        int height = 0;
        int channels = 0;

        unsigned char *textureData = loadTexture(texturePath, width, height, channels);

        if (m_width == 0) {
            m_width = width;
            m_height = height;
            m_channels = channels;
            m_layers = sizeof...(texturePaths) + 1;
            data.reserve(m_width * m_height * channels * m_layers);
        } else if (m_width != width || m_height != height || m_channels != channels) {
            throw std::runtime_error("TextureArray: all textures must be the same size");
        }

        if (textureData) {
            data.insert(data.end(), textureData, textureData + width * height * channels);
        } else {
            throw std::runtime_error("TextureArray: failed to load texture");
        }

        freeTexture(textureData);

        if constexpr (sizeof...(texturePaths) > 0) {
            loadTextures(data, texturePaths...);
        }
    }

};

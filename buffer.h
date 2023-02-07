#pragma once

#include <GL/glew.h>

#include <vector>

enum class BufferUsage {
    StreamDraw = GL_STREAM_DRAW,
    StreamRead = GL_STREAM_READ,
    StreamCopy = GL_STREAM_COPY,
    StaticDraw = GL_STATIC_DRAW,
    StaticRead = GL_STATIC_READ,
    StaticCopy = GL_STATIC_COPY,
    DynamicDraw = GL_DYNAMIC_DRAW,
    DynamicRead = GL_DYNAMIC_READ,
    DynamicCopy = GL_DYNAMIC_COPY
};

class Buffer {
public:
    explicit Buffer(BufferUsage bufferUsage = BufferUsage::StaticDraw) : m_usage(bufferUsage) {
        glCreateBuffers(1, &m_id);
    }

    ~Buffer() {
        glDeleteBuffers(1, &m_id);
    }

    Buffer(const Buffer &other) = delete;
    Buffer &operator=(const Buffer &other) = delete;

    [[nodiscard]] GLuint getId() const {
        return m_id;
    }

    [[nodiscard]] GLsizeiptr getSize() const {
        return m_size;
    }

    [[nodiscard]] GLsizeiptr getCapacity() const {
        return m_capacity;
    }

    template<typename T>
    void setData(const std::vector<T> &data) {
        setData(data.data(), data.size() * sizeof(T));
    }

    template<typename T>
    void setData(const T *data, GLsizeiptr n) {
        m_size = n;
        if (m_size > m_capacity) {
            glNamedBufferData(m_id, m_size, data, static_cast<GLenum>(m_usage));
            m_capacity = m_size;
        } else {
            glNamedBufferSubData(m_id, 0, m_size, data);
        }
    }

private:
    GLuint m_id{0};
    GLsizeiptr m_size{0};
    GLsizeiptr m_capacity{0};
    BufferUsage m_usage;
};
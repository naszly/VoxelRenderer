#pragma once

#include <GL/glew.h>

#include <initializer_list>

#include "buffer.h"

enum class VertexType {
    Byte = GL_BYTE,
    UnsignedByte = GL_UNSIGNED_BYTE,
    Short = GL_SHORT,
    UnsignedShort = GL_UNSIGNED_SHORT,
    Int = GL_INT,
    UnsignedInt = GL_UNSIGNED_INT,
    Double = GL_DOUBLE,
    Float = GL_FLOAT,
    HalfFloat = GL_HALF_FLOAT,
    Fixed = GL_FIXED
};

enum class VertexInternalType {
    Float,
    Double,
    Int
};

struct VertexArrayAttrib {
public:
    constexpr VertexArrayAttrib(int location, VertexType type, int count,
                      VertexInternalType internalType = VertexInternalType::Float,
                      bool normalized = false) :
        location(location), type(static_cast<int>(type)), size(getSize(type) * count),
        numOfComponents(count), internalType(internalType), normalized(normalized) {}

private:
    friend class VertexArray;
    const int location;
    const int type;
    const int size;
    const int numOfComponents;
    const VertexInternalType internalType;
    const bool normalized;

    void setVertexArrayAttribFormat(GLuint id, GLuint offset) const;

    static constexpr int getSize(VertexType type) {
        switch (type) {
            case VertexType::Byte:
            case VertexType::UnsignedByte:
                return 1;
            case VertexType::Short:
            case VertexType::UnsignedShort:
            case VertexType::HalfFloat:
                return 2;
            case VertexType::Int:
            case VertexType::UnsignedInt:
            case VertexType::Float:
            case VertexType::Fixed:
                return 4;
            case VertexType::Double:
                return 8;
        }
        return 0;
    }
};

class VertexArray {
public:
    VertexArray();

    ~VertexArray();

    void bind() const;

    void pushVertexBuffer(const Buffer &vb,
                          std::initializer_list<const VertexArrayAttrib> attributes,
                          GLuint divisor = 0);

    void setElementBuffer(const Buffer &eb) const;

private:
    GLuint m_id{0};
    GLuint m_bindings{0};
};


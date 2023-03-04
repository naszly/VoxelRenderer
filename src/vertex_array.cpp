
#include "vertex_array.h"

#include <GL/glew.h>

void VertexArrayAttrib::setVertexArrayAttribFormat(GLuint id, GLuint offset) const {
    glEnableVertexArrayAttrib(id, location);
    switch (internalType) {
        case VertexInternalType::Int:
            glVertexArrayAttribIFormat(id, location, numOfComponents, type, offset);
            break;
        case VertexInternalType::Float:
            glVertexArrayAttribFormat(id, location, numOfComponents, type, normalized, offset);
            break;
        case VertexInternalType::Double:
            glVertexArrayAttribLFormat(id, location, numOfComponents, type, offset);
            break;
    }
}

VertexArray::VertexArray() {
    glCreateVertexArrays(1, &m_id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_id);
}

void VertexArray::bind() const {
    glBindVertexArray(m_id);
}

void VertexArray::pushVertexBuffer(const Buffer &vb,
                                   std::initializer_list<const VertexArrayAttrib> attributes,
                                   GLuint divisor) {
    GLsizei offset = 0;
    for (auto &attrib: attributes) {
        attrib.setVertexArrayAttribFormat(m_id, offset);
        offset += attrib.size;
    }

    glVertexArrayVertexBuffer(m_id, m_bindings, vb.getId(), 0, offset);

    for (auto &attrib: attributes) {
        glVertexArrayAttribBinding(m_id, attrib.location, m_bindings);
    }

    glVertexArrayBindingDivisor(m_id, m_bindings, divisor);

    ++m_bindings;
}

void VertexArray::setElementBuffer(const Buffer &eb) const {
    glVertexArrayElementBuffer(m_id, eb.getId());
}
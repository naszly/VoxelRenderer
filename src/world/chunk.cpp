
#include "chunk.h"

Chunk::Chunk() : m_vertexBuffer(BufferUsage::DynamicDraw) {
    static Buffer m_billboardVertexBuffer;
    constexpr float r = 1.73205080757f / 2.0f;
    constexpr float billboardVertices[] {
        -r, r, 0.0f,
        -r, -r, 0.0f,
        r, -r, 0.0f,
        r, -r, 0.0f,
        r, r, 0.0f,
        -r, r, 0.0f
    };

    if (m_billboardVertexBuffer.getSize() == 0) {
        m_billboardVertexBuffer.setData(billboardVertices, sizeof(billboardVertices));
    }

    m_vertexArray.pushVertexBuffer(m_billboardVertexBuffer, {
        VertexArrayAttrib(0, VertexType::Float, 3, VertexInternalType::Float) // billboard vertices
    }, 0);

    m_vertexArray.pushVertexBuffer(m_vertexBuffer, {
        VertexArrayAttrib(1, VertexType::UnsignedInt, 1, VertexInternalType::Int), // voxel position
        VertexArrayAttrib(2, VertexType::UnsignedInt, 1, VertexInternalType::Int) // voxel color
    }, 1);
}

void Chunk::fillRandom(float density) {
    for (int i = 0; i < CHUNK_SIZE_CUBED; ++i) {
        if (rand() % 1000 < density * 1000) {
            m_voxels.emplace(indexToPosition(i),
                             glm::vec3(rand() % 256 / 255.0f, rand() % 256 / 255.0f, rand() % 256 / 255.0f));
        }
    }
    m_dirty = true;
}

glm::vec3 Chunk::getVoxel(const glm::ivec3 &position) {
    auto it = m_voxels.find(Voxel(position, glm::vec3(0.0f)));
    return it != m_voxels.end() ? it->getColor() : glm::vec3(-1.0f);
}

void Chunk::addVoxel(const glm::ivec3 &position, const glm::vec3 &color) {
    m_voxels.emplace(position, color);
    m_dirty = true;
}

bool Chunk::removeVoxel(const glm::ivec3 &position) {
    if (m_voxels.erase(Voxel(position, glm::vec3(0.0f))) > 0) {
        m_dirty = true;
        return true;
    }
    return false;
}

bool Chunk::isVoxelEmpty(const glm::ivec3 &position) {
    return getVoxel(position).x < 0.0f;
}

void Chunk::upload() {
    std::vector<Voxel> voxels(m_voxels.begin(), m_voxels.end());
    m_vertexBuffer.setData(voxels);
    m_count = static_cast<GLsizei>(voxels.size());
    m_dirty = false;
}

void Chunk::render() {
    m_vertexArray.bind();
    if (m_dirty)
        upload();
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_count);
}


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
        VertexArrayAttrib(2, VertexType::UnsignedInt, 1, VertexInternalType::Int) // voxel material
    }, 1);
}

void Chunk::fill(const std::function<std::optional<Voxel>(glm::ivec3)> &func) {
    for (int i = 0; i < CHUNK_SIZE_CUBED; ++i) {
        auto pos = indexToPosition(i);
        auto voxel = func(pos);
        if (voxel) {
            assert(!voxel->isEmpty());
            m_voxels.emplace(voxel.value());
        }
    }
    m_dirty = true;
}

Voxel Chunk::getVoxel(const glm::ivec3 &position) {
    auto it = m_voxels.find(Voxel(position, 0));
    if (it != m_voxels.end()) {
        return *it;
    }
    return Voxel(position);
}

void Chunk::addVoxel(const Voxel &voxel) {
    assert(!voxel.isEmpty());
    m_voxels.emplace(voxel);
    m_dirty = true;
}

bool Chunk::removeVoxel(const glm::ivec3 &position) {
    if (m_voxels.erase(Voxel(position)) > 0) {
        m_dirty = true;
        return true;
    }
    return false;
}

bool Chunk::isVoxelEmpty(const glm::ivec3 &position) {
    return getVoxel(position).isEmpty();
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

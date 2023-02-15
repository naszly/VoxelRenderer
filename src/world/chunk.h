#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <set>
#include <algorithm>
#include <functional>
#include <optional>

#include "voxel.h"
#include "buffer.h"
#include "vertex_array.h"

constexpr int CHUNK_SIZE = 64;
constexpr int CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_SIZE_CUBED = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

class Chunk {
public:
    Chunk() : m_vertexBuffer(BufferUsage::DynamicDraw) {
        m_vertexArray.pushVertexBuffer(m_vertexBuffer, {
            VertexArrayAttrib(0, VertexType::UnsignedInt, 1, VertexInternalType::Int),
            VertexArrayAttrib(1, VertexType::UnsignedInt, 1, VertexInternalType::Int),
            VertexArrayAttrib(2, VertexType::UnsignedInt, 1, VertexInternalType::Int)
        });
    }

    void fill(const std::function<std::optional<Voxel>(glm::ivec3)>& func) {
        for (int i = 0; i < CHUNK_SIZE_CUBED; ++i) {
            auto pos = indexToPosition(i);
            auto voxel = func(pos);
            if (voxel) {
                m_voxels.emplace(voxel.value());
            }
        }
        m_dirty = true;
    }

    glm::vec3 getVoxel(const glm::ivec3 &position) {
        auto it = m_voxels.find(Voxel(position, glm::vec3(0.0f)));
        return it != m_voxels.end() ? it->getColor() : glm::vec3(-1.0f);
    }

    void addVoxel(const glm::ivec3 &position, const glm::vec3 &color) {
        m_voxels.emplace(position, color);
        m_dirty = true;
    }

    bool removeVoxel(const glm::ivec3 &position) {
        if (m_voxels.erase(Voxel(position, glm::vec3(0.0f))) > 0) {
            m_dirty = true;
            return true;
        }
        return false;
    }

    bool isVoxelEmpty(const glm::ivec3 &position) {
        return getVoxel(position).x < 0.0f;
    }

    void upload() {
        std::vector<Voxel> voxels(m_voxels.begin(), m_voxels.end());
        m_vertexBuffer.setData(voxels);
        m_count = static_cast<GLsizei>(voxels.size());
        m_dirty = false;
    }

    void render() {
        m_vertexArray.bind();
        if (m_dirty)
            upload();
        glDrawArrays(GL_POINTS, 0, m_count);
    }

    [[nodiscard]] size_t getVoxelCount() const {
        return m_voxels.size();
    }

private:
    std::set<Voxel, Voxel::Compare> m_voxels;
    Buffer m_vertexBuffer;
    VertexArray m_vertexArray;
    GLsizei m_count{0};
    bool m_dirty{false};

    static glm::vec3 indexToPosition(int index) {
        return {index / CHUNK_SIZE_SQUARED,
                (index % CHUNK_SIZE_SQUARED) / CHUNK_SIZE,
                index % CHUNK_SIZE};
    }
};

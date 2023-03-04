#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <set>
#include <algorithm>

#include "voxel.h"
#include "buffer.h"
#include "vertex_array.h"

constexpr int CHUNK_SIZE = 64;
constexpr int CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_SIZE_CUBED = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

class Chunk {
public:
    Chunk();

    void fillRandom(float density);

    glm::vec3 getVoxel(const glm::ivec3 &position);

    void addVoxel(const glm::ivec3 &position, const glm::vec3 &color);

    bool removeVoxel(const glm::ivec3 &position);

    bool isVoxelEmpty(const glm::ivec3 &position);

    void upload();

    void render();

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

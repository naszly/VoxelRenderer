#pragma once

#include <glm/glm.hpp>

#include <unordered_map>
#include <memory>

#include "voxel.h"
#include "chunk.h"
#include "shader.h"

namespace std {
    template<>
    struct hash<glm::ivec3> {
        std::size_t operator()(const glm::ivec3 &k) const {
            return ((std::hash<int>()(k.x)
                     ^ (std::hash<int>()(k.y) << 1)) >> 1)
                   ^ (std::hash<int>()(k.z) << 1);
        }
    };
}

class World {
public:
    void addChunk(const glm::ivec3 &position, const std::shared_ptr<Chunk> &chunk) {
        m_chunks.emplace(position, chunk);
        chunk->upload();
    }

    void render(const Shader &shader) {
        shader.setFloat("uChunkSize", CHUNK_SIZE);
        for (auto &chunk: m_chunks) {
            shader.setVec3("uChunkPosition", glm::vec3(chunk.first));
            chunk.second->render();
        }
    }

    bool removeVoxel(const glm::ivec3 &position) {
        glm::ivec3 chunkPosition = getChunkPosition(position);
        auto chunk = m_chunks.find(chunkPosition);
        if (chunk != m_chunks.end()) {
            glm::ivec3 localPosition = getLocalPosition(position);
            return chunk->second->removeVoxel(localPosition);
        }
        return false;
    }

    void addVoxel(const glm::ivec3 &position, uint32_t material) {
        glm::ivec3 chunkPosition = getChunkPosition(position);
        glm::ivec3 localPosition = getLocalPosition(position);
        auto chunk = m_chunks.find(chunkPosition);
        if (chunk != m_chunks.end()) {
            chunk->second->addVoxel(Voxel{localPosition, material});
        } else {
            auto newChunk = std::make_shared<Chunk>();
            newChunk->addVoxel(Voxel{localPosition, material});
            addChunk(chunkPosition, newChunk);
        }
    }

    bool isVoxelEmpty(const glm::ivec3 &position) {
        glm::ivec3 chunkPosition = getChunkPosition(position);
        auto chunk = m_chunks.find(chunkPosition);
        if (chunk != m_chunks.end()) {
            glm::ivec3 localPosition = getLocalPosition(position);
            return chunk->second->isVoxelEmpty(localPosition);
        }
        return true;
    }

    size_t getVoxelCount() {
        size_t count = 0;
        for (auto &chunk: m_chunks) {
            count += chunk.second->getVoxelCount();
        }
        return count;
    }

private:
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_chunks;

    static int mod(int k, int n) {
        return ((k %= n) < 0) ? k + n : k;
    }

    static glm::ivec3 mod(const glm::ivec3 &k, int n) {
        return {mod(k.x, n), mod(k.y, n), mod(k.z, n)};
    }

    static glm::ivec3 getChunkPosition(const glm::ivec3 &position) {
        return glm::floor(glm::vec3(position) / (float) CHUNK_SIZE);
    }

    static glm::ivec3 getLocalPosition(const glm::ivec3 &position) {
        return mod(position, CHUNK_SIZE);
    }
};

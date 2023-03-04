#pragma once

#include <glm/glm.hpp>

constexpr uint32_t EMPTY_VOXEL = std::numeric_limits<uint32_t>::max();

class Voxel {
public:
    Voxel() = default;

    Voxel(const glm::uvec3 &position, uint32_t materialID) : m_materialID(materialID) {
        setPosition(position);
    }

    explicit Voxel(const glm::uvec3 &position) : m_materialID(EMPTY_VOXEL) {
        setPosition(position);
    }

    void setPosition(const glm::uvec3 &position) {
        // x 10 bits, y 10 bits, z 10 bits
        assert(position.x < 1024 && position.y < 1024 && position.z < 1024);
        m_position = (position.x << 20) | (position.y << 10) | position.z;
    }

    [[nodiscard]] glm::uvec3 getPosition() const {
        return {(m_position >> 20) & 0x3FF, (m_position >> 10) & 0x3FF, m_position & 0x3FF};
    }

    [[nodiscard]] bool isEmpty() const {
        return m_materialID == EMPTY_VOXEL;
    }

    [[nodiscard]] uint32_t getMaterialID() const {
        return m_materialID;
    }

    struct Compare {
        bool operator()(const Voxel &a, const Voxel &b) const {
            return a.m_position < b.m_position;
        }
    };

private:
    uint32_t m_position{0};
    uint32_t m_materialID{EMPTY_VOXEL};
};
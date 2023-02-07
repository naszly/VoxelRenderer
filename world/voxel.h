#pragma once

#include <glm/glm.hpp>

class Voxel {
public:
    Voxel() = default;

    Voxel(const glm::uvec3 &position, const glm::vec3 &color) {
        setPosition(position);
        setColor(color);
    }

    void setPosition(const glm::uvec3 &position) {
        // x 10 bits, y 10 bits, z 10 bits
        assert(position.x < 1024 && position.y < 1024 && position.z < 1024);
        m_position = (position.x << 20) | (position.y << 10) | position.z;
    }

    [[nodiscard]] glm::uvec3 getPosition() const {
        return {(m_position >> 20) & 0x3FF, (m_position >> 10) & 0x3FF, m_position & 0x3FF};
    }

    void setColor(const glm::vec3 &color) {
        glm::vec3 c = glm::clamp(color, 0.0f, 1.0f);
        m_color = (uint32_t(c.r * 255) << 24) |
                  (uint32_t(c.g * 255) << 16) |
                  (uint32_t(c.b * 255) << 8) |
                  0xFF;
    }

    [[nodiscard]] glm::vec3 getColor() const {
        return glm::vec3((m_color >> 24) & 0xFF, (m_color >> 16) & 0xFF, (m_color >> 8) & 0xFF) / 255.0f;
    }

    struct Compare {
        bool operator()(const Voxel &a, const Voxel &b) const {
            return a.m_position < b.m_position;
        }
    };

private:
    uint32_t m_position{0};
    uint32_t m_color{0};
};
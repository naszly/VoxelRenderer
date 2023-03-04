#pragma once

#include <cstdint>
#include <glm/vec4.hpp>

struct Material {
    glm::vec4 color;
    uint32_t texture;
    uint32_t padding[3]{};

    Material(glm::vec4 color, uint32_t texture) : color(color), texture(texture) {}

    explicit Material(glm::vec4 color) : color(color), texture(UINT32_MAX) {}

    bool operator==(const Material &other) const {
        return color == other.color && texture == other.texture;
    }
};
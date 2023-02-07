#pragma once

#include <glm/glm.hpp>

#include <functional>

#include "camera.h"
#include "world/world.h"

struct GLFWwindow;

class PlayerController {
public:
    PlayerController(Camera &camera, World &world, GLFWwindow *window);

    void update(float deltaTime);

    [[nodiscard]] float getReach() const {
        return m_reach;
    }

private:
    Camera &m_camera;
    World &m_world;
    GLFWwindow *m_window;

    const glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const float m_cameraSpeed = 10.0f;
    const float m_reach = 50.0f;

    typedef std::function<bool(glm::ivec3, glm::ivec3)> RayHitCallbackFn;

    static void castRay(glm::vec3 position, glm::vec3 direction, float length, const RayHitCallbackFn &callback);
};

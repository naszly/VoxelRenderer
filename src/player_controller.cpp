
#include "player_controller.h"

#include <GLFW/glfw3.h>

PlayerController::PlayerController(Camera &camera, World &world, GLFWwindow *window)
    : m_camera(camera), m_world(world), m_window(window) {

    glfwSetWindowUserPointer(m_window, &m_camera);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, double xPos, double yPos) {
        static double lastX = xPos;
        static double lastY = yPos;
        static bool firstMouse = true;

        if (firstMouse) {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
        }

        double xOffset = xPos - lastX;
        double yOffset = lastY - yPos;
        lastX = xPos;
        lastY = yPos;

        double sensitivity = 0.05f;
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        static double yaw = 90.0f;
        static double pitch = 0.0f;

        yaw += xOffset;
        pitch += yOffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::dvec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        auto *camera = (Camera *) glfwGetWindowUserPointer(window);

        camera->setDirection(glm::normalize(front));
    });
}

void PlayerController::update(float deltaTime) {
    float speed = m_cameraSpeed * deltaTime;

    glm::vec3 position = m_camera.getPosition();

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        position += glm::normalize(glm::vec3(m_camera.getDirection().x, .0f, m_camera.getDirection().z)) * speed;
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        position -= glm::normalize(glm::vec3(m_camera.getDirection().x, .0f, m_camera.getDirection().z)) * speed;
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(m_camera.getDirection(), m_cameraUp)) * speed;
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(m_camera.getDirection(), m_cameraUp)) * speed;
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += m_cameraUp * speed;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        position -= m_cameraUp * speed;

    m_camera.setPosition(position);

    static bool leftMousePressed = false;
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!leftMousePressed) {
            leftMousePressed = true;
            castRay(position, glm::normalize(m_camera.getDirection()), m_reach, [&](glm::ivec3 pos, glm::ivec3 prevPos) {
                return m_world.removeVoxel(pos);
            });
        }
    } else {
        leftMousePressed = false;
    }

    static bool rightMousePressed = false;
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (!rightMousePressed) {
            rightMousePressed = true;
            castRay(position, glm::normalize(m_camera.getDirection()), m_reach, [&](glm::ivec3 pos, glm::ivec3 prevPos) {
                if (m_world.isVoxelEmpty(pos))
                    return false;

                m_world.addVoxel(prevPos, 0);
                return true;
            });
        }
    } else {
        rightMousePressed = false;
    }
}

void PlayerController::castRay(glm::vec3 position, glm::vec3 direction, float length,
                               const PlayerController::RayHitCallbackFn &callback) {
    glm::ivec3 current = glm::floor(position);
    glm::ivec3 end = glm::ivec3(glm::floor(position + direction * length));
    glm::ivec3 sign = glm::sign(direction);

    glm::vec3 tMax = (glm::vec3(current) + glm::step(glm::vec3(0), direction) - position) / direction;
    glm::vec3 tDelta = glm::vec3(sign) / direction;

    if (glm::isnan(tMax.x)) tMax.x = std::numeric_limits<float>::infinity();
    if (glm::isnan(tMax.y)) tMax.y = std::numeric_limits<float>::infinity();
    if (glm::isnan(tMax.z)) tMax.z = std::numeric_limits<float>::infinity();

    glm::ivec3 previous = current;

    while (!callback(current, previous) && !(current.x == end.x && current.y == end.y && current.z == end.z)) {
        previous = current;
        if (tMax.x < tMax.y && tMax.x < tMax.z) {
            current.x += sign.x;
            tMax.x += tDelta.x;
        } else if (tMax.y < tMax.z) {
            current.y += sign.y;
            tMax.y += tDelta.y;
        } else {
            current.z += sign.z;
            tMax.z += tDelta.z;
        }
    }
}


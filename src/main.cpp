
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "shader.h"
#include "world/world.h"
#include "camera.h"
#include "player_controller.h"

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 1000.0f;


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "VoxelRayTracer", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);


    Camera camera;
    camera.setPerspective(glm::radians(60.0f), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
    camera.setDirection(glm::vec3(0.0f, 0.0f, 1.0f));


    World world;
    std::shared_ptr<Chunk> chunk1 = std::make_shared<Chunk>();
    chunk1->fillRandom(0.1);
    world.addChunk(glm::ivec3(0, 0, 0), chunk1);


    PlayerController cameraController(camera, world, window);


    Shader screenShader;
    screenShader.init("shaders/screen.vert", "shaders/screen.frag");
    screenShader.use();
    screenShader.setVec2("uViewportSize", glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
    //screenShader.setFloat("uNearPlane", NEAR_PLANE);
    //screenShader.setFloat("uFarPlane", FAR_PLANE);
    screenShader.setFloat("uReach", cameraController.getReach());


    while (!glfwWindowShouldClose(window)) {
        static auto lastTime = glfwGetTime();
        auto currentTime = glfwGetTime();
        auto deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // print fps every second
        static int frameCount = 0;
        static double lastFpsTime = 0.0;
        frameCount++;
        if (currentTime - lastFpsTime >= 1.0) {
            std::cout << "FPS: " << frameCount << " voxel count: " << world.getVoxelCount() << std::endl;
            frameCount = 0;
            lastFpsTime += 1.0;
        }

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        cameraController.update((float)deltaTime);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        screenShader.setMat4("uProjectionView", camera.getProjectionViewMatrix());
        screenShader.setMat4("uInvProjectionView", camera.getInverseProjectionViewMatrix());
        screenShader.setVec3("uCameraPosition", camera.getPosition());

        world.render(screenShader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

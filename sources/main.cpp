#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "window.h"
#include "terrain.h"
#include "shader_utils.h"
#include "camera.h"
#include "hiking_visualizer.h"
#include "math_utils.h"
#include "hiking_data.h"
#include "skybox.h"

// Global variables
Camera camera(glm::vec3(0.0f, 500.0f, 500.0f));
HikingVisualizer hikingVisualizer;
float lastX = 640.0f;
float lastY = 360.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool followHiker = false;

// Window dimensions
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.processMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);

    // Vertical movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.moveVertical(deltaTime, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.moveVertical(deltaTime, false);

    // Hiker speed control
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        hikingVisualizer.setHikerSpeed(hikingVisualizer.getHikeStats().currentSpeed * 1.1f);
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        hikingVisualizer.setHikerSpeed(hikingVisualizer.getHikeStats().currentSpeed * 0.9f);

    // Reset camera
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.setPosition(glm::vec3(50.0f, 30.0f, 50.0f));
    }

    // Toggle mouse cursor
    static bool mPressed = false;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (!mPressed) {
            if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            mPressed = true;
        }
    }
    else {
        mPressed = false;
    }

    // Toggle follow mode
    static bool fPressed = false;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!fPressed) {
            followHiker = !followHiker;
            fPressed = true;
        }
    }
    else {
        fPressed = false;
    }
}

int main() {
    // Initialize window
    Window window(SCR_WIDTH, SCR_HEIGHT, "OpenGL Hiking Simulator");
    if (!window.initialize()) {
        return -1;
    }

    // Set callbacks
    glfwSetCursorPosCallback(window.getGLFWwindow(), mouse_callback);
    glfwSetFramebufferSizeCallback(window.getGLFWwindow(), framebuffer_size_callback);
    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //https://github.com/fegennari/3DWorld/tree/master/textures/skybox/water_scene
   
    // Initialize skybox
    std::vector<std::string> faces{
        "A:/Taief/Project/OpenGL_Project/textures/skybox/right.jpg",
        "A:/Taief/Project/OpenGL_Project/textures/skybox/left.jpg",
        "A:/Taief/Project/OpenGL_Project/textures/skybox/top.jpg",
        "A:/Taief/Project/OpenGL_Project/textures/skybox/bottom.jpg",
        "A:/Taief/Project/OpenGL_Project/textures/skybox/front.jpg",
        "A:/Taief/Project/OpenGL_Project/textures/skybox/back.jpg"
    };

    Skybox skybox;
    if (!skybox.initialize(faces)) {
        std::cerr << "Failed to initialize skybox" << std::endl;
        return -1;
    }

    // Load hiking data
    std::vector<glm::vec3> hikingData;
    if (!loadHikingData("A:/Taief/Project/OpenGL_Project/data/Afternoon_Run.gpx", hikingData)) {
        std::cerr << "Failed to load hiking data" << std::endl;
        return -1;
    }
    std::cout << "Loaded " << hikingData.size() << " hiking points" << std::endl;

    // Initialize hiking visualizer
    if (!hikingVisualizer.initialize(hikingData)) {
        std::cerr << "Failed to initialize hiking visualizer" << std::endl;
        return -1;
    }

    // Set camera position near the first point of the trail
    if (!hikingData.empty()) {
        glm::vec3 firstPoint = hikingData.front();
        camera.setPosition(firstPoint + glm::vec3(0.0f, 50.0f, 150.0f)); // Adjust offsets as needed
    }



    // Initialize terrain
    Terrain terrain;
    if (!terrain.initialize(
        "A:/Taief/Project/OpenGL_Project/data/hoydedata_svarthvitt.png",
        "A:/Taief/Project/OpenGL_Project/textures/tex2.png",
        hikingData)) {
        std::cerr << "Failed to initialize terrain" << std::endl;
        return -1;
    }

    // Load shaders
    GLuint terrainShader = loadShaders(
        "A:/Taief/Project/OpenGL_Project/shaders/vertex_shader.glsl",
        "A:/Taief/Project/OpenGL_Project/shaders/fragment_shader.glsl");
    if (!terrainShader) {
        return -1;
    }

    // Set up matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
        0.1f,     // Near plane
        1000.0f   // Far plane (adjust if needed)
    );

    // Main loop
    while (!glfwWindowShouldClose(window.getGLFWwindow())) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window.getGLFWwindow());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update hiking visualization
        hikingVisualizer.update(deltaTime);

        // Update camera if following hiker
        if (followHiker) {
            glm::vec3 hikerPos = hikingVisualizer.getCurrentHikerPosition();
            camera.setPosition(hikerPos + glm::vec3(-150.0f, 100.0f, -150.0f));
        }

        // Get view matrix
        glm::mat4 view = camera.getViewMatrix();

        // Draw terrain
        terrain.draw(view, projection);

        // Draw hiking trail
        hikingVisualizer.draw(view, projection);

        // Draw skybox last
        glDepthFunc(GL_LEQUAL);  // Change depth function for skybox
        skybox.draw(view, projection);
        glDepthFunc(GL_LESS);    // Restore default depth function

        // Display stats
        auto stats = hikingVisualizer.getHikeStats();
        std::cout << "\rElevation: " << stats.currentElevation
            << "m | Completion: " << stats.completionPercentage
            << "% | Speed: " << stats.currentSpeed << " m/s" << std::flush;

        glfwSwapBuffers(window.getGLFWwindow());
        glfwPollEvents();
    }

    glDeleteProgram(terrainShader);
    glfwTerminate();
    return 0;
}
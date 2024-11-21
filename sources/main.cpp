#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "window.h"
#include "terrain.h"
#include "shader_utils.h"
#include "hiking_data.h"

// Function to check OpenGL errors
void checkGLError(const std::string& function) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error in " << function << ": " << gluErrorString(err) << std::endl;
    }
}

int main() {
    Window window(800, 600, "OpenGL Terrain");

    if (!window.initialize()) {
        return -1;
    }

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    checkGLError("glewInit");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    checkGLError("glEnable(GL_DEPTH_TEST)");

    std::vector<glm::vec3> hikingData;
    if (!loadHikingData("A:/Taief/Project/OpenGL_Project/data/Afternoon_Run.gpx", hikingData)) {
        std::cerr << "Failed to load hiking data" << std::endl;
        return -1;
    }
    std::cout << "Successfully loaded GPX data with " << hikingData.size() << " track points." << std::endl;

    Terrain terrain;
    if (!terrain.initialize("A:/Taief/Project/OpenGL_Project/data/hoydedata_svarthvitt.png",
        "A:/Taief/Project/OpenGL_Project/textures/tex2.png", hikingData)) {
        std::cerr << "Failed to initialize terrain" << std::endl;
        return -1;
    }
    checkGLError("terrain.initialize");

    terrain.debugOutput();  // Added this to output terrain information

    GLuint shaderProgram = loadShaders("A:/Taief/Project/OpenGL_Project/shaders/vertex_shader.glsl",
        "A:/Taief/Project/OpenGL_Project/shaders/fragment_shader.glsl");
    if (shaderProgram == 0) {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }
    glUseProgram(shaderProgram);
    checkGLError("glUseProgram(shaderProgram)");

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(40.0f, 50.0f, 40.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 200.0f);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    if (modelLoc == -1 || viewLoc == -1 || projLoc == -1) {
        std::cerr << "Failed to get uniform locations for transformation matrices." << std::endl;
        return -1;
    }

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Lighting properties
    GLint lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    if (lightDirLoc == -1 || lightColorLoc == -1 || objectColorLoc == -1) {
        std::cerr << "Failed to get uniform locations for lighting." << std::endl;
        return -1;
    }
    glm::vec3 lightDir(-1.0f, -1.0f, -0.5f); // Adjusted light direction for more depth
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 objectColor(0.3f, 0.6f, 0.2f); // More earthy green color

    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    glUniform3fv(objectColorLoc, 1, glm::value_ptr(objectColor));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    while (!glfwWindowShouldClose(window.getGLFWwindow())) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkGLError("glClear");

        glUseProgram(shaderProgram);
        checkGLError("glUseProgram(shaderProgram)");

        // Update transformations
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        terrain.draw();
        checkGLError("terrain.draw");

        glfwSwapBuffers(window.getGLFWwindow());
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

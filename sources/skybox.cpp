#include "skybox.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "../external/stb_image.h"

Skybox::Skybox() : skyboxShader(std::make_unique<Shader>()) {
}

Skybox::~Skybox() {
    cleanup();
}

void Skybox::cleanup() {
    if (skyboxVAO) glDeleteVertexArrays(1, &skyboxVAO);
    if (skyboxVBO) glDeleteBuffers(1, &skyboxVBO);
    if (cubemapTexture) glDeleteTextures(1, &cubemapTexture);
}

bool Skybox::initialize(const std::vector<std::string>& faces) {
    float skyboxVertices[] = {
        // positions          
        // Back face
        -1.0f,  1.0f, -1.0f, // Top-left
        -1.0f, -1.0f, -1.0f, // Bottom-left
         1.0f, -1.0f, -1.0f, // Bottom-right
         1.0f, -1.0f, -1.0f, // Bottom-right
         1.0f,  1.0f, -1.0f, // Top-right
        -1.0f,  1.0f, -1.0f, // Top-left
        // Front face
        -1.0f, -1.0f,  1.0f, // Bottom-left
        -1.0f,  1.0f,  1.0f, // Top-left
         1.0f,  1.0f,  1.0f, // Top-right
         1.0f,  1.0f,  1.0f, // Top-right
         1.0f, -1.0f,  1.0f, // Bottom-right
        -1.0f, -1.0f,  1.0f, // Bottom-left
        // Left face
        -1.0f,  1.0f,  1.0f, // Top-left
        -1.0f,  1.0f, -1.0f, // Top-right
        -1.0f, -1.0f, -1.0f, // Bottom-right
        -1.0f, -1.0f, -1.0f, // Bottom-right
        -1.0f, -1.0f,  1.0f, // Bottom-left
        -1.0f,  1.0f,  1.0f, // Top-left
        // Right face
         1.0f,  1.0f, -1.0f, // Top-left
         1.0f,  1.0f,  1.0f, // Top-right
         1.0f, -1.0f,  1.0f, // Bottom-right
         1.0f, -1.0f,  1.0f, // Bottom-right
         1.0f, -1.0f, -1.0f, // Bottom-left
         1.0f,  1.0f, -1.0f, // Top-left
         // Bottom face
         -1.0f, -1.0f, -1.0f, // Top-right
          1.0f, -1.0f, -1.0f, // Top-left
          1.0f, -1.0f,  1.0f, // Bottom-left
          1.0f, -1.0f,  1.0f, // Bottom-left
         -1.0f, -1.0f,  1.0f, // Bottom-right
         -1.0f, -1.0f, -1.0f, // Top-right
         // Top face
         -1.0f,  1.0f,  1.0f, // Top-left
          1.0f,  1.0f,  1.0f, // Top-right
          1.0f,  1.0f, -1.0f, // Bottom-right
          1.0f,  1.0f, -1.0f, // Bottom-right
         -1.0f,  1.0f, -1.0f, // Bottom-left
         -1.0f,  1.0f,  1.0f  // Top-left
    };

    // Load shaders
    if (!skyboxShader->load(
        "A:/Taief/Project/OpenGL_Project/shaders/skybox_vertex.glsl",
        "A:/Taief/Project/OpenGL_Project/shaders/skybox_fragment.glsl")) {
        return false;
    }

    // Create buffers
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    return loadCubemap(faces);
}

bool Skybox::loadCubemap(const std::vector<std::string>& faces) {
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
            return false;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

void Skybox::draw(const glm::mat4& view, const glm::mat4& projection) {
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();

    // Remove translation from view matrix
    glm::mat4 skyView = glm::mat4(glm::mat3(view));

    skyboxShader->setMat4("view", skyView);
    skyboxShader->setMat4("projection", projection);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

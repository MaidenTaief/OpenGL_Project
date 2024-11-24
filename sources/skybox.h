#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.h"

class Skybox {
public:
    Skybox();
    ~Skybox();

    // Delete copy constructor and assignment operator
    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    bool initialize(const std::vector<std::string>& faces);
    void draw(const glm::mat4& view, const glm::mat4& projection);
    void cleanup();

private:
    GLuint skyboxVAO{ 0 };
    GLuint skyboxVBO{ 0 };
    GLuint cubemapTexture{ 0 };
    std::unique_ptr<Shader> skyboxShader;

    bool loadCubemap(const std::vector<std::string>& faces);
};
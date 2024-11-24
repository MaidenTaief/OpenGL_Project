#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Terrain {
public:
    Terrain();
    ~Terrain();

    // Delete copy constructor and assignment operator
    Terrain(const Terrain&) = delete;
    Terrain& operator=(const Terrain&) = delete;

    bool initialize(const std::string& heightMapPath,
        const std::string& texturePath,
        const std::vector<glm::vec3>& hikingData);
    void draw(const glm::mat4& view, const glm::mat4& projection);
    void debugOutput() const;
    void cleanup();

    // Getters for terrain properties
    float getMinHeight() const { return minHeight; }
    float getMaxHeight() const { return maxHeight; }
    int getWidth() const { return terrainWidth; }
    int getHeight() const { return terrainHeight; }

private:
    // Mesh data
    std::unique_ptr<std::vector<Vertex>> vertices;
    std::unique_ptr<std::vector<unsigned int>> indices;

    // OpenGL objects
    GLuint VAO{ 0 };
    GLuint VBO{ 0 };
    GLuint EBO{ 0 };
    GLuint heightmapTexture{ 0 };
    GLuint terrainTexture{ 0 };
    std::unique_ptr<Shader> shader;

    // Terrain properties
    unsigned int numIndices{ 0 };
    float minHeight{ std::numeric_limits<float>::max() };
    float maxHeight{ std::numeric_limits<float>::lowest() };
    int terrainWidth{ 0 };
    int terrainHeight{ 0 };

    // Private methods
    bool loadHeightMap(const std::string& path, int& width, int& height,
        std::vector<unsigned char>& heightData);
    bool loadTexture(const std::string& path);
    void generateTerrainVertices(const std::vector<unsigned char>& heightData,
        int width, int height,
        const std::vector<glm::vec3>& hikingData);
    void generateTerrainIndices(int width, int height);
    void calculateNormals();
    void setupBuffers();
};
#include "terrain.h"
#include "hiking_data.h"
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Constructor
Terrain::Terrain() : VBO(0), VAO(0), EBO(0), textureID(0), numIndices(0) {}

// Destructor
Terrain::~Terrain() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureID);
}

// Initialization function with hiking data
bool Terrain::initialize(const std::string& heightMapPath, const std::string& texturePath, const std::vector<glm::vec3>& hikingData) {
    int width, height;
    std::vector<unsigned char> heightData;

    if (!loadHeightMap(heightMapPath, width, height, heightData)) {
        std::cerr << "Failed to load height map: " << heightMapPath << std::endl;
        return false;
    }

    generateTerrainVertices(heightData, width, height, hikingData);
    generateTerrainIndices(width, height);
    calculateNormals();

    if (!loadTexture(texturePath)) {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
        return false;
    }

    setupBuffers();

    return true;
}

// Load height map
bool Terrain::loadHeightMap(const std::string& path, int& width, int& height, std::vector<unsigned char>& heightData) {
    int nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 1);
    if (!data) {
        return false;
    }

    heightData.assign(data, data + (width * height));
    stbi_image_free(data);
    return true;
}

// Generate vertices for terrain using height data and hiking data
void Terrain::generateTerrainVertices(const std::vector<unsigned char>& heightData, int width, int height, const std::vector<glm::vec3>& hikingData) {
    const float HEIGHT_SCALE = 50.0f; // Increase to make elevation more visible
    const int STEP_SIZE = 1;

    vertices.clear();
    for (int y = 0; y < height; y += STEP_SIZE) {
        for (int x = 0; x < width; x += STEP_SIZE) {
            unsigned char pixel = heightData[y * width + x];
            float elevation = static_cast<float>(pixel) / 255.0f;

            // Adjust elevation based on hiking data if nearby track points exist
            float adjustedElevation = elevation * HEIGHT_SCALE;
            for (const auto& point : hikingData) {
                float distance = glm::distance(glm::vec2(x, y), glm::vec2(point.x, point.z));
                if (distance < 10.0f) { // Influence radius of 10 units
                    adjustedElevation = point.y; // Set elevation based on hiking data
                }
            }

            Vertex vertex;
            vertex.position = glm::vec3(static_cast<float>(x), adjustedElevation, static_cast<float>(y));
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vertex.texCoords = glm::vec2(static_cast<float>(x) / width, static_cast<float>(y) / height);

            vertices.push_back(vertex);
        }
    }
}

// Generate indices for terrain
void Terrain::generateTerrainIndices(int width, int height) {
    const int STEP_SIZE = 1;
    int reducedWidth = width / STEP_SIZE;
    int reducedHeight = height / STEP_SIZE;

    indices.clear();
    for (int y = 0; y < reducedHeight - 1; ++y) {
        for (int x = 0; x < reducedWidth - 1; ++x) {
            int topLeft = y * reducedWidth + x;
            int topRight = topLeft + 1;
            int bottomLeft = (y + 1) * reducedWidth + x;
            int bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    numIndices = static_cast<unsigned int>(indices.size());
}

// Calculate normals
void Terrain::calculateNormals() {
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        glm::vec3 v0 = vertices[i0].position;
        glm::vec3 v1 = vertices[i1].position;
        glm::vec3 v2 = vertices[i2].position;

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        vertices[i0].normal += normal;
        vertices[i1].normal += normal;
        vertices[i2].normal += normal;
    }

    for (auto& vertex : vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }
}

// Load texture
bool Terrain::loadTexture(const std::string& path) {
    int texWidth, texHeight, texChannels;
    unsigned char* texData = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, 0);
    if (!texData) {
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(texData);
    return true;
}

// Setup buffers
void Terrain::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// Draw function
void Terrain::draw() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

// Debug output function
void Terrain::debugOutput() {
    std::cout << "Number of vertices: " << vertices.size() << std::endl;
    std::cout << "Number of indices: " << indices.size() << std::endl;
}

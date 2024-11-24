#include "terrain.h"
#include <iostream>
#include <vector>
#include <limits>
#include "../external/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Terrain::Terrain()
    : vertices(std::make_unique<std::vector<Vertex>>())
    , indices(std::make_unique<std::vector<unsigned int>>())
    , shader(std::make_unique<Shader>()) {
}

Terrain::~Terrain() {
    cleanup();
}

bool Terrain::initialize(const std::string& heightMapPath,
    const std::string& texturePath,
    const std::vector<glm::vec3>& hikingData) {

    // Load shaders
    if (!shader->load(
        "A:/Taief/Project/OpenGL_Project/shaders/vertex_shader.glsl",
        "A:/Taief/Project/OpenGL_Project/shaders/fragment_shader.glsl")) {
        std::cerr << "Failed to load terrain shaders" << std::endl;
        return false;
    }

    int width, height;
    std::vector<unsigned char> heightData;

    if (!loadHeightMap(heightMapPath, width, height, heightData)) {
        std::cerr << "Failed to load height map: " << heightMapPath << std::endl;
        return false;
    }

    terrainWidth = width;
    terrainHeight = height;

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

bool Terrain::loadHeightMap(const std::string& path, int& width, int& height,
    std::vector<unsigned char>& heightData) {
    int nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 1);
    if (!data) {
        std::cerr << "Failed to load heightmap: " << stbi_failure_reason() << std::endl;
        return false;
    }

    heightData.assign(data, data + (width * height));
    stbi_image_free(data);
    return true;
}

bool Terrain::loadTexture(const std::string& path) {
    int texWidth, texHeight, texChannels;
    unsigned char* texData = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, 0);
    if (!texData) {
        std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
        return false;
    }

    glGenTextures(1, &terrainTexture);
    glBindTexture(GL_TEXTURE_2D, terrainTexture);

    GLenum format = texChannels == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(texData);
    return true;
}

void Terrain::generateTerrainVertices(const std::vector<unsigned char>& heightData,
    int width, int height,
    const std::vector<glm::vec3>& hikingData) {

    // Increase scale factors significantly
    const float HEIGHT_SCALE = 500.0f;    // Increase height scale
    const float TERRAIN_SCALE = 50.0f;    // Increase horizontal scale

    vertices->clear();
    vertices->reserve(width * height);

    minHeight = std::numeric_limits<float>::max();
    maxHeight = std::numeric_limits<float>::lowest();

    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float xPos = (x - width / 2) * TERRAIN_SCALE;   // Center the terrain
            float zPos = (z - height / 2) * TERRAIN_SCALE;  // Center the terrain

            unsigned char pixel = heightData[z * width + x];
            float elevation = static_cast<float>(pixel) / 255.0f * HEIGHT_SCALE;

            // Adjust elevation based on hiking data
            for (const auto& point : hikingData) {
                float distance = glm::distance(glm::vec2(xPos, zPos), glm::vec2(point.x, point.z));
                if (distance < 10.0f * TERRAIN_SCALE) {
                    elevation = point.y * HEIGHT_SCALE;
                }
            }

            minHeight = std::min(minHeight, elevation);
            maxHeight = std::max(maxHeight, elevation);

            Vertex vertex;
            vertex.position = glm::vec3(xPos, elevation, zPos);
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vertex.texCoords = glm::vec2(static_cast<float>(x) / width,
                static_cast<float>(z) / height);
            vertices->push_back(vertex);
        }
    }
}

void Terrain::generateTerrainIndices(int width, int height) {
    indices->clear();
    indices->reserve((width - 1) * (height - 1) * 6);

    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            unsigned int topLeft = z * width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * width + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices->push_back(topLeft);
            indices->push_back(bottomLeft);
            indices->push_back(topRight);
            indices->push_back(topRight);
            indices->push_back(bottomLeft);
            indices->push_back(bottomRight);
        }
    }

    numIndices = static_cast<unsigned int>(indices->size());
}

void Terrain::calculateNormals() {
    // Reset normals
    for (auto& vertex : *vertices) {
        vertex.normal = glm::vec3(0.0f);
    }

    // Calculate normals
    for (size_t i = 0; i < indices->size(); i += 3) {
        unsigned int i0 = (*indices)[i];
        unsigned int i1 = (*indices)[i + 1];
        unsigned int i2 = (*indices)[i + 2];

        glm::vec3 v1 = (*vertices)[i1].position - (*vertices)[i0].position;
        glm::vec3 v2 = (*vertices)[i2].position - (*vertices)[i0].position;
        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

        (*vertices)[i0].normal += normal;
        (*vertices)[i1].normal += normal;
        (*vertices)[i2].normal += normal;
    }

    // Normalize
    for (auto& vertex : *vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }
}

void Terrain::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), vertices->data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Terrain::draw(const glm::mat4& view, const glm::mat4& projection) {
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTexture);
    shader->setInt("terrainTexture", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Terrain::debugOutput() const {
    std::cout << "\nTerrain Debug Information:" << std::endl;
    std::cout << "Number of vertices: " << vertices->size() << std::endl;
    std::cout << "Number of indices: " << indices->size() << std::endl;
    std::cout << "Min height: " << minHeight << std::endl;
    std::cout << "Max height: " << maxHeight << std::endl;
    std::cout << "Terrain dimensions: " << terrainWidth << "x" << terrainHeight << std::endl;

    glm::vec3 minBounds(std::numeric_limits<float>::max());
    glm::vec3 maxBounds(std::numeric_limits<float>::lowest());

    for (const auto& vertex : *vertices) {
        minBounds = glm::min(minBounds, vertex.position);
        maxBounds = glm::max(maxBounds, vertex.position);
    }

    std::cout << "Terrain bounds:" << std::endl;
    std::cout << "X: " << minBounds.x << " to " << maxBounds.x << std::endl;
    std::cout << "Y: " << minBounds.y << " to " << maxBounds.y << std::endl;
    std::cout << "Z: " << minBounds.z << " to " << maxBounds.z << std::endl;
}

void Terrain::cleanup() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (heightmapTexture) glDeleteTextures(1, &heightmapTexture);
    if (terrainTexture) glDeleteTextures(1, &terrainTexture);
}
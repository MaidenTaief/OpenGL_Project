#ifndef TERRAIN_H
#define TERRAIN_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <GL/glew.h>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Terrain {
public:
    Terrain();
    ~Terrain();

    bool initialize(const std::string& heightMapPath, const std::string& texturePath, const std::vector<glm::vec3>& gpxPoints);
    void draw();
    void debugOutput();

private:
    GLuint VBO, VAO, EBO, textureID;
    unsigned int numIndices;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    bool loadHeightMap(const std::string& path, int& width, int& height, std::vector<unsigned char>& heightData);
    bool loadTexture(const std::string& path);
    void generateTerrainVertices(const std::vector<unsigned char>& heightData, int width, int height, const std::vector<glm::vec3>& gpxPoints);
    void generateTerrainIndices(int width, int height);
    void calculateNormals();
    void setupBuffers();
};

#endif // TERRAIN_H
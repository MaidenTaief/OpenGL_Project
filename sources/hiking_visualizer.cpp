// hiking_visualizer.cpp
#include "hiking_visualizer.h"
#include <iostream>
#include <limits>
#include <glm/gtc/type_ptr.hpp>

HikingVisualizer::HikingVisualizer() : trailShader(std::make_unique<Shader>()) {
}

HikingVisualizer::~HikingVisualizer() {
    cleanup();
}

void HikingVisualizer::cleanup() {
    if (trailVAO) glDeleteVertexArrays(1, &trailVAO);
    if (trailVBO) glDeleteBuffers(1, &trailVBO);
    if (hikerVAO) glDeleteVertexArrays(1, &hikerVAO);
    if (hikerVBO) glDeleteBuffers(1, &hikerVBO);
    trailVAO = trailVBO = hikerVAO = hikerVBO = 0;
}

bool HikingVisualizer::initialize(const std::vector<glm::vec3>& hikingPoints) {
    if (hikingPoints.empty()) {
        std::cerr << "No hiking points provided" << std::endl;
        return false;
    }

    trailPoints = hikingPoints;
    currentPosition = trailPoints[0];

    // Load shaders using the Shader class
    if (!trailShader->load(
        "A:/Taief/Project/OpenGL_Project/shaders/trail_vertex.glsl",
        "A:/Taief/Project/OpenGL_Project/shaders/trail_fragment.glsl")) {
        std::cerr << "Failed to load trail shaders" << std::endl;
        return false;
    }

    if (!setupTrailBuffer()) {
        return false;
    }

    calculateTotalDistance();
    updateTrailStatistics();
    return true;
}

bool HikingVisualizer::setupTrailBuffer() {
    // Create and setup trail buffer
    glGenVertexArrays(1, &trailVAO);
    glGenBuffers(1, &trailVBO);

    glBindVertexArray(trailVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
    glBufferData(GL_ARRAY_BUFFER, trailPoints.size() * sizeof(glm::vec3),
        trailPoints.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Create and setup hiker position buffer
    glGenVertexArrays(1, &hikerVAO);
    glGenBuffers(1, &hikerVBO);

    glBindVertexArray(hikerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hikerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &currentPosition, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    return true;
}

void HikingVisualizer::update(float deltaTime) {
    if (currentSegment >= trailPoints.size() - 1) return;

    float distanceToMove = hikerSpeed * deltaTime;
    currentDistance += distanceToMove;
    totalTime += deltaTime;

    while (currentSegment < trailPoints.size() - 1) {
        glm::vec3 segmentStart = trailPoints[currentSegment];
        glm::vec3 segmentEnd = trailPoints[currentSegment + 1];
        float segmentLength = glm::distance(segmentStart, segmentEnd);

        glm::vec3 direction = glm::normalize(segmentEnd - segmentStart);
        float distanceToSegmentEnd = glm::distance(currentPosition, segmentEnd);

        if (distanceToMove <= distanceToSegmentEnd) {
            currentPosition += direction * distanceToMove;
            updateHikerPosition();
            break;
        }
        else {
            distanceToMove -= distanceToSegmentEnd;
            currentPosition = segmentEnd;
            currentSegment++;
            updateHikerPosition();
            if (currentSegment >= trailPoints.size() - 1) {
                // Hiker has reached the end
                currentPosition = trailPoints.back();
                updateHikerPosition();
                break;
            }
        }
    }
}

void HikingVisualizer::draw(const glm::mat4& view, const glm::mat4& projection) {
    trailShader->use();
    trailShader->setMat4("view", view);
    trailShader->setMat4("projection", projection);

    // Draw trail
    if (!trailPoints.empty()) {
        glBindVertexArray(trailVAO);
        glLineWidth(trailWidth);
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(trailPoints.size()));
    }

    // Draw hiker position
    glBindVertexArray(hikerVAO);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, 1);
}

void HikingVisualizer::updateHikerPosition() {
    glBindBuffer(GL_ARRAY_BUFFER, hikerVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3), &currentPosition);
}

void HikingVisualizer::calculateTotalDistance() {
    totalDistance = 0.0f;
    for (size_t i = 0; i < trailPoints.size() - 1; ++i) {
        totalDistance += glm::distance(trailPoints[i], trailPoints[i + 1]);
    }
}

void HikingVisualizer::updateTrailStatistics() {
    maxHeight = -std::numeric_limits<float>::infinity();
    minHeight = std::numeric_limits<float>::infinity();

    for (const auto& point : trailPoints) {
        maxHeight = std::max(maxHeight, point.y);
        minHeight = std::min(minHeight, point.y);
    }
}

HikingVisualizer::HikeStats HikingVisualizer::getHikeStats() const {
    return HikeStats{
        currentPosition.y,
        totalDistance,
        hikerSpeed,
        totalTime > 0.0f ? currentDistance / totalTime : 0.0f,
        maxHeight,
        minHeight,
        trailPoints.size(),
        getCompletionPercentage()
    };
}

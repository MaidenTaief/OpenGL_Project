// hiking_visualizer.h
#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.h"

class HikingVisualizer {
public:
    HikingVisualizer();
    ~HikingVisualizer();

    bool initialize(const std::vector<glm::vec3>& hikingPoints);
    void update(float deltaTime);
    void draw(const glm::mat4& view, const glm::mat4& projection);
    void cleanup();

    struct HikeStats {
        float currentElevation;
        float totalDistance;
        float currentSpeed;
        float averageSpeed;
        float maxHeight;
        float minHeight;
        size_t totalPoints;
        float completionPercentage;
    };

    HikeStats getHikeStats() const;
    glm::vec3 getCurrentHikerPosition() const { return currentPosition; }
    void setHikerSpeed(float speed) { hikerSpeed = speed; }
    float getHikerSpeed() const { return hikerSpeed; }

private:
    bool setupTrailBuffer();
    void updateHikerPosition();
    void calculateTotalDistance();
    void updateTrailStatistics();
    float calculateSegmentLength(const glm::vec3& start, const glm::vec3& end);
    float getCompletionPercentage() const {
        return (currentDistance / totalDistance) * 100.0f;
    }

    std::unique_ptr<Shader> trailShader;

    std::vector<glm::vec3> trailPoints;
    GLuint trailVAO = 0;
    GLuint trailVBO = 0;
    GLuint hikerVAO = 0;
    GLuint hikerVBO = 0;

    glm::vec3 currentPosition;
    size_t currentSegment = 0;
    float currentDistance = 0.0f;
    float totalDistance = 0.0f;
    float totalTime = 0.0f;
    float hikerSpeed = 5.0f; // meters per second
    float trailWidth = 2.0f;

    float maxHeight = 0.0f;
    float minHeight = 0.0f;
};

// hiking_data.h
#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct HikingPoint {
    glm::vec3 position{ 0.0f };
    float timestamp{ 0.0f };
};

bool loadHikingData(const std::string& filename, std::vector<glm::vec3>& hikingPoints);
void smoothPath(std::vector<glm::vec3>& points);
glm::vec3 gpsToLocalCoordinates(double lat, double lon, double ele);

// Constants for coordinate transformation
//constexpr double SCALE_FACTOR = 100.0;
//constexpr double LAT_ORIGIN = 60.0;
//constexpr double LON_ORIGIN = 10.0;
#ifndef HIKING_DATA_H
#define HIKING_DATA_H

#include <vector>
#include <glm/glm.hpp>
#include <string>

bool loadHikingData(const std::string& gpxFilePath, std::vector<glm::vec3>& points);

#endif // HIKING_DATA_H

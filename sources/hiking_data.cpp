#include "hiking_data.h"
#include "tinyxml2.h"
#include <iostream>
#include <glm/glm.hpp>
#include <string>

bool loadHikingData(const std::string& gpxFilePath, std::vector<glm::vec3>& points) {
    tinyxml2::XMLDocument doc;

    std::cerr << "Attempting to load GPX file: " << gpxFilePath << std::endl;

    tinyxml2::XMLError loadResult = doc.LoadFile(gpxFilePath.c_str());
    if (loadResult != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading GPX file: " << gpxFilePath << ", Error: " << doc.ErrorStr() << std::endl;
        return false;
    }

    tinyxml2::XMLElement* gpxElement = doc.FirstChildElement("gpx");
    if (!gpxElement) {
        std::cerr << "Invalid GPX file: No root element <gpx> found." << std::endl;
        return false;
    }

    tinyxml2::XMLElement* trkElement = gpxElement->FirstChildElement("trk");
    if (!trkElement) {
        std::cerr << "Invalid GPX file: No <trk> element found." << std::endl;
        return false;
    }

    tinyxml2::XMLElement* trksegElement = trkElement->FirstChildElement("trkseg");
    if (!trksegElement) {
        std::cerr << "Invalid GPX file: No <trkseg> element found." << std::endl;
        return false;
    }

    tinyxml2::XMLElement* trkptElement = trksegElement->FirstChildElement("trkpt");
    while (trkptElement) {
        // Extract latitude and longitude attributes
        double lat = trkptElement->DoubleAttribute("lat");
        double lon = trkptElement->DoubleAttribute("lon");

        // Extract elevation
        tinyxml2::XMLElement* eleElement = trkptElement->FirstChildElement("ele");
        if (!eleElement) {
            std::cerr << "Warning: No <ele> element found for trkpt, skipping." << std::endl;
            trkptElement = trkptElement->NextSiblingElement("trkpt");
            continue;
        }
        double ele = eleElement->DoubleText();

        // Add the point (latitude, elevation, longitude)
        points.emplace_back(static_cast<float>(lat), static_cast<float>(ele), static_cast<float>(lon));

        // Move to the next track point
        trkptElement = trkptElement->NextSiblingElement("trkpt");
    }

    std::cerr << "Successfully loaded GPX data with " << points.size() << " track points." << std::endl;

    return true;
}

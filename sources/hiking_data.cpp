// hiking_data.cpp
#include "hiking_data.h"
#include <iostream>
#include <limits>
#include "tinyxml2.h"

// Globals to store min values
double minLon = std::numeric_limits<double>::max();
double minLat = std::numeric_limits<double>::max();

glm::vec3 gpsToLocalCoordinates(double lat, double lon, double ele) {
    // Adjust these scale factors to match your terrain scale
    const float TERRAIN_SCALE = 100000.0f; // Adjust as needed
    const float HEIGHT_SCALE = 1.0f;       // Adjust for elevation scaling

    // Use minLon and minLat as origins
    float x = static_cast<float>((lon - minLon) * TERRAIN_SCALE);
    float z = static_cast<float>((lat - minLat) * TERRAIN_SCALE);
    float y = static_cast<float>(ele * HEIGHT_SCALE);

    return glm::vec3(x, y, z);
}


bool loadHikingData(const std::string& filename, std::vector<glm::vec3>& hikingPoints) {
    try {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
            std::cerr << "Error loading GPX file: " << filename << std::endl;
            return false;
        }

        // Get the root element (gpx)
        tinyxml2::XMLElement* gpx = doc.FirstChildElement("gpx");
        if (!gpx) {
            std::cerr << "No GPX element found" << std::endl;
            return false;
        }

        // Get the track (trk)
        tinyxml2::XMLElement* trk = gpx->FirstChildElement("trk");
        if (!trk) {
            std::cerr << "No track element found" << std::endl;
            return false;
        }

        // Get the track segment (trkseg)
        tinyxml2::XMLElement* trkseg = trk->FirstChildElement("trkseg");
        if (!trkseg) {
            std::cerr << "No track segment found" << std::endl;
            return false;
        }

        // First pass to find minLon and minLat
        for (tinyxml2::XMLElement* trkpt = trkseg->FirstChildElement("trkpt");
            trkpt != nullptr;
            trkpt = trkpt->NextSiblingElement("trkpt")) {

            double lat = trkpt->DoubleAttribute("lat");
            double lon = trkpt->DoubleAttribute("lon");

            if (lon < minLon) minLon = lon;
            if (lat < minLat) minLat = lat;
        }

        // Clear existing points
        hikingPoints.clear();

        // Second pass to process points
        for (tinyxml2::XMLElement* trkpt = trkseg->FirstChildElement("trkpt");
            trkpt != nullptr;
            trkpt = trkpt->NextSiblingElement("trkpt")) {

            double lat = trkpt->DoubleAttribute("lat");
            double lon = trkpt->DoubleAttribute("lon");

            // Get elevation
            tinyxml2::XMLElement* ele = trkpt->FirstChildElement("ele");
            double elevation = 0.0;
            if (ele) {
                elevation = std::stod(ele->GetText());
            }

            // Convert GPS coordinates to local coordinate system
            glm::vec3 point = gpsToLocalCoordinates(lat, lon, elevation);
            hikingPoints.push_back(point);
        }

        // Validate that we loaded some points
        if (hikingPoints.empty()) {
            std::cerr << "No track points found in GPX file" << std::endl;
            return false;
        }

        // Optional: Print out some points for debugging
        for (size_t i = 0; i < std::min<size_t>(5, hikingPoints.size()); ++i) {
            std::cout << "Point " << i << ": (" << hikingPoints[i].x << ", "
                << hikingPoints[i].y << ", " << hikingPoints[i].z << ")" << std::endl;
        }

        std::cout << "Successfully loaded " << hikingPoints.size() << " hiking points" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error processing GPX file: " << e.what() << std::endl;
        return false;
    }
}

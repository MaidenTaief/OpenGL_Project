#pragma once
#include <glm/glm.hpp>

namespace math_utils {
    // Generic clamp function
    template<typename T>
    inline T clamp(const T& value, const T& min, const T& max) {
        return value < min ? min : (value > max ? max : value);
    }

    // Vector operations
    inline glm::vec3 lerp(const glm::vec3& a, const glm::vec3& b, float t) {
        return a + t * (b - a);
    }

    inline float calculateElevationGain(const glm::vec3& a, const glm::vec3& b) {
        return b.y - a.y;
    }
}
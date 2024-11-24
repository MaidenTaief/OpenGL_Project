#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

namespace math_utils {
    template<typename T>
    T clamp(T value, T min, T max) {
        return value < min ? min : (value > max ? max : value);
    }
}

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    Camera(glm::vec3 startPosition = glm::vec3(500.0f, 300.0f, 500.0f)) {
        position = startPosition;
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        yaw = -90.0f;
        pitch = 0.0f;
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        movementSpeed = 100.0f;
        mouseSensitivity = 0.1f;
        minSpeed = 10.0f;
        maxSpeed = 500.0f;
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    void processKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        switch (direction) {
        case FORWARD:
            position += front * velocity;
            break;
        case BACKWARD:
            position -= front * velocity;
            break;
        case LEFT:
            position -= right * velocity;
            break;
        case RIGHT:
            position += right * velocity;
            break;
        case UP:
            position.y += velocity;
            break;
        case DOWN:
            position.y = std::max(1.0f, position.y - velocity);
            break;
        }
    }

    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (constrainPitch) {
            pitch = math_utils::clamp(pitch, -89.0f, 89.0f);
        }

        updateCameraVectors();
    }

    void adjustSpeed(float factor) {
        movementSpeed = math_utils::clamp(movementSpeed * factor, minSpeed, maxSpeed);
        std::cout << "Camera speed: " << movementSpeed << std::endl;
    }

    void moveVertical(float deltaTime, bool upward) {
        float velocity = movementSpeed * deltaTime;
        if (upward) {
            position.y += velocity;
        }
        else {
            position.y = std::max(1.0f, position.y - velocity);
        }
    }

    void resetPosition() {
        position = glm::vec3(0.0f, 100.0f, 0.0f);
        yaw = -90.0f;
        pitch = 0.0f;
        updateCameraVectors();
    }

    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getPosition() const { return position; }
    void setSpeed(float speed) { movementSpeed = math_utils::clamp(speed, minSpeed, maxSpeed); }
    float getSpeed() const { return movementSpeed; }

    // Getters for camera vectors
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getRight() const { return right; }

private:
    // Camera attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Euler angles
    float yaw;
    float pitch;

    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float minSpeed;
    float maxSpeed;

    void updateCameraVectors() {
        // Calculate the new front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        front = glm::normalize(newFront);
        // Recalculate the right and up vectors
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};
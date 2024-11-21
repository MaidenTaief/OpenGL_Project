#include "window.h"
#include <iostream>

Window::Window(int width, int height, const char* title)
    : width(width), height(height), title(title), window(nullptr) {
}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool Window::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Create a GLFW window
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);

    // Set the viewport
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    return true;
}

GLFWwindow* Window::getGLFWwindow() {
    return window;
}

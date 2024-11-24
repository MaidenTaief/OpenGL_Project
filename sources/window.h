#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool initialize();
    GLFWwindow* getGLFWwindow();
    const std::string& getLastError() const { return lastErrorMessage; }

private:
    int width;
    int height;
    const char* title;
    GLFWwindow* window;
    std::string lastErrorMessage;
};

#endif // WINDOW_H

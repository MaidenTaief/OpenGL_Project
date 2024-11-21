#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool initialize();
    GLFWwindow* getGLFWwindow();

private:
    int width;
    int height;
    const char* title;
    GLFWwindow* window;
};

#endif // WINDOW_H

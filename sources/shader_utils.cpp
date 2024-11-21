#include "shader_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>

GLuint loadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    // Load and compile vertex shader
    std::ifstream vertexFile(vertexShaderPath);
    if (!vertexFile) {
        std::cerr << "Failed to open vertex shader file: " << vertexShaderPath << std::endl;
        return 0;
    }
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    vertexFile.close();
    std::string vertexCode = vertexStream.str();
    const char* vShaderCode = vertexCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Error compiling vertex shader: " << infoLog << std::endl;
        return 0;
    }

    // Load and compile fragment shader
    std::ifstream fragmentFile(fragmentShaderPath);
    if (!fragmentFile) {
        std::cerr << "Failed to open fragment shader file: " << fragmentShaderPath << std::endl;
        return 0;
    }
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    fragmentFile.close();
    std::string fragmentCode = fragmentStream.str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Error compiling fragment shader: " << infoLog << std::endl;
        return 0;
    }

    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Error linking shader program: " << infoLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

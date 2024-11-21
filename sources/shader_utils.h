#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <string>
#include <GL/glew.h>

// Declaration of loadShaders function
GLuint loadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

#endif // SHADER_UTILS_H

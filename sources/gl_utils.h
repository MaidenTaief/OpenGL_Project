// Add this to a new header file called gl_utils.h

#pragma once
#include <iostream>

#define GL_CHECK(x) \
    x; \
    { \
        GLenum err; \
        while ((err = glGetError()) != GL_NO_ERROR) { \
            std::cerr << "OpenGL error in " << #x << ": " << err << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        } \
    }

// Example usage:
// GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, VBO));
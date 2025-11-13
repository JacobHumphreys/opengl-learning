#include "errors.hpp"
#include "iostream"

void clearGlErrors() {
    while (glGetError() != GL_NO_ERROR) { };
}

bool logGlCall(std::string function, std::string file, int line) {
    while (GLenum error = glGetError()) {
        println(std::cerr, "[OpenGL Error]:\n\t[CODE] 0x{:x}\n\t[FUNCTION CALL] {}\n\t[ORIGIN] {} (line: {})", error, function, file, line);
        return false;
    }
    return true;
}

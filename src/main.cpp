#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <print>
#include <string>

#include "shaders.hpp"
#include "errors.hpp"

using std::println;
using std::string;

int main(void) {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    auto window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window) {
        CALL_GL(glfwTerminate());
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        println(std::cerr, "Failed to start GLEW");
        glfwTerminate();
        return -1;
    } else {
        println("version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    const float_t positions[] = {
        -0.5f, -0.5f, // 0
        0.5f, -0.5f, // 1
        0.5f, 0.5f, // 2
        -0.5f, 0.5f, // 3
    };

    const uint32_t indicies[] = {
        0, 1, 2, // triangle 1
        2, 3, 0, // triangle 2
    };

    {
        uint32_t vertex_buffer;
        CALL_GL(glGenBuffers(1, &vertex_buffer));
        CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));

        CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));

        CALL_GL(glEnableVertexAttribArray(0));
        CALL_GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));
    }

    {
        uint32_t index_buffer_object;
        CALL_GL(glGenBuffers(1, &index_buffer_object));
        CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object));
        CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW));
    }

    auto basic_shader_source = shaders::parseShader("./src/resources/shaders/basic.glsl");

    auto shader = shaders::createShader(basic_shader_source);
    CALL_GL(glUseProgram(shader));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        { // fix wayland
            int fbWidth, fbHeight;
            CALL_GL(glfwGetFramebufferSize(window, &fbWidth, &fbHeight));
            // Set viewport to framebuffer size
            CALL_GL(glViewport(0, 0, fbWidth, fbHeight));
        }

        /* Render here */
        CALL_GL(glClear(GL_COLOR_BUFFER_BIT));

        CALL_GL(
            glDrawElements(GL_TRIANGLES, sizeof(indicies) / sizeof(float), GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        CALL_GL(glfwSwapBuffers(window));

        /* Poll for and process events */
        CALL_GL(glfwPollEvents());
    }

    CALL_GL(glDeleteProgram(shader));

    CALL_GL(glfwTerminate());
    return 0;
}

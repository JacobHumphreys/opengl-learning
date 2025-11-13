#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <print>
#include <string>

#include "debug.hpp"
#include "shaders.hpp"
#include "errors.hpp"

using std::println;
using std::string;

int main(void) {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    auto window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    // Sets vsync
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        println(std::cerr, "Failed to start GLEW");
        glfwTerminate();
        return -1;
    } else {
        println("version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    constexpr float_t positions[] = {
        -0.5f, -0.5f, // 0
        0.5f, -0.5f, // 1
        0.5f, 0.5f, // 2
        -0.5f, 0.5f, // 3
    };

    constexpr uint32_t indices[] = {
        0, 1, 2, // triangle 1
        2, 3, 0, // triangle 2
    };

    uint32_t vertex_array_object;
    CALL_GL(glGenVertexArrays(1, &vertex_array_object))
    CALL_GL(glBindVertexArray(vertex_array_object))

    uint32_t vertex_buffer;
    CALL_GL(glGenBuffers(1, &vertex_buffer));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
    CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions) , positions, GL_STATIC_DRAW));

    CALL_GL(glEnableVertexAttribArray(0));

    // Sets 0th index of vertex_array_object to bind to the currently bound buffer (vertex_buffer)
    CALL_GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr));

    uint32_t index_buffer;
    CALL_GL(glGenBuffers(1, &index_buffer));
    CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer));
    CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    auto basic_shader_source = shaders::parseShader("./src/resources/shaders/basic.glsl");

    auto shader = shaders::createShader(basic_shader_source);
    CALL_GL(glUseProgram(shader));

    // Set color uniform
    int32_t color_uniform = glGetUniformLocation(shader, "u_color");
    ASSERT_BP(color_uniform != -1);

    CALL_GL(glUniform4f(color_uniform, 0.8f, 0.3f, 0.4f, 1.0f));

    CALL_GL(glUseProgram(0));
    CALL_GL(glBindVertexArray(0));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    float r = 0;
    float increment = 0.05f;
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        { // fix wayland
            int fbWidth, fbHeight;
            CALL_GL(glfwGetFramebufferSize(window, &fbWidth, &fbHeight));
            // Set viewport to framebuffer size
            CALL_GL(glViewport(0, 0, fbWidth, fbHeight));
        }

        /* Render here */
        CALL_GL(glClear(GL_COLOR_BUFFER_BIT));

        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0)
            increment = 0.05f;

        r += increment;

        /* Draw Calls */
        CALL_GL(glUseProgram(shader));
        CALL_GL(glUniform4f(color_uniform, r, 0.3f, 0.4f, 1.0f));
        CALL_GL(glBindVertexArray(vertex_array_object));
        CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer));

        CALL_GL(
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(float), GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        CALL_GL(glfwSwapBuffers(window));

        /* Poll for and process events */
        CALL_GL(glfwPollEvents());
    }

    CALL_GL(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}

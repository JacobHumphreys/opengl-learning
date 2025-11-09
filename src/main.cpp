#include <GL/glew.h>
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

using std::println;
using std::string;

int main(void) {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    auto window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
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
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    }

    {
        uint32_t index_buffer_object;
        glGenBuffers(1, &index_buffer_object);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
    }

    auto basic_shader_source = shaders::parseShader("./src/resources/shaders/basic.glsl");

    auto shader = shaders::createShader(basic_shader_source);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        { // fix wayland
            int fbWidth, fbHeight;
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
            // Set viewport to framebuffer size
            glViewport(0, 0, fbWidth, fbHeight);
        }

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //        glDrawArrays(GL_TRIANGLES, 0, sizeof(positions) / 2);
        glDrawElements(GL_TRIANGLES, sizeof(indicies) / sizeof(float), GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}

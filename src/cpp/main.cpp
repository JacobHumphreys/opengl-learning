#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <print>
#include <stdint.h>
#include <string>

#include "errors.hpp"
#include "renderer/IndexBuffer.hpp"
#include "renderer/Shader.hpp"
#include "renderer/VertexArray.hpp"
#include "renderer/VertexBuffer.hpp"
#include "renderer/VertexBufferLayout.hpp"

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

    constexpr auto positions = std::array<float_t, 8> {
        -0.5f, -0.5f, // 0
        0.5f, -0.5f, // 1
        0.5f, 0.5f, // 2
        -0.5f, 0.5f, // 3
    };

    constexpr auto indices = std::array<uint32_t, 6> {
        0, 1, 2, // triangle 1
        2, 3, 0, // triangle 2
    };

    VertexArray vertex_array;
    VertexBuffer vertex_buffer(positions.data(), positions.size() * sizeof(float_t));
    vertex_buffer.bind();

    VertexBufferLayout layout;
    layout.push<float>(2);
    vertex_array.addBuffer(vertex_buffer, layout);

    IndexBuffer index_buffer(indices.data(), indices.size());
    index_buffer.bind();

    // auto basic_shader_source = shaders::parseShader("./src/resources/shaders/basic.glsl");

    Shader shader("./src/resources/shaders/basic.glsl");
    shader.bind();
    shader.unbind();
    vertex_array.unbind();
    vertex_buffer.unbind();
    index_buffer.unbind();

    float r = 0;
    float increment = 0.05f;
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        { // fix wayland
            int fbWidth, fbHeight;
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
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
        shader.bind();
        shader.setUniform4f("u_color", float4(r, 0.3f, 0.4f, 1.0f));
        vertex_array.bind();
        index_buffer.bind();

        CALL_GL(
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(float), GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // CALL_GL(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}

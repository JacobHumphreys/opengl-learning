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
#include <ranges>
#include <string>
#include <vector>

using std::println;

using std::string;
using std::vector;

static uint32_t compileShader(uint32_t type, const vector<string>& sources) {
    uint32_t id = glCreateShader(type);

    auto raw_sources = sources | std::ranges::views::transform([](const string& s) -> const char* { return s.c_str(); })
        | std::ranges::to<vector<const char*>>();

    glShaderSource(id, raw_sources.size(), raw_sources.data(), nullptr);
    glCompileShader(id);

    int32_t result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int32_t length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        auto message = static_cast<char*>(alloca(sizeof(char) * static_cast<size_t>(length)));
        glGetShaderInfoLog(id, length, &length, message);
        println(std::cerr);
        println(std::cerr, "Shader Compilation Error on shader type: {}", type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
        println(std::cerr, "Error: {}", message);
        println(std::cerr);
    }

    return id;
}

static uint32_t compileShader(uint32_t type, const string& source) {
    const vector<string> sources = { source };
    return compileShader(type, sources);
}

static uint32_t createShader(const string& vertexShader, const string& fragmentShader) {
    uint32_t program = glCreateProgram();

    uint32_t vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

    const float_t positions[6] = {
        -0.5f, -0.5f,
        0.f, 0.5f,
        0.5f, -0.5f
    };

    uint32_t buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    string vertexShader = "#version 330 core\n"
                          "\n"
                          "layout(location = 0) in vec4 position;\n"
                          "void main()\n"
                          "{\n"
                          "    gl_Position = position;\n"
                          "}\n";

    string fragmentShader = "#version 330 core\n"
                            "\n"
                            "in vec2 uv;\n"
                            "layout(location = 0) out vec4 color;\n"
                            "\n"
                            "void main()\n"
                            "{\n"
                            "    color = vec4(uv.x, uv.y, 0.0, 1.0);\n"
                            "}\n";

    auto shader = createShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    glDeleteShader(shader);

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

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

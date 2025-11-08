#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <print>

using std::println;

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

        glBegin(GL_TRIANGLES);

        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.f, 0.5f);
        glVertex2f(0.5f, -0.5f);

        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

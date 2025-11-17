const std = @import("std");
const builtin = @import("builtin");

const gl = @import("gl.zig").gl;
const glfw = @import("gl.zig").glfw;
const assertBp = @import("errors.zig").assert;
const callGl = @import("errors.zig").callGl;
const shaders = @import("shaders.zig");
const renderer = @import("renderer.zig");

const Error = error{
    InitError,
};

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}).init;
    const alloc = gpa.allocator();
    defer assertBp(gpa.deinit() == .ok) catch @breakpoint();

    if (glfw.glfwInit() == glfw.GLFW_FALSE)
        return Error.InitError;

    defer glfw.glfwTerminate();

    glfw.glfwWindowHint(glfw.GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfw.glfwWindowHint(glfw.GLFW_CONTEXT_VERSION_MINOR, 6);
    glfw.glfwWindowHint(glfw.GLFW_OPENGL_PROFILE, glfw.GLFW_OPENGL_CORE_PROFILE);

    const window = glfw.glfwCreateWindow(640, 480, "glfw window", null, null) orelse {
        return Error.InitError;
    };

    glfw.glfwMakeContextCurrent(window);

    glfw.glfwSwapInterval(1);

    if (gl.glewInit() != gl.GLEW_OK) {
        std.log.err("Failed to start GLFW", .{});
        return Error.InitError;
    } else {
        std.log.info("[VERSION] {s}", .{gl.glGetString(gl.GL_VERSION)});
    }

    const positions = [_]f32{
        -0.5, -0.5, // 0
        0.5, -0.5, // 1
        0.5, 0.5, // 2
        -0.5, 0.5, // 3
    };

    const indices = [_]u32{
        0, 1, 2, // triangle 1
        2, 3, 0, // triangle 2
    };

    var vertex_array_object: u32 = undefined;
    try callGl(gl.glGenVertexArrays().?, .{ 1, &vertex_array_object });
    try callGl(gl.glBindVertexArray().?, .{vertex_array_object});

    const vertex_buffer = try renderer.VertexArray.init(&positions, positions.len * @sizeOf(f32));
    defer vertex_buffer.deinit();
    vertex_buffer.bind();

    try callGl(gl.glEnableVertexAttribArray().?, .{0});

    // Sets 0th index of vertex_array_object to bind to the currently bound buffer ().?(vertex_buffer)
    try callGl(
        gl.glVertexAttribPointer().?,
        .{ 0, 2, gl.GL_FLOAT, gl.GL_FALSE, @sizeOf(f32) * 2, null },
    );

    const index_buffer = try renderer.IndexArray.init(&indices, indices.len);
    defer index_buffer.deinit();
    index_buffer.bind();

    const basic_shader_source = try shaders.parseShader(
        alloc,
        "./src/resources/shaders/basic.glsl",
    );
    defer basic_shader_source.deinit(alloc);

    const shader = try shaders.createShader(alloc, basic_shader_source);
    defer callGl(gl.glDeleteProgram().?, .{shader}) catch @breakpoint();

    try callGl(gl.glUseProgram().?, .{shader});

    // Set color uniform
    const color_uniform: i32 = gl.glGetUniformLocation().?(shader, "u_color");
    assertBp(color_uniform != -1) catch @breakpoint();

    try callGl(gl.glUseProgram().?, .{0});
    try callGl(gl.glBindVertexArray().?, .{0});
    vertex_buffer.unbind();
    index_buffer.unbind();

    var r: f32 = 0;
    var increment: f32 = 0.05;
    while (glfw.glfwWindowShouldClose(window) == glfw.GLFW_FALSE) {
        {
            var fb_width: i32 = 0;
            var fb_height: i32 = 0;
            glfw.glfwGetFramebufferSize(window, &fb_width, &fb_height);
            try callGl(gl.glViewport, .{ 0, 0, fb_width, fb_height });
        }
        try callGl(gl.glClear, .{gl.GL_COLOR_BUFFER_BIT});

        increment = if (r > 1.0)
            -0.05
        else if (r < 0)
            0.05
        else
            increment;

        r += increment;

        try callGl(gl.glUseProgram().?, .{shader});
        try callGl(gl.glUniform4f().?, .{ color_uniform, r, 0.3, 0.4, 1.0 });

        try callGl(gl.glBindVertexArray().?, .{vertex_array_object});
        index_buffer.bind();

        try callGl(
            gl.glDrawElements,
            .{ gl.GL_TRIANGLES, indices.len, gl.GL_UNSIGNED_INT, null },
        );

        try callGl(glfw.glfwSwapBuffers, .{window});

        try callGl(glfw.glfwPollEvents, .{});
    }
}

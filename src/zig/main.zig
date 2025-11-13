const std = @import("std");
const builtin = @import("builtin");

const gl = @import("gl.zig").gl;
const assertBp = @import("errors.zig").assertBp;
const callGl = @import("errors.zig").callGl;
const shaders = @import("shaders.zig");

const Error = error{
    InitError,
};

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}).init;
    const alloc = gpa.allocator();
    defer assertBp(gpa.deinit() == .ok);

    if (gl.glfwInit() == gl.GLFW_FALSE)
        return Error.InitError;

    defer callGl(gl.glfwTerminate, .{});

    gl.glfwWindowHint(gl.GLFW_CONTEXT_VERSION_MAJOR, 4);
    gl.glfwWindowHint(gl.GLFW_CONTEXT_VERSION_MINOR, 6);
    gl.glfwWindowHint(gl.GLFW_OPENGL_PROFILE, gl.GLFW_OPENGL_CORE_PROFILE);

    const window = gl.glfwCreateWindow(640, 480, "glfw window", null, null) orelse {
        return Error.InitError;
    };

    gl.glfwMakeContextCurrent(window);

    gl.glfwSwapInterval(1);

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
    callGl(gl.glGenVertexArrays().?, .{ 1, &vertex_array_object });
    callGl(gl.glBindVertexArray().?, .{vertex_array_object});

    var vertex_buffer: u32 = undefined;
    callGl(gl.glGenBuffers().?, .{ 1, &vertex_buffer });
    callGl(gl.glBindBuffer().?, .{ gl.GL_ARRAY_BUFFER, vertex_buffer });
    callGl(gl.glBufferData().?, .{
        gl.GL_ARRAY_BUFFER,
        @sizeOf(@TypeOf(positions)),
        &positions,
        gl.GL_STATIC_DRAW,
    });

    callGl(gl.glEnableVertexAttribArray().?, .{0});

    // Sets 0th index of vertex_array_object to bind to the currently bound buffer ().?(vertex_buffer)
    callGl(
        gl.glVertexAttribPointer().?,
        .{ 0, 2, gl.GL_FLOAT, gl.GL_FALSE, @sizeOf(f32) * 2, null },
    );

    var index_buffer: u32 = undefined;
    callGl(gl.glGenBuffers().?, .{ 1, &index_buffer });
    callGl(gl.glBindBuffer().?, .{ gl.GL_ELEMENT_ARRAY_BUFFER, index_buffer });
    callGl(
        gl.glBufferData().?,
        .{
            gl.GL_ELEMENT_ARRAY_BUFFER,
            @sizeOf(@TypeOf(indices)),
            &indices,
            gl.GL_STATIC_DRAW,
        },
    );

    const basic_shader_source = try shaders.parseShader(
        alloc,
        "./src/resources/shaders/basic.glsl",
    );
    defer basic_shader_source.deinit(alloc);

    const shader = try shaders.createShader(alloc, basic_shader_source);
    callGl(gl.glUseProgram().?, .{shader});
    defer callGl(gl.glDeleteProgram().?, .{shader});

    // Set color uniform
    const color_uniform: i32 = gl.glGetUniformLocation().?(shader, "u_color");
    assertBp(color_uniform != -1);

    callGl(gl.glUniform4f().?, .{ color_uniform, 0.8, 0.3, 0.4, 1.0 });

    callGl(gl.glUseProgram().?, .{0});
    callGl(gl.glBindVertexArray().?, .{0});
    callGl(gl.glBindBuffer().?, .{ gl.GL_ARRAY_BUFFER, 0 });
    callGl(gl.glBindBuffer().?, .{ gl.GL_ELEMENT_ARRAY_BUFFER, 0 });

    var r: f32 = 0;
    var increment: f32 = 0.05;
    while (gl.glfwWindowShouldClose(window) == gl.GLFW_FALSE) {
        {
            var fb_width: i32 = 0;
            var fb_height: i32 = 0;
            callGl(gl.glfwGetFramebufferSize, .{ window, &fb_width, &fb_height });
            callGl(gl.glViewport, .{ 0, 0, fb_width, fb_height });
        }
        callGl(gl.glClear, .{gl.GL_COLOR_BUFFER_BIT});

        increment = if (r > 1.0)
            -0.05
        else if (r < 0)
            0.05
        else
            increment;

        r += increment;

        callGl(gl.glUseProgram().?, .{shader});
        callGl(gl.glUniform4f().?, .{ color_uniform, r, 0.3, 0.4, 1.0 });

        callGl(gl.glBindVertexArray().?, .{vertex_array_object});
        callGl(gl.glBindBuffer().?, .{ gl.GL_ELEMENT_ARRAY_BUFFER, index_buffer });

        callGl(
            gl.glDrawElements,
            .{ gl.GL_TRIANGLES, indices.len, gl.GL_UNSIGNED_INT, null },
        );

        callGl(gl.glfwSwapBuffers, .{window});

        callGl(gl.glfwPollEvents, .{});
    }
}

pub const gl = @cImport({
    @cInclude("GL/glew.h");
});

pub const glfw = @cImport({
    @cInclude("GLFW/glfw3.h");
});

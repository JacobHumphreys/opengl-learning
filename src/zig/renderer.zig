const gl = @import("gl.zig").gl;
const callGl = @import("errors.zig").callGl;

pub const VertexArray = struct {
    renderer_id: u32,

    pub fn init(data: *const anyopaque, size: u32) !VertexArray {
        var array_buffer: u32 = undefined;
        try callGl(gl.glGenBuffers().?, .{ 1, &array_buffer });
        try callGl(gl.glBindBuffer().?, .{ gl.GL_ARRAY_BUFFER, array_buffer });
        try callGl(
            gl.glBufferData().?,
            .{
                gl.GL_ARRAY_BUFFER,
                size,
                data,
                gl.GL_STATIC_DRAW,
            },
        );

        try callGl(gl.glBindBuffer().?, .{ gl.GL_ARRAY_BUFFER, 0 });
        return .{ .renderer_id = array_buffer };
    }

    pub fn deinit(self: *const VertexArray) void {
        callGl(gl.glDeleteBuffers().?, .{ 1, &self.renderer_id }) catch @breakpoint();
    }

    pub fn bind(self: *const VertexArray) void {
        callGl(gl.glBindBuffer().?, .{ gl.GL_ARRAY_BUFFER, self.renderer_id }) catch @breakpoint();
    }

    pub fn unbind(self: *const VertexArray) void {
        _ = self;
        callGl(gl.glBindBuffer().?, .{ gl.GL_ARRAY_BUFFER, 0 }) catch @breakpoint();
    }
};

pub const IndexArray = struct {
    renderer_id: u32,
    count: u32,
    pub fn init(data: [*]const u32, count: u32) !IndexArray {
        var index_buffer: u32 = undefined;
        try callGl(gl.glGenBuffers().?, .{ 1, &index_buffer });
        try callGl(gl.glBindBuffer().?, .{ gl.GL_ELEMENT_ARRAY_BUFFER, index_buffer });
        try callGl(
            gl.glBufferData().?,
            .{
                gl.GL_ELEMENT_ARRAY_BUFFER,
                count * @sizeOf(u32),
                data,
                gl.GL_STATIC_DRAW,
            },
        );

        try callGl(gl.glBindBuffer().?, .{ gl.GL_ELEMENT_ARRAY_BUFFER, 0 });
        return .{ .count = count, .renderer_id = index_buffer };
    }

    pub fn deinit(self: *const IndexArray) void {
        callGl(gl.glDeleteBuffers().?, .{ 1, &self.renderer_id }) catch @breakpoint();
    }

    pub fn bind(self: *const IndexArray) void {
        callGl(gl.glBindBuffer().?, .{ gl.GL_ELEMENT_ARRAY_BUFFER, self.renderer_id }) catch @breakpoint();
    }

    pub fn unbind(self: *const IndexArray) void {
        _ = self;
        callGl(gl.glBindBuffer().?, .{ gl.GL_ELEMENT_ARRAY_BUFFER, 0 }) catch @breakpoint();
    }
};

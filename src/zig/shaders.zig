const std = @import("std");
const ArrayList = std.ArrayList;
const Allocator = std.mem.Allocator;
const File = std.fs.File;

const gl = @import("gl.zig").gl;

const ShaderProgramSource = struct {
    vertex_source: [:0]const u8,
    fragment_source: [:0]const u8,

    pub fn deinit(self: ShaderProgramSource, alloc: Allocator) void{
        alloc.free(self.fragment_source);
        alloc.free(self.vertex_source);
    }
};

pub fn compileShaders(alloc: Allocator, shaderType: u32, sources: [][]const u8) Allocator.Error!u32 {
    const id = gl.glCreateShader().?(shaderType);

    const c_sources = try alloc.alloc([*c]const u8, sources.len);
    defer alloc.free(c_sources);

    for (0..c_sources.len) |i| c_sources[i] = sources[i].ptr;

    gl.glShaderSource().?(id, @intCast(c_sources.len), c_sources.ptr, null);
    gl.glCompileShader().?(id);

    var result: i32 = undefined;
    gl.glGetShaderiv().?(id, gl.GL_COMPILE_STATUS, &result);

    if (result == gl.GL_FALSE) {
        var length: i32 = undefined;
        gl.glGetShaderiv().?(id, gl.GL_INFO_LOG_LENGTH, &length);

        const message = try alloc.alloc(u8, @intCast(length));
        defer alloc.free(message);

        gl.glGetShaderInfoLog().?(id, length, &length, message.ptr);
        std.log.err(
            "\nShader Compilation Error on shader type: {s}\n",
            .{
                if (shaderType == gl.GL_VERTEX_SHADER) "VERTEX" else "FRAGMENT",
            },
        );
        std.log.err("Error: {s}\n\n", .{message});
    }

    return id;
}

pub fn compileShader(alloc: Allocator, shaderType: u32, source: []const u8) Allocator.Error!u32 {
    var sources = [_][]const u8{source};
    return compileShaders(alloc, shaderType, &sources);
}

pub fn createShader(alloc: Allocator, source: ShaderProgramSource) Allocator.Error!u32 {
    const program: u32 = gl.glCreateProgram().?();

    const vs: u32 = try compileShader(alloc, gl.GL_VERTEX_SHADER, source.vertex_source);
    const fs: u32 = try compileShader(alloc, gl.GL_FRAGMENT_SHADER, source.fragment_source);
    gl.glAttachShader().?(program, vs);
    gl.glAttachShader().?(program, fs);
    gl.glLinkProgram().?(program);
    gl.glValidateProgram().?(program);

    gl.glDeleteShader().?(vs);
    gl.glDeleteShader().?(fs);

    return program;
}

pub const ParserError = (File.OpenError || Allocator.Error || std.Io.Reader.DelimiterError);

/// Returns struct containing two owned string reads of file at file_path
/// struct members can be freed by calling: 
///
///     ShaderProgramSource.deinit(alloc);
///
pub fn parseShader(alloc: Allocator, file_path: []const u8) ParserError!ShaderProgramSource {
    const file = try std.fs.cwd().openFile(file_path, .{ .mode = .read_only });
    defer file.close();

    var read_buffer: [1024]u8 = undefined;
    var file_reader = file.reader(&read_buffer);

    const ShaderType = enum {
        vertex,
        fragment,
    };

    var shaders: std.EnumArray(ShaderType, ArrayList(u8)) = .init(.{
        .vertex = .empty,
        .fragment = .empty,
    });

    var shaderType: ?ShaderType = null;

    while (try file_reader.interface.takeDelimiter('\n')) |line| {
        if (std.mem.indexOf(u8, line, "#shader")) |_| {
            if (std.mem.indexOf(u8, line, "vertex")) |_| {
                shaderType = .vertex;
            } else if (std.mem.indexOf(u8, line, "fragment")) |_| {
                shaderType = .fragment;
            }
            continue;
        }

        if (shaderType) |t| {
            try shaders.getPtr(t).appendSlice(alloc, line);
            try shaders.getPtr(t).append(alloc, '\n');
        }
    }

    return .{
        .vertex_source = try shaders.getPtr(.vertex).toOwnedSliceSentinel(alloc, 0),
        .fragment_source = try shaders.getPtr(.fragment).toOwnedSliceSentinel(alloc, 0),
    };
}

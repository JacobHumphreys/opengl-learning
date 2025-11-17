const std = @import("std");
const builtin = @import("builtin");
const gl = @import("gl.zig").gl;

pub inline fn callGl(function : anytype, args : anytype) !void {
    clearGlErrors();
    _ = @call(.auto, function, args);
    try assert(logGlCall(@src().file, @src().line));
}

fn logGlCall(file : [] const u8, line : i32) bool {
    const err = gl.glGetError();
    while (err != 0) : (err = gl.glGetError()) {
            std.log.err(
                "[OpenGL Error]:\n\t[CODE] 0x{x}\n\t[ORIGIN] {s} (line: {})\n",
                . { err, file, line }, );
            return false;
        }
    return true;
}

inline fn clearGlErrors() void {
    while (gl.glGetError() != gl.GL_NO_ERROR) { }
}

const AssertionError = error { AssertionFailed };

pub fn assert(assertion : bool) AssertionError !void {
    if (assertion)
        return else {
            return AssertionError.AssertionFailed;
        }
}

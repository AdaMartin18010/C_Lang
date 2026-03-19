//
// Auto-generated from: Zig\Zig_Python_Binding.md
// Line: 183
// Language: zig
// Block ID: b61d8d07
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// pyzig.zig - Python 绑定辅助库

const std = @import("std");
const c = @cImport({
    @cDefine("PY_SSIZE_T_CLEAN", {});
    @cInclude("Python.h");
});

// Python 对象包装
pub const PyObject = struct {
    ptr: *c.PyObject,

    pub fn fromOwned(ptr: *c.PyObject) PyObject {
        return .{ .ptr = ptr };
    }

    pub fn fromBorrowed(ptr: *c.PyObject) PyObject {
        c.Py_INCREF(ptr);
        return .{ .ptr = ptr };
    }

    pub fn deinit(self: PyObject) void {
        c.Py_DECREF(self.ptr);
    }

    pub fn asLong(self: PyObject) !i64 {
        if (c.PyLong_Check(self.ptr) == 0) return error.TypeError;
        return c.PyLong_AsLong(self.ptr);
    }

    pub fn asDouble(self: PyObject) !f64 {
        if (c.PyFloat_Check(self.ptr) == 0) return error.TypeError;
        return c.PyFloat_AsDouble(self.ptr);
    }

    pub fn asString(self: PyObject) ![]const u8 {
        if (c.PyUnicode_Check(self.ptr) == 0) return error.TypeError;
        var size: c.Py_ssize_t = 0;
        const str = c.PyUnicode_AsUTF8AndSize(self.ptr, &size);
        if (str == null) return error.ConversionError;
        return str[0..@intCast(size)];
    }

    pub fn fromInt(val: i64) PyObject {
        return .{ .ptr = c.PyLong_FromLong(val) orelse unreachable };
    }

    pub fn fromFloat(val: f64) PyObject {
        return .{ .ptr = c.PyFloat_FromDouble(val) orelse unreachable };
    }

    pub fn fromString(s: []const u8) PyObject {
        return .{ .ptr = c.PyUnicode_FromStringAndSize(s.ptr, @intCast(s.len)) orelse unreachable };
    }

    pub fn listNew() PyObject {
        return .{ .ptr = c.PyList_New(0) orelse unreachable };
    }

    pub fn listAppend(self: PyObject, item: PyObject) !void {
        if (c.PyList_Append(self.ptr, item.ptr) < 0) return error.AppendError;
    }
};

// 函数包装宏
pub fn pyfn(comptime name: []const u8, comptime func: anytype) c.PyMethodDef {
    const Wrapper = struct {
        fn call(self: *c.PyObject, args: *c.PyObject) callconv(.C) ?*c.PyObject {
            return wrapFunction(func, self, args);
        }
    };

    return .{
        .ml_name = name.ptr,
        .ml_meth = @ptrCast(&Wrapper.call),
        .ml_flags = c.METH_VARARGS,
        .ml_doc = null,
    };
}

fn wrapFunction(comptime func: anytype, self: *c.PyObject, args: *c.PyObject) ?*c.PyObject {
    _ = self;

    const info = @typeInfo(@TypeOf(func));
    if (info != .Fn) @compileError("Expected function");

    const params = info.Fn.params;
    var py_args: [params.len]PyObject = undefined;

    // 解析参数
    comptime var format: [params.len + 1]u8 = undefined;
    comptime var i: usize = 0;
    inline while (i < params.len) : (i += 1) {
        const T = params[i].type.?;
        format[i] = switch (T) {
            i32, i64 => 'i',
            f32, f64 => 'd',
            []const u8 => 's',
            else => @compileError("Unsupported type"),
        };
    }
    format[params.len] = 0;

    // 解析参数 (简化)
    if (c.PyArg_ParseTuple(args, &format, &py_args[0]) == 0) {
        return null;
    }

    // 调用函数
    const result = @call(.auto, func, py_args);

    // 返回结果
    return switch (@TypeOf(result)) {
        i32, i64 => c.PyLong_FromLong(result),
        f32, f64 => c.PyFloat_FromDouble(result),
        []const u8 => c.PyUnicode_FromStringAndSize(result.ptr, @intCast(result.len)),
        void => blk: {
            c.Py_RETURN_NONE;
            break :blk null;
        },
        else => @compileError("Unsupported return type"),
    };
}

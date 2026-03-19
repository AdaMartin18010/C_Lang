//
// Auto-generated from: Zig\Zig_Python_Binding.md
// Line: 57
// Language: zig
// Block ID: 10ffc6b4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// mymodule.zig - Python 扩展模块

const std = @import("std");

// Python C API 绑定
const c = @cImport({
    @cDefine("PY_SSIZE_T_CLEAN", {});
    @cInclude("Python.h");
});

// 导出函数
export fn add(a: *c.PyObject, b: *c.PyObject) ?*c.PyObject {
    var x: c_long = 0;
    var y: c_long = 0;

    if (c.PyArg_ParseTuple(a, "ll", &x, &y) == 0) {
        return null;
    }

    return c.PyLong_FromLong(x + y);
}

// 方法定义
var methods = [_]c.PyMethodDef{
    .{
        .ml_name = "add",
        .ml_meth = @ptrCast(&add),
        .ml_flags = c.METH_VARARGS,
        .ml_doc = "Add two numbers.",
    },
    .{
        .ml_name = null,
        .ml_meth = null,
        .ml_flags = 0,
        .ml_doc = null,
    },
};

// 模块定义
var module = c.PyModuleDef{
    .m_base = .{
        .ob_base = .{ .ob_refcnt = 1, .ob_type = null },
        .m_init = null,
        .m_index = 0,
        .m_copy = null,
    },
    .m_name = "mymodule",
    .m_doc = "My Zig extension module.",
    .m_size = -1,
    .m_methods = &methods,
    .m_slots = null,
    .m_traverse = null,
    .m_clear = null,
    .m_free = null,
};

// 模块初始化函数
export fn PyInit_mymodule() ?*c.PyObject {
    return c.PyModule_Create(&module);
}

//
// Auto-generated from: Zig\Zig_Python_Binding.md
// Line: 311
// Language: zig
// Block ID: 455265ff
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// mylib.zig
const py = @import("pyzig.zig");

// 普通 Zig 函数
fn fibonacci(n: i64) i64 {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

fn processArray(arr: py.PyObject) !py.PyObject {
    const len = c.PyList_Size(arr.ptr);
    var result = py.PyObject.listNew();

    var i: c.Py_ssize_t = 0;
    while (i < len) : (i += 1) {
        const item = c.PyList_GetItem(arr.ptr, i);
        const val = try py.PyObject.fromBorrowed(item).asDouble();
        const processed = val * 2.0;
        try result.listAppend(py.PyObject.fromFloat(processed));
    }

    return result;
}

// 模块方法表
var methods = [_]c.PyMethodDef{
    py.pyfn("fibonacci", fibonacci),
    .{
        .ml_name = "process_array",
        .ml_meth = @ptrCast(&pyProcessArray),
        .ml_flags = c.METH_O,
        .ml_doc = "Process an array.",
    },
    .{ .ml_name = null, .ml_meth = null, .ml_flags = 0, .ml_doc = null },
};

export fn pyProcessArray(self: *c.PyObject, args: *c.PyObject) ?*c.PyObject {
    _ = self;
    const arr = py.PyObject.fromBorrowed(args);
    const result = processArray(arr) catch return null;
    return result.ptr;
}

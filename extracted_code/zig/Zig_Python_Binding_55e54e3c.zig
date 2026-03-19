//
// Auto-generated from: Zig\Zig_Python_Binding.md
// Line: 376
// Language: zig
// Block ID: 55e54e3c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// numpy.zig - NumPy 集成

const c = @cImport({
    @cDefine("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION");
    @cInclude("numpy/arrayobject.h");
});

pub const NumpyArray = struct {
    ptr: *c.PyArrayObject,

    pub fn fromPyObject(obj: *c.PyObject) !NumpyArray {
        if (c.PyArray_Check(obj) == 0) return error.NotNumpyArray;
        return .{ .ptr = @ptrCast(obj) };
    }

    pub fn shape(self: NumpyArray) []const c.npy_intp {
        const ndim = c.PyArray_NDIM(self.ptr);
        const dims = c.PyArray_DIMS(self.ptr);
        return dims[0..@intCast(ndim)];
    }

    pub fn data(self: NumpyArray) [*]u8 {
        return @ptrCast(c.PyArray_DATA(self.ptr));
    }

    pub fn dtype(self: NumpyArray) c_int {
        return c.PyArray_TYPE(self.ptr);
    }

    pub fn strides(self: NumpyArray) []const c.npy_intp {
        const ndim = c.PyArray_NDIM(self.ptr);
        const str = c.PyArray_STRIDES(self.ptr);
        return str[0..@intCast(ndim)];
    }

    pub fn len(self: NumpyArray) usize {
        return @intCast(c.PyArray_SIZE(self.ptr));
    }

    pub fn asSlice(self: NumpyArray, comptime T: type) []T {
        const ptr: [*]T = @alignCast(@ptrCast(self.data()));
        return ptr[0..self.len()];
    }
};

// 数组操作示例
export fn array_sum(self: *c.PyObject, args: *c.PyObject) ?*c.PyObject {
    _ = self;

    var arr_obj: ?*c.PyObject = null;
    if (c.PyArg_ParseTuple(args, "O", &arr_obj) == 0) return null;

    const arr = NumpyArray.fromPyObject(arr_obj.?) catch {
        c.PyErr_SetString(c.PyExc_TypeError, "Expected numpy array");
        return null;
    };

    const dtype = arr.dtype();
    var sum: f64 = 0;

    if (dtype == c.NPY_FLOAT64) {
        const data = arr.asSlice(f64);
        for (data) |val| {
            sum += val;
        }
    } else if (dtype == c.NPY_FLOAT32) {
        const data = arr.asSlice(f32);
        for (data) |val| {
            sum += @as(f64, val);
        }
    } else if (dtype == c.NPY_INT64) {
        const data = arr.asSlice(i64);
        for (data) |val| {
            sum += @as(f64, val);
        }
    } else {
        c.PyErr_SetString(c.PyExc_TypeError, "Unsupported dtype");
        return null;
    }

    return c.PyFloat_FromDouble(sum);
}

// SIMD 加速数组操作
export fn array_multiply(self: *c.PyObject, args: *c.PyObject) ?*c.PyObject {
    _ = self;

    var arr_obj: ?*c.PyObject = null;
    var scalar: f64 = 0;
    if (c.PyArg_ParseTuple(args, "Od", &arr_obj, &scalar) == 0) return null;

    const arr = NumpyArray.fromPyObject(arr_obj.?) catch {
        c.PyErr_SetString(c.PyExc_TypeError, "Expected numpy array");
        return null;
    };

    // 创建输出数组
    const dims = arr.shape();
    const out = c.PyArray_SimpleNew(@intCast(dims.len), dims.ptr, c.NPY_FLOAT64);
    if (out == null) return null;

    const in_data = arr.asSlice(f64);
    const out_data = @as([*]f64, @alignCast(@ptrCast(c.PyArray_DATA(out))))[0..in_data.len];

    // SIMD 乘法
    const V = @Vector(4, f64);
    const s = @as(V, @splat(scalar));

    var i: usize = 0;
    while (i + 4 <= in_data.len) : (i += 4) {
        const v: V = in_data[i..][0..4].*;
        out_data[i..][0..4].* = v * s;
    }

    // 剩余元素
    while (i < in_data.len) : (i += 1) {
        out_data[i] = in_data[i] * scalar;
    }

    return out;
}

# Zig Python 绑定：构建高性能 Python 扩展

> **定位**: Zig 生态 / FFI | **难度**: ⭐⭐⭐⭐ | **目标**: Zig 与 Python 互操作

---

## 📋 目录

- [Zig Python 绑定：构建高性能 Python 扩展](#zig-python-绑定构建高性能-python-扩展)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、Python C API 基础](#一python-c-api-基础)
    - [1.1 最小扩展模块](#11-最小扩展模块)
    - [1.2 Zig 构建配置](#12-zig-构建配置)
  - [二、使用 PyO3 风格](#二使用-pyo3-风格)
    - [2.1 包装器宏](#21-包装器宏)
    - [2.2 使用包装器](#22-使用包装器)
  - [三、NumPy 集成](#三numpy-集成)
    - [3.1 NumPy C API 绑定](#31-numpy-c-api-绑定)
  - [四、异步支持](#四异步支持)
  - [五、打包与发布](#五打包与发布)
    - [5.1 setup.py 配置](#51-setuppy-配置)
    - [5.2 pyproject.toml](#52-pyprojecttoml)
  - [六、实战：科学计算库](#六实战科学计算库)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [Zig Python 绑定：构建高性能 Python 扩展](#zig-python-绑定构建高性能-python-扩展)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、Python C API 基础](#一python-c-api-基础)
    - [1.1 最小扩展模块](#11-最小扩展模块)
    - [1.2 Zig 构建配置](#12-zig-构建配置)
  - [二、使用 PyO3 风格](#二使用-pyo3-风格)
    - [2.1 包装器宏](#21-包装器宏)
    - [2.2 使用包装器](#22-使用包装器)
  - [三、NumPy 集成](#三numpy-集成)
    - [3.1 NumPy C API 绑定](#31-numpy-c-api-绑定)
  - [四、异步支持](#四异步支持)
  - [五、打包与发布](#五打包与发布)
    - [5.1 setup.py 配置](#51-setuppy-配置)
    - [5.2 pyproject.toml](#52-pyprojecttoml)
  - [六、实战：科学计算库](#六实战科学计算库)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 一、Python C API 基础

### 1.1 最小扩展模块

```zig
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
```

```python
# setup.py

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../README.md](../README.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/README.md](../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/README.md](../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 知识库  
**难度等级**: L1-L6  
**前置依赖**: 核心知识体系  
**后续延伸**: 持续学习

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/README.md](../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/README.md](../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/README.md](../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/README.md](../04_Industrial_Scenarios/README.md) | 工业实践场景 |

### 【总体层】知识体系架构
```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/README.md) |

---

---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
from setuptools import setup, Extension

module = Extension(
    'mymodule',
    sources=['mymodule.c'],  # 由 Zig 生成
    extra_objects=['mymodule.o'],
)

setup(
    name='mymodule',
    version='1.0',
    description='Zig Python extension',
    ext_modules=[module],
)
```

### 1.2 Zig 构建配置

```zig
// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 获取 Python 配置
    const python_cflags = b.run(&.{"python3-config", "--cflags"});
    const python_ldflags = b.run(&.{"python3-config", "--ldflags"});

    // 创建共享库 (.so / .pyd)
    const lib = b.addSharedLibrary(.{
        .name = "mymodule",
        .root_source_file = .{ .path = "src/mymodule.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 链接 Python
    lib.linkLibC();
    lib.addIncludePath(.{ .path = "/usr/include/python3.11" });

    // 添加链接选项
    var it = std.mem.split(u8, python_ldflags, " ");
    while (it.next()) |flag| {
        if (flag.len > 2 and flag[0] == '-' and flag[1] == 'l') {
            lib.linkSystemLibrary(flag[2..]);
        }
    }

    b.installArtifact(lib);
}
```

---

## 二、使用 PyO3 风格

### 2.1 包装器宏

```zig
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
```

### 2.2 使用包装器

```zig
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
```

```python
# test_mylib.py
import mylib

# 测试斐波那契
result = mylib.fibonacci(10)
print(f"fib(10) = {result}")  # 55

# 测试数组处理
arr = [1.0, 2.0, 3.0, 4.0]
result = mylib.process_array(arr)
print(f"processed = {result}")  # [2.0, 4.0, 6.0, 8.0]
```

---

## 三、NumPy 集成

### 3.1 NumPy C API 绑定

```zig
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
```

---

## 四、异步支持

```zig
// asyncio.zig - Python 异步集成

const c = @cImport({
    @cInclude("Python.h");
});

// 创建协程
pub const Coroutine = struct {
    future: *c.PyObject,

    pub fn init() Coroutine {
        // 获取 asyncio 模块
        const asyncio = c.PyImport_ImportModule("asyncio");
        defer c.Py_DECREF(asyncio);

        // 创建 Future
        const future = c.PyObject_CallMethod(asyncio, "Future", null);

        return .{ .future = future.? };
    }

    pub fn setResult(self: Coroutine, result: *c.PyObject) void {
        c.PyObject_CallMethod(self.future, "set_result", "O", result);
    }

    pub fn asPyObject(self: Coroutine) *c.PyObject {
        return self.future;
    }
};

// 异步函数示例
export fn async_fetch(self: *c.PyObject, args: *c.PyObject) ?*c.PyObject {
    _ = self;

    var url: [*:0]u8 = null;
    if (c.PyArg_ParseTuple(args, "s", &url) == 0) return null;

    // 创建协程对象
    const coro = Coroutine.init();

    // 在后台线程执行 (实际应该使用 asyncio 的事件循环)
    const thread = std.Thread.spawn(.{}, struct {
        fn run(u: [*:0]u8, f: *c.PyObject) void {
            // 模拟 HTTP 请求
            std.time.sleep(1 * std.time.ns_per_s);

            // 设置结果
            const result = c.PyUnicode_FromString("Fetched: ");
            const url_str = c.PyUnicode_FromString(u);
            const combined = c.PyUnicode_Concat(result, url_str);

            c.PyObject_CallMethod(f, "set_result", "O", combined);

            c.Py_DECREF(result);
            c.Py_DECREF(url_str);
        }
    }.run, .{ url, coro.future }) catch {
        c.PyErr_SetString(c.PyExc_RuntimeError, "Failed to spawn thread");
        return null;
    };
    thread.detach();

    return coro.asPyObject();
}

const std = @import("std");
```

---

## 五、打包与发布

### 5.1 setup.py 配置

```python
# setup.py
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import subprocess
import os

class ZigBuilder(build_ext):
    def build_extension(self, ext):
        # 创建构建目录
        build_dir = os.path.join(self.build_temp, 'zig-out')
        os.makedirs(build_dir, exist_ok=True)

        # 运行 Zig 构建
        subprocess.check_call([
            'zig', 'build',
            '-Doptimize=ReleaseFast',
            f'-Dtarget=native',
        ])

        # 复制生成的库文件
        ext_path = self.get_ext_fullpath(ext.name)
        lib_file = os.path.join('zig-out', 'lib', f'lib{ext.name}.so')

        import shutil
        shutil.copy2(lib_file, ext_path)

ext_modules = [
    Extension(
        'myziglib',
        sources=[],  # Zig 会处理编译
    ),
]

setup(
    name='myziglib',
    version='1.0.0',
    description='High-performance Zig extension for Python',
    ext_modules=ext_modules,
    cmdclass={'build_ext': ZigBuilder},
    python_requires='>=3.8',
)
```

### 5.2 pyproject.toml

```toml
# pyproject.toml
[build-system]
requires = ["setuptools>=45", "wheel", "setuptools_scm>=6.2"]
build-backend = "setuptools.build_meta"

[project]
name = "myziglib"
version = "1.0.0"
description = "High-performance Zig extension for Python"
readme = "README.md"
requires-python = ">=3.8"
classifiers = [
    "Development Status :: 4 - Beta",
    "Intended Audience :: Developers",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
]

[project.optional-dependencies]
dev = ["pytest", "black", "mypy"]
```

---

## 六、实战：科学计算库

```zig
// scicalc.zig - 科学计算库

const std = @import("std");
const c = @cImport({
    @cDefine("PY_SSIZE_T_CLEAN", {});
    @cInclude("Python.h");
    @cDefine("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION");
    @cInclude("numpy/arrayobject.h");
});

// 矩阵乘法 (Zig 实现 + SIMD)
export fn matrix_multiply(self: *c.PyObject, args: *c.PyObject) ?*c.PyObject {
    _ = self;

    var a_obj: ?*c.PyObject = null;
    var b_obj: ?*c.PyObject = null;
    if (c.PyArg_ParseTuple(args, "OO", &a_obj, &b_obj) == 0) return null;

    // 获取数组
    if (c.PyArray_Check(a_obj.?) == 0 or c.PyArray_Check(b_obj.?) == 0) {
        c.PyErr_SetString(c.PyExc_TypeError, "Arguments must be numpy arrays");
        return null;
    }

    const a = @as(*c.PyArrayObject, @ptrCast(a_obj.?));
    const b = @as(*c.PyArrayObject, @ptrCast(b_obj.?));

    // 检查维度
    const a_dims = c.PyArray_DIMS(a);
    const b_dims = c.PyArray_DIMS(b);
    const m = a_dims[0];
    const n = a_dims[1];
    const p = b_dims[1];

    if (a_dims[1] != b_dims[0]) {
        c.PyErr_SetString(c.PyExc_ValueError, "Incompatible matrix dimensions");
        return null;
    }

    // 创建输出数组
    const out_dims = [2]c.npy_intp{ m, p };
    const out = c.PyArray_SimpleNew(2, &out_dims, c.NPY_FLOAT64);
    if (out == null) return null;

    const a_data = @as([*]f64, @alignCast(@ptrCast(c.PyArray_DATA(a))));
    const b_data = @as([*]f64, @alignCast(@ptrCast(c.PyArray_DATA(b))));
    const c_data = @as([*]f64, @alignCast(@ptrCast(c.PyArray_DATA(out))));

    // 矩阵乘法 (分块 + SIMD)
    const BLOCK = 64;

    var i: usize = 0;
    while (i < m) : (i += 1) {
        var j: usize = 0;
        while (j < p) : (j += 1) {
            var sum: f64 = 0;
            var k: usize = 0;

            // SIMD 内积
            const V = @Vector(4, f64);
            var vec_sum: V = @splat(0);

            while (k + 4 <= n) : (k += 4) {
                const a_vec: V = @as([*]V, @alignCast(@ptrCast(&a_data[i * n + k])))[0];

                var b_vec: V = undefined;
                comptime var vi: usize = 0;
                inline while (vi < 4) : (vi += 1) {
                    b_vec[vi] = b_data[(k + vi) * p + j];
                }

                vec_sum += a_vec * b_vec;
            }

            sum = vec_sum[0] + vec_sum[1] + vec_sum[2] + vec_sum[3];

            // 剩余元素
            while (k < n) : (k += 1) {
                sum += a_data[i * n + k] * b_data[k * p + j];
            }

            c_data[i * p + j] = sum;
        }
    }

    return out;
}

// K-Means 聚类
export fn kmeans(self: *c.PyObject, args: *c.PyObject) ?*c.PyObject {
    _ = self;

    var data_obj: ?*c.PyObject = null;
    var k: i64 = 3;
    var max_iter: i64 = 100;

    if (c.PyArg_ParseTuple(args, "O|ll", &data_obj, &k, &max_iter) == 0) return null;

    // 获取数据
    if (c.PyArray_Check(data_obj.?) == 0) {
        c.PyErr_SetString(c.PyExc_TypeError, "Data must be numpy array");
        return null;
    }

    const data = @as(*c.PyArrayObject, @ptrCast(data_obj.?));
    const n_samples = c.PyArray_DIM(data, 0);
    const n_features = c.PyArray_DIM(data, 1);
    const data_ptr = @as([*]f64, @alignCast(@ptrCast(c.PyArray_DATA(data))));

    // 分配中心点
    const centers = c.PyArray_SimpleNew(2, &[2]c.npy_intp{ k, n_features }, c.NPY_FLOAT64);
    const labels = c.PyArray_SimpleNew(1, &[1]c.npy_intp{n_samples}, c.NPY_INT64);

    // K-Means 算法 (简化版)
    // 1. 随机初始化中心点
    // 2. 迭代直到收敛
    // ...

    _ = data_ptr;
    _ = max_iter;

    // 返回 (centers, labels)
    return c.PyTuple_Pack(2, centers, labels);
}
```

```python
# benchmark.py
import numpy as np
import time
import myziglib

def benchmark_matrix_multiply():
    # 创建大矩阵
    a = np.random.rand(1000, 1000)
    b = np.random.rand(1000, 1000)

    # NumPy 版本
    start = time.time()
    result_np = a @ b
    np_time = time.time() - start
    print(f"NumPy: {np_time:.3f}s")

    # Zig 版本
    start = time.time()
    result_zig = myziglib.matrix_multiply(a, b)
    zig_time = time.time() - start
    print(f"Zig: {zig_time:.3f}s")
    print(f"Speedup: {np_time/zig_time:.2f}x")

    # 验证结果
    assert np.allclose(result_np, result_zig)

if __name__ == "__main__":
    benchmark_matrix_multiply()
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review

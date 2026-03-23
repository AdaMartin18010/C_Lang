# Zig WebAssembly 开发：浏览器与 WASI 全栈

> **定位**: Zig 应用 / WebAssembly | **难度**: ⭐⭐⭐⭐ | **目标**: Web、边缘计算、插件系统

---

## 📋 目录

- [Zig WebAssembly 开发：浏览器与 WASI 全栈](#zig-webassembly-开发浏览器与-wasi-全栈)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、为什么选择 Zig + WASM](#一为什么选择-zig--wasm)
  - [二、WASM 基础](#二wasm-基础)
    - [2.1 Zig 编译到 WASM](#21-zig-编译到-wasm)
    - [2.2 最小 WASM 模块](#22-最小-wasm-模块)
    - [2.3 内存管理策略](#23-内存管理策略)
  - [三、浏览器端开发](#三浏览器端开发)
    - [3.1 DOM 操作](#31-dom-操作)
    - [3.2 HTML 与 JavaScript 胶水代码](#32-html-与-javascript-胶水代码)
  - [四、WASI 服务器端](#四wasi-服务器端)
    - [4.1 命令行工具](#41-命令行工具)
  - [五、与 JavaScript 互操作](#五与-javascript-互操作)
    - [5.1 复杂类型传递](#51-复杂类型传递)
    - [5.2 JavaScript 端封装](#52-javascript-端封装)
  - [六、性能优化](#六性能优化)
    - [6.1 编译优化](#61-编译优化)
    - [6.2 SIMD 优化](#62-simd-优化)
  - [七、实战：图像处理库](#七实战图像处理库)
  - [八、实战：游戏引擎](#八实战游戏引擎)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [Zig WebAssembly 开发：浏览器与 WASI 全栈](#zig-webassembly-开发浏览器与-wasi-全栈)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、为什么选择 Zig + WASM](#一为什么选择-zig--wasm)
  - [二、WASM 基础](#二wasm-基础)
    - [2.1 Zig 编译到 WASM](#21-zig-编译到-wasm)
    - [2.2 最小 WASM 模块](#22-最小-wasm-模块)
    - [2.3 内存管理策略](#23-内存管理策略)
  - [三、浏览器端开发](#三浏览器端开发)
    - [3.1 DOM 操作](#31-dom-操作)
    - [3.2 HTML 与 JavaScript 胶水代码](#32-html-与-javascript-胶水代码)
  - [四、WASI 服务器端](#四wasi-服务器端)
    - [4.1 命令行工具](#41-命令行工具)
  - [五、与 JavaScript 互操作](#五与-javascript-互操作)
    - [5.1 复杂类型传递](#51-复杂类型传递)
    - [5.2 JavaScript 端封装](#52-javascript-端封装)
  - [六、性能优化](#六性能优化)
    - [6.1 编译优化](#61-编译优化)
    - [6.2 SIMD 优化](#62-simd-优化)
  - [七、实战：图像处理库](#七实战图像处理库)
  - [八、实战：游戏引擎](#八实战游戏引擎)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 一、为什么选择 Zig + WASM

```
┌─────────────────────────────────────────────────────────────────────┐
│              Zig 编译到 WebAssembly 的优势                            │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  1. 小体积                                                           │
│     - 无运行时开销                                                    │
│     - 标准库可选                                                      │
│     - 典型输出: 10KB-100KB vs C++ 的 MB 级                            │
│                                                                      │
│  2. 高性能                                                           │
│     - 直接编译到 WASM，无解释器                                       │
│     -  SIMD 支持                                                      │
│     -  内存布局控制                                                   │
│                                                                      │
│  3. 易用性                                                           │
│     - 内置交叉编译: zig build -Dtarget=wasm32-freestanding           │
│     - 无需额外工具链                                                   │
│     -  C 头文件直接导入                                                │
│                                                                      │
│  4. 安全性                                                           │
│     -  类型安全                                                       │
│     -  显式内存管理                                                   │
│     -  无未定义行为(调试模式)                                          │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 二、WASM 基础

### 2.1 Zig 编译到 WASM

```bash
# 浏览器目标 (无系统 API)

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
zig build -Dtarget=wasm32-freestanding -Doptimize=ReleaseSmall

# WASI 目标 (有文件系统等)
zig build -Dtarget=wasm32-wasi -Doptimize=ReleaseFast

# 生成不同格式
zig build-lib -target wasm32-freestanding -dynamic -O ReleaseSmall src/lib.zig
```

### 2.2 最小 WASM 模块

```zig
// lib.zig - 可导出的 WASM 库

// 导出函数给 JavaScript
export fn add(a: i32, b: i32) i32 {
    return a + b;
}

export fn factorial(n: u32) u32 {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// 内存管理
extern "env" fn malloc(size: usize) ?[*]u8;
extern "env" fn free(ptr: [*]u8) void;

export fn alloc_buffer(size: usize) ?[*]u8 {
    return malloc(size);
}

export fn free_buffer(ptr: [*]u8) void {
    free(ptr);
}
```

### 2.3 内存管理策略

```zig
// wasm_allocator.zig

const std = @import("std");

// 使用 WebAssembly 内存作为堆
var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();

// 导出的内存
export var memory: [1024 * 1024]u8 align(16) = undefined;  // 1MB 初始内存

// 使用固定缓冲区分配器
var fba = std.heap.FixedBufferAllocator.init(&memory);
export const wasm_allocator = fba.allocator();

// 导出分配函数
export fn wasm_alloc(size: usize) ?[*]u8 {
    const ptr = wasm_allocator.alloc(u8, size) catch return null;
    return ptr.ptr;
}

export fn wasm_free(ptr: [*]u8, size: usize) void {
    wasm_allocator.free(ptr[0..size]);
}
```

---

## 三、浏览器端开发

### 3.1 DOM 操作

```zig
// dom.zig - Web API 绑定

// JavaScript 函数声明
extern "env" {
    fn js_console_log(ptr: [*]const u8, len: usize) void;
    fn js_document_get_element_by_id(ptr: [*]const u8, len: usize) u32;
    fn js_element_set_inner_html(element: u32, ptr: [*]const u8, len: usize) void;
    fn js_alert(ptr: [*]const u8, len: usize) void;
}

// Zig 包装函数
pub fn console_log(msg: []const u8) void {
    js_console_log(msg.ptr, msg.len);
}

pub fn alert(msg: []const u8) void {
    js_alert(msg.ptr, msg.len);
}

pub fn getElementById(id: []const u8) u32 {
    return js_document_get_element_by_id(id.ptr, id.len);
}

pub fn setInnerHtml(element: u32, html: []const u8) void {
    js_element_set_inner_html(element, html.ptr, html.len);
}

// 导出初始化函数
export fn init() void {
    console_log("Zig WASM initialized!");
}

export fn update_counter(value: i32) void {
    var buf: [256]u8 = undefined;
    const msg = std.fmt.bufPrint(&buf, "Count: {}", .{value}) catch return;

    const counter = getElementById("counter");
    setInnerHtml(counter, msg);
}
```

### 3.2 HTML 与 JavaScript 胶水代码

```html
<!-- index.html -->
<!DOCTYPE html>
<html>
<head>
    <title>Zig WASM Demo</title>
</head>
<body>
    <h1>Zig WebAssembly Demo</h1>
    <div id="counter">Count: 0</div>
    <button id="increment">Increment</button>

    <script type="module">
        const wasmModule = await WebAssembly.instantiateStreaming(
            fetch("zig.wasm"),
            {
                env: {
                    memory: new WebAssembly.Memory({ initial: 256 }),

                    js_console_log: (ptr, len) => {
                        const bytes = new Uint8Array(memory.buffer, ptr, len);
                        console.log(new TextDecoder().decode(bytes));
                    },

                    js_document_get_element_by_id: (ptr, len) => {
                        const id = new TextDecoder().decode(
                            new Uint8Array(memory.buffer, ptr, len)
                        );
                        return 1; // 简化
                    },

                    js_alert: (ptr, len) => {
                        alert(new TextDecoder().decode(
                            new Uint8Array(memory.buffer, ptr, len)
                        ));
                    }
                }
            }
        );

        const { exports } = wasmModule.instance;
        const memory = exports.memory;

        exports.init();

        let count = 0;
        document.getElementById('increment').onclick = () => {
            count++;
            exports.update_counter(count);
        };
    </script>
</body>
</html>
```

---

## 四、WASI 服务器端

### 4.1 命令行工具

```zig
// cli.zig - WASI 命令行程序

const std = @import("std");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len < 2) {
        try std.io.getStdErr().writeAll("Usage: program <file>\n");
        return error.InvalidArguments;
    }

    const content = std.fs.cwd().readFileAlloc(allocator, args[1], 1024 * 1024) catch |err| {
        try std.io.getStdErr().writer().print("Error: {}\n", .{err});
        return err;
    };
    defer allocator.free(content);

    const word_count = count_words(content);
    const line_count = count_lines(content);

    try std.io.getStdOut().writer().print(
        "Words: {} Lines: {} Bytes: {}\n",
        .{ word_count, line_count, content.len }
    );
}

fn count_words(text: []const u8) usize {
    var count: usize = 0;
    var in_word = false;
    for (text) |c| {
        if (std.ascii.isWhitespace(c)) {
            if (in_word) count += 1;
            in_word = false;
        } else {
            in_word = true;
        }
    }
    if (in_word) count += 1;
    return count;
}

fn count_lines(text: []const u8) usize {
    var count: usize = 1;
    for (text) |c| {
        if (c == '\n') count += 1;
    }
    return count;
}
```

---

## 五、与 JavaScript 互操作

### 5.1 复杂类型传递

```zig
// interop.zig

// 结构体定义
const Point = extern struct {
    x: f64,
    y: f64,
};

const Rect = extern struct {
    top_left: Point,
    bottom_right: Point,
};

// 导出结构体操作
export fn create_point(x: f64, y: f64) Point {
    return .{ .x = x, .y = y };
}

export fn point_distance(a: Point, b: Point) f64 {
    const dx = a.x - b.x;
    const dy = a.y - b.y;
    return std.math.sqrt(dx * dx + dy * dy);
}

export fn rect_area(r: Rect) f64 {
    const width = r.bottom_right.x - r.top_left.x;
    const height = r.bottom_right.y - r.top_left.y;
    return width * height;
}

// 数组操作
export fn sum_array(ptr: [*]const f64, len: usize) f64 {
    var sum: f64 = 0;
    for (0..len) |i| {
        sum += ptr[i];
    }
    return sum;
}
```

### 5.2 JavaScript 端封装

```javascript
// zig-wrapper.js

class ZigModule {
    constructor(wasmModule) {
        this.exports = wasmModule.instance.exports;
        this.memory = this.exports.memory;
    }

    readString(ptr, len) {
        return new TextDecoder().decode(
            new Uint8Array(this.memory.buffer, ptr, len)
        );
    }

    writeString(str) {
        const bytes = new TextEncoder().encode(str);
        const ptr = this.exports.wasm_alloc(bytes.length);
        new Uint8Array(this.memory.buffer).set(bytes, ptr);
        return { ptr, len: bytes.length };
    }

    readPoint(ptr) {
        const view = new DataView(this.memory.buffer, ptr, 16);
        return {
            x: view.getFloat64(0, true),
            y: view.getFloat64(8, true)
        };
    }

    add(a, b) {
        return this.exports.add(a, b);
    }
}

export { ZigModule };
```

---

## 六、性能优化

### 6.1 编译优化

```bash
# 调试模式 (大文件，含调试信息)
zig build -Dtarget=wasm32-freestanding

# 发布小体积模式 (推荐用于 Web)
zig build -Dtarget=wasm32-freestanding -Doptimize=ReleaseSmall

# 发布高性能模式
zig build -Dtarget=wasm32-freestanding -Doptimize=ReleaseFast
```

### 6.2 SIMD 优化

```zig
// simd.zig - WebAssembly SIMD

pub fn add_vectors_simd(a: []const f32, b: []const f32, result: []f32) void {
    const V = @Vector(4, f32);

    var i: usize = 0;
    while (i + 4 <= a.len) : (i += 4) {
        const va: V = a[i..][0..4].*;
        const vb: V = b[i..][0..4].*;
        const vr = va + vb;
        result[i..][0..4].* = vr;
    }

    while (i < a.len) : (i += 1) {
        result[i] = a[i] + b[i];
    }
}

pub fn dot_product_simd(a: []const f32, b: []const f32) f32 {
    const V = @Vector(4, f32);

    var sum_vec: V = @splat(0);

    var i: usize = 0;
    while (i + 4 <= a.len) : (i += 4) {
        const va: V = a[i..][0..4].*;
        const vb: V = b[i..][0..4].*;
        sum_vec += va * vb;
    }

    var sum = sum_vec[0] + sum_vec[1] + sum_vec[2] + sum_vec[3];

    while (i < a.len) : (i += 1) {
        sum += a[i] * b[i];
    }

    return sum;
}
```

---

## 七、实战：图像处理库

```zig
// image.zig - 图像处理 WASM 库

const std = @import("std");

pub const Image = extern struct {
    width: u32,
    height: u32,
    data: [*]u8,  // RGBA 数据

    pub fn get_pixel(self: Image, x: u32, y: u32) [4]u8 {
        const idx = (y * self.width + x) * 4;
        return .{
            self.data[idx],
            self.data[idx + 1],
            self.data[idx + 2],
            self.data[idx + 3],
        };
    }

    pub fn set_pixel(self: Image, x: u32, y: u32, r: u8, g: u8, b: u8, a: u8) void {
        const idx = (y * self.width + x) * 4;
        self.data[idx] = r;
        self.data[idx + 1] = g;
        self.data[idx + 2] = b;
        self.data[idx + 3] = a;
    }
};

// 灰度转换
export fn grayscale(image: Image) void {
    var y: u32 = 0;
    while (y < image.height) : (y += 1) {
        var x: u32 = 0;
        while (x < image.width) : (x += 1) {
            const pixel = image.get_pixel(x, y);
            const gray = @as(u8, @intFromFloat(
                0.299 * @as(f32, @floatFromInt(pixel[0])) +
                0.587 * @as(f32, @floatFromInt(pixel[1])) +
                0.114 * @as(f32, @floatFromInt(pixel[2]))
            ));
            image.set_pixel(x, y, gray, gray, gray, pixel[3]);
        }
    }
}

// 亮度调整
export fn adjust_brightness(image: Image, factor: f32) void {
    var y: u32 = 0;
    while (y < image.height) : (y += 1) {
        var x: u32 = 0;
        while (x < image.width) : (x += 1) {
            const pixel = image.get_pixel(x, y);
            const r = @min(255, @as(u32, @intFromFloat(@as(f32, @floatFromInt(pixel[0])) * factor)));
            const g = @min(255, @as(u32, @intFromFloat(@as(f32, @floatFromInt(pixel[1])) * factor)));
            const b = @min(255, @as(u32, @intFromFloat(@as(f32, @floatFromInt(pixel[2])) * factor)));
            image.set_pixel(x, y, @intCast(r), @intCast(g), @intCast(b), pixel[3]);
        }
    }
}
```

---

## 八、实战：游戏引擎

```zig
// game.zig - 简单 WASM 游戏引擎

const std = @import("std");

// 游戏状态
var player_x: f32 = 400;
var player_y: f32 = 300;

const Bullet = struct {
    x: f32,
    y: f32,
    active: bool,
};

var bullets: [100]Bullet = undefined;

// 输入状态 (由 JS 更新)
export var input_up: bool = false;
export var input_down: bool = false;
export var input_left: bool = false;
export var input_right: bool = false;

export fn init() void {
    player_x = 400;
    player_y = 300;
    for (&bullets) |*b| b.active = false;
}

export fn update(dt: f32) void {
    const speed = 300;

    if (input_left) player_x -= speed * dt;
    if (input_right) player_x += speed * dt;
    if (input_up) player_y -= speed * dt;
    if (input_down) player_y += speed * dt;

    player_x = std.math.clamp(player_x, 20, 780);
    player_y = std.math.clamp(player_y, 20, 580);
}

export fn get_player_x() f32 { return player_x; }
export fn get_player_y() f32 { return player_y; }
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
> - 目标: WebAssembly 全栈开发
> - 应用: Web 应用、边缘计算、插件系统


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

# Zig 性能优化：从算法到 SIMD

> **定位**: Zig 核心 / 性能优化 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: 榨取硬件最大性能

---

## 📋 目录

- [Zig 性能优化：从算法到 SIMD](#zig-性能优化从算法到-simd)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、性能分析基础](#一性能分析基础)
    - [1.1 基准测试](#11-基准测试)
    - [1.2 性能计数器](#12-性能计数器)
  - [二、算法优化](#二算法优化)
    - [2.1 算法复杂度选择](#21-算法复杂度选择)
    - [2.2 缓存友好的数据结构](#22-缓存友好的数据结构)
    - [2.3 分支预测优化](#23-分支预测优化)
  - [三、内存优化](#三内存优化)
    - [3.1 内存分配策略](#31-内存分配策略)
    - [3.2 内存对齐](#32-内存对齐)
    - [3.3 零拷贝技术](#33-零拷贝技术)
  - [四、SIMD 向量化](#四simd-向量化)
    - [4.1 基础 SIMD](#41-基础-simd)
    - [4.2 矩阵运算](#42-矩阵运算)
    - [4.3 图像处理 SIMD](#43-图像处理-simd)
  - [五、缓存优化](#五缓存优化)
    - [5.1 缓存行优化](#51-缓存行优化)
    - [5.2 预取](#52-预取)
  - [六、编译器优化](#六编译器优化)
    - [6.1 编译模式](#61-编译模式)
    - [6.2 内联与展开](#62-内联与展开)
    - [6.3 链接时优化 (LTO)](#63-链接时优化-lto)
  - [七、实战：高性能计算](#七实战高性能计算)
    - [7.1 快速傅里叶变换 (FFT)](#71-快速傅里叶变换-fft)
    - [7.2 高性能日志系统](#72-高性能日志系统)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [Zig 性能优化：从算法到 SIMD](#zig-性能优化从算法到-simd)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、性能分析基础](#一性能分析基础)
    - [1.1 基准测试](#11-基准测试)
    - [1.2 性能计数器](#12-性能计数器)
  - [二、算法优化](#二算法优化)
    - [2.1 算法复杂度选择](#21-算法复杂度选择)
    - [2.2 缓存友好的数据结构](#22-缓存友好的数据结构)
    - [2.3 分支预测优化](#23-分支预测优化)
  - [三、内存优化](#三内存优化)
    - [3.1 内存分配策略](#31-内存分配策略)
    - [3.2 内存对齐](#32-内存对齐)
    - [3.3 零拷贝技术](#33-零拷贝技术)
  - [四、SIMD 向量化](#四simd-向量化)
    - [4.1 基础 SIMD](#41-基础-simd)
    - [4.2 矩阵运算](#42-矩阵运算)
    - [4.3 图像处理 SIMD](#43-图像处理-simd)
  - [五、缓存优化](#五缓存优化)
    - [5.1 缓存行优化](#51-缓存行优化)
    - [5.2 预取](#52-预取)
  - [六、编译器优化](#六编译器优化)
    - [6.1 编译模式](#61-编译模式)
    - [6.2 内联与展开](#62-内联与展开)
    - [6.3 链接时优化 (LTO)](#63-链接时优化-lto)
  - [七、实战：高性能计算](#七实战高性能计算)
    - [7.1 快速傅里叶变换 (FFT)](#71-快速傅里叶变换-fft)
    - [7.2 高性能日志系统](#72-高性能日志系统)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 一、性能分析基础

### 1.1 基准测试

```zig
const std = @import("std");
const time = std.time;

// 高精度计时器
pub const Timer = struct {
    start_time: i128,
    name: []const u8,

    pub fn start(name: []const u8) Timer {
        return .{
            .start_time = time.nanoTimestamp(),
            .name = name,
        };
    }

    pub fn stop(self: Timer) void {
        const elapsed = time.nanoTimestamp() - self.start_time;
        const ms = @as(f64, @floatFromInt(elapsed)) / time.ns_per_ms;
        std.debug.print("[{}] {d:.3} ms\n", .{ self.name, ms });
    }
};

// 使用示例
pub fn benchmarkExample() void {
    const timer = Timer.start("computation");
    defer timer.stop();

    // 被测试的代码
    var sum: u64 = 0;
    for (0..1_000_000) |i| {
        sum += i;
    }
    std.mem.doNotOptimizeAway(sum);
}

// 多次运行的基准测试
pub fn benchmarkMultiple(
    comptime name: []const u8,
    comptime iterations: usize,
    func: fn () void,
) void {
    var total_time: i128 = 0;
    var min_time: i128 = std.math.maxInt(i128);
    var max_time: i128 = 0;

    var i: usize = 0;
    while (i < iterations) : (i += 1) {
        const start = time.nanoTimestamp();
        func();
        const elapsed = time.nanoTimestamp() - start;

        total_time += elapsed;
        min_time = @min(min_time, elapsed);
        max_time = @max(max_time, elapsed);
    }

    const avg = @as(f64, @floatFromInt(total_time)) / @as(f64, @floatFromInt(iterations)) / time.ns_per_ms;
    const min_ms = @as(f64, @floatFromInt(min_time)) / time.ns_per_ms;
    const max_ms = @as(f64, @floatFromInt(max_time)) / time.ns_per_ms;

    std.debug.print(
        \\[{s}] {} iterations
        \\  avg: {d:.3} ms
        \\  min: {d:.3} ms
        \\  max: {d:.3} ms
        \\
    , .{ name, iterations, avg, min_ms, max_ms });
}
```

### 1.2 性能计数器

```zig
// 使用 CPU 性能计数器 (Linux)
const std = @import("std");

pub const PerfCounter = struct {
    fd: i32,

    pub fn init(config: PerfConfig) !PerfCounter {
        const PERF_TYPE_HARDWARE = 0;
        const PERF_COUNT_HW_CPU_CYCLES = 0;
        const PERF_COUNT_HW_INSTRUCTIONS = 1;
        const PERF_COUNT_HW_CACHE_REFERENCES = 2;
        const PERF_COUNT_HW_CACHE_MISSES = 3;

        var attr: std.os.linux.perf_event_attr = .{
            .type = PERF_TYPE_HARDWARE,
            .config = switch (config) {
                .cycles => PERF_COUNT_HW_CPU_CYCLES,
                .instructions => PERF_COUNT_HW_INSTRUCTIONS,
                .cache_refs => PERF_COUNT_HW_CACHE_REFERENCES,
                .cache_misses => PERF_COUNT_HW_CACHE_MISSES,
            },
            .size = @sizeOf(std.os.linux.perf_event_attr),
            .flags = .{},
        };

        const fd = std.os.linux.perf_event_open(&attr, 0, -1, -1, 0);
        if (fd < 0) return error.PerfEventOpenFailed;

        return .{ .fd = fd };
    }

    pub fn deinit(self: PerfCounter) void {
        std.os.close(self.fd);
    }

    pub fn read(self: PerfCounter) u64 {
        var value: u64 = 0;
        _ = std.os.read(self.fd, std.mem.asBytes(&value)) catch return 0;
        return value;
    }

    pub fn reset(self: PerfCounter) void {
        const PERF_EVENT_IOC_RESET = 3;
        _ = std.os.linux.ioctl(self.fd, PERF_EVENT_IOC_RESET, 0);
    }
};

pub const PerfConfig = enum {
    cycles,
    instructions,
    cache_refs,
    cache_misses,
};
```

---

## 二、算法优化

### 2.1 算法复杂度选择

```zig
// 查找算法对比

// O(n) 线性查找
pub fn linearSearch(haystack: []const i32, needle: i32) ?usize {
    for (haystack, 0..) |item, i| {
        if (item == needle) return i;
    }
    return null;
}

// O(log n) 二分查找 (要求已排序)
pub fn binarySearch(haystack: []const i32, needle: i32) ?usize {
    var left: usize = 0;
    var right = haystack.len;

    while (left < right) {
        const mid = left + (right - left) / 2;
        if (haystack[mid] == needle) return mid;
        if (haystack[mid] < needle) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return null;
}

// O(1) 哈希查找
pub fn HashSet(comptime T: type) type {
    return std.HashMap(T, void, std.hash_map.defaultContext(T), 80);
}
```

### 2.2 缓存友好的数据结构

```zig
// 面向数据设计: Structure of Arrays (SoA)
// 优于 Array of Structures (AoS)

// ❌ AoS - 缓存不友好
const ParticleAoS = struct {
    x: f32,
    y: f32,
    z: f32,
    vx: f32,
    vy: f32,
    vz: f32,
    mass: f32,
    active: bool,
};

// ✅ SoA - 缓存友好
const ParticleSoA = struct {
    x: []f32,
    y: []f32,
    z: []f32,
    vx: []f32,
    vy: []f32,
    vz: []f32,
    mass: []f32,
    active: []bool,

    pub fn updatePositions(self: ParticleSoA, dt: f32) void {
        // 连续内存访问，缓存命中率高
        for (self.x, self.y, self.z, self.vx, self.vy, self.vz, self.active) |*x, *y, *z, vx, vy, vz, active| {
            if (!active) continue;
            x.* += vx * dt;
            y.* += vy * dt;
            z.* += vz * dt;
        }
    }
};
```

### 2.3 分支预测优化

```zig
// 分支预测友好的代码

// ❌ 随机分支
pub fn sumWithBranch(data: []const i32) i64 {
    var sum: i64 = 0;
    for (data) |x| {
        if (x > 0) {  // 随机数据导致分支预测失败
            sum += x;
        }
    }
    return sum;
}

// ✅ 排序后处理 (分支更可预测)
pub fn sumSorted(data: []i32) i64 {
    std.sort.sort(i32, data, {}, comptime std.sort.asc(i32));

    var sum: i64 = 0;
    for (data) |x| {
        if (x <= 0) break;  // 一旦遇到负数就停止
        sum += x;
    }
    return sum;
}

// ✅ 使用条件移动
pub fn conditionalSelect(condition: bool, a: i32, b: i32) i32 {
    // 编译器通常会优化为 CMOV 指令
    return if (condition) a else b;
}
```

---

## 三、内存优化

### 3.1 内存分配策略

```zig
const std = @import("std");

// 使用合适的分配器
pub fn allocationStrategies() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 1. 小对象使用固定缓冲区分配器
    var buffer: [4096]u8 = undefined;
    var fba = std.heap.FixedBufferAllocator.init(&buffer);
    const small_allocator = fba.allocator();

    const small_data = try small_allocator.alloc(u8, 100);
    defer small_allocator.free(small_data);

    // 2. Arena 分配器 - 批量释放
    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();
    const arena_allocator = arena.allocator();

    // 多次分配，只需一次释放
    _ = try arena_allocator.alloc(u8, 100);
    _ = try arena_allocator.alloc(u8, 200);
    _ = try arena_allocator.alloc(u8, 300);

    // 3. 对象池 - 重用对象
    const Pool = std.heap.MemoryPool(MyStruct);
    var pool = try Pool.initPreheated(allocator, 100);
    defer pool.deinit();

    const obj = try pool.create();
    defer pool.destroy(obj);
}

const MyStruct = struct {
    data: [256]u8,
    len: usize,
};
```

### 3.2 内存对齐

```zig
// 对齐优化

// 自然对齐
const AlignedStruct = extern struct {
    a: u8,      // offset 0
    // padding 7 bytes
    b: u64,     // offset 8
    c: u32,     // offset 16
    d: u8,      // offset 20
    // padding 3 bytes
    // total: 24 bytes
};

// 重新排序减少填充
const PackedStruct = extern struct {
    b: u64,     // offset 0
    c: u32,     // offset 8
    a: u8,      // offset 12
    d: u8,      // offset 13
    // padding 2 bytes
    // total: 16 bytes
};

// 显式对齐
const CacheAligned = extern struct {
    data: [64]u8 align(64),  // 64 字节对齐 (缓存行)
};

// SIMD 对齐
const SimdAligned = extern struct {
    vec: @Vector(8, f32) align(32),  // 32 字节对齐 (AVX)
};
```

### 3.3 零拷贝技术

```zig
// 零拷贝数据处理

// ❌ 拷贝数据
pub fn processWithCopy(data: []const u8) void {
    var buffer: [1024]u8 = undefined;
    @memcpy(&buffer, data);  // 拷贝
    processBuffer(&buffer);
}

// ✅ 直接处理，无需拷贝
pub fn processNoCopy(data: []const u8) void {
    processBuffer(data);  // 直接使用
}

// 使用切片避免拷贝
pub fn parseHeaders(data: []const u8) !HeaderMap {
    var map = HeaderMap.init();

    var lines = std.mem.split(u8, data, "\r\n");
    while (lines.next()) |line| {
        if (line.len == 0) break;

        // 解析但不拷贝
        const colon = std.mem.indexOf(u8, line, ": ") orelse continue;
        const key = line[0..colon];
        const value = line[colon + 2 ..];

        try map.put(key, value);  // 存储切片引用
    }

    return map;
}

const HeaderMap = std.StringHashMap([]const u8);
```

---

## 四、SIMD 向量化

### 4.1 基础 SIMD

```zig
// 使用 Zig 的向量类型

// 向量加法
pub fn vectorAdd(a: []const f32, b: []const f32, result: []f32) void {
    const V = @Vector(4, f32);

    const len = a.len;
    var i: usize = 0;

    // SIMD 处理
    while (i + 4 <= len) : (i += 4) {
        const va: V = a[i..][0..4].*;
        const vb: V = b[i..][0..4].*;
        const vr = va + vb;
        result[i..][0..4].* = vr;
    }

    // 处理剩余
    while (i < len) : (i += 1) {
        result[i] = a[i] + b[i];
    }
}

// 点积 (SIMD)
pub fn dotProductSimd(a: []const f32, b: []const f32) f32 {
    const V = @Vector(8, f32);

    var sum_vec: V = @splat(0);

    const len = a.len;
    var i: usize = 0;

    while (i + 8 <= len) : (i += 8) {
        const va: V = a[i..][0..8].*;
        const vb: V = b[i..][0..8].*;
        sum_vec += va * vb;
    }

    // 水平求和
    var sum = sum_vec[0] + sum_vec[1] + sum_vec[2] + sum_vec[3] +
              sum_vec[4] + sum_vec[5] + sum_vec[6] + sum_vec[7];

    // 剩余元素
    while (i < len) : (i += 1) {
        sum += a[i] * b[i];
    }

    return sum;
}
```

### 4.2 矩阵运算

```zig
// 矩阵乘法 (块矩阵 + SIMD)
pub fn matrixMultiply(
    a: []const f32,
    b: []const f32,
    c: []f32,
    n: usize,
) void {
    const BLOCK_SIZE = 64;
    const V = @Vector(4, f32);

    @memset(c, 0);

    var i: usize = 0;
    while (i < n) : (i += BLOCK_SIZE) {
        var j: usize = 0;
        while (j < n) : (j += BLOCK_SIZE) {
            var k: usize = 0;
            while (k < n) : (k += BLOCK_SIZE) {
                // 块矩阵乘法
                multiplyBlock(a, b, c, i, j, k, n, BLOCK_SIZE);
            }
        }
    }
}

fn multiplyBlock(
    a: []const f32,
    b: []const f32,
    c: []f32,
    i0: usize,
    j0: usize,
    k0: usize,
    n: usize,
    block_size: usize,
) void {
    const V = @Vector(4, f32);

    const i_max = @min(i0 + block_size, n);
    const j_max = @min(j0 + block_size, n);
    const k_max = @min(k0 + block_size, n);

    var i = i0;
    while (i < i_max) : (i += 1) {
        var j = j0;
        while (j + 4 <= j_max) : (j += 4) {
            var sum: V = @splat(0);

            var k = k0;
            while (k < k_max) : (k += 1) {
                const a_val = a[i * n + k];
                const b_vec: V = b[k * n + j ..][0..4].*;
                sum += @as(V, @splat(a_val)) * b_vec;
            }

            c[i * n + j ..][0..4].* += sum;
        }
    }
}
```

### 4.3 图像处理 SIMD

```zig
// RGB 到灰度转换 (SIMD)
pub fn rgbToGrayscaleSimd(input: []const u8, output: []u8, pixel_count: usize) void {
    const V = @Vector(16, u8);
    const VW = @Vector(16, u16);

    const r_coeff: u16 = 77;   // 0.299 * 256
    const g_coeff: u16 = 150;  // 0.587 * 256
    const b_coeff: u16 = 29;   // 0.114 * 256

    var i: usize = 0;
    while (i + 48 <= pixel_count * 3) : (i += 48) {
        // 加载 16 个像素的 RGB 数据 (48 字节)
        const rgb_data = input[i..][0..48].*;

        // 分离 R, G, B
        var gray_pixels: [16]u8 = undefined;

        var j: usize = 0;
        while (j < 16) : (j += 1) {
            const r = @as(u16, rgb_data[j * 3]);
            const g = @as(u16, rgb_data[j * 3 + 1]);
            const b = @as(u16, rgb_data[j * 3 + 2]);

            const gray = (r * r_coeff + g * g_coeff + b * b_coeff) >> 8;
            gray_pixels[j] = @truncate(gray);
        }

        @memcpy(output[i / 3 ..][0..16], &gray_pixels);
    }
}
```

---

## 五、缓存优化

### 5.1 缓存行优化

```zig
// 避免伪共享 (False Sharing)

// ❌ 多个线程修改相邻数据
const BadCounter = struct {
    counters: [8]u64,  // 在同一个缓存行
};

// ✅ 填充到独立缓存行
const CACHE_LINE = 64;
const GoodCounter = struct {
    counters: [8]align(CACHE_LINE) CacheAlignedCounter,
};

const CacheAlignedCounter = extern struct {
    value: u64,
    padding: [CACHE_LINE - @sizeOf(u64)]u8,
};

// 使用
pub fn incrementCounters(counters: *GoodCounter) void {
    var threads: [8]std.Thread = undefined;

    for (&threads, 0..) |*t, i| {
        t.* = std.Thread.spawn(.{}, struct {
            fn run(idx: usize, c: *GoodCounter) void {
                for (0..1_000_000) |_| {
                    _ = @atomicRmw(u64, &c.counters[idx].value, .Add, 1, .Monotonic);
                }
            }
        }.run, .{ i, counters }) catch unreachable;
    }

    for (threads) |t| t.join();
}
```

### 5.2 预取

```zig
// 软件预取
pub fn prefetchExample(data: []const u8) u64 {
    var sum: u64 = 0;
    const PREFETCH_DISTANCE = 64;  // 提前 64 字节

    var i: usize = 0;
    while (i < data.len) : (i += 1) {
        // 预取未来数据
        if (i + PREFETCH_DISTANCE < data.len) {
            @prefetch(&data[i + PREFETCH_DISTANCE], .{ .rw = .read, .locality = 3 });
        }

        sum += data[i];
    }

    return sum;
}
```

---

## 六、编译器优化

### 6.1 编译模式

```bash
# 调试模式 (快速编译，安全检查)

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
zig build

# 发布小体积 (ReleaseSmall)
zig build -Doptimize=ReleaseSmall
# - 启用优化
# - 移除调试信息
# - 最小化代码体积

# 发布高性能 (ReleaseFast)
zig build -Doptimize=ReleaseFast
# - 最大优化级别
# - 启用自动向量化
# - 适合性能关键代码

# 发布安全 (ReleaseSafe)
zig build -Doptimize=ReleaseSafe
# - 优化但保留安全检查
# - 平衡性能与安全
```

### 6.2 内联与展开

```zig
// 强制内联
inline fn addInline(a: i32, b: i32) i32 {
    return a + b;
}

// 循环展开提示
pub fn sumUnrolled(data: []const i32) i64 {
    var sum: i64 = 0;
    var i: usize = 0;

    // 手动展开
    while (i + 4 <= data.len) : (i += 4) {
        sum += data[i];
        sum += data[i + 1];
        sum += data[i + 2];
        sum += data[i + 3];
    }

    // 剩余
    while (i < data.len) : (i += 1) {
        sum += data[i];
    }

    return sum;
}

// 使用 comptime 展开
pub fn sumComptimeUnroll(comptime unroll: usize, data: []const i32) i64 {
    var sum: i64 = 0;
    var i: usize = 0;

    while (i + unroll <= data.len) : (i += unroll) {
        comptime var j: usize = 0;
        inline while (j < unroll) : (j += 1) {
            sum += data[i + j];
        }
    }

    while (i < data.len) : (i += 1) {
        sum += data[i];
    }

    return sum;
}
```

### 6.3 链接时优化 (LTO)

```zig
// build.zig
const exe = b.addExecutable(.{
    .name = "app",
    .root_source_file = .{ .path = "src/main.zig" },
    .target = target,
    .optimize = .ReleaseFast,
});

// 启用 LTO
exe.want_lto = true;
```

---

## 七、实战：高性能计算

### 7.1 快速傅里叶变换 (FFT)

```zig
// Cooley-Tukey FFT 算法
const std = @import("std");
const math = std.math;

pub fn fft(
    allocator: std.mem.Allocator,
    input: []const f32,
    output_re: []f32,
    output_im: []f32,
) !void {
    const n = input.len;
    std.debug.assert(@popCount(n) == 1);  // 必须是 2 的幂

    // 位反转重排
    for (0..n) |i| {
        const j = bitReverse(i, @intCast(math.log2(n)));
        output_re[j] = input[i];
        output_im[j] = 0;
    }

    // FFT 主循环
    var stage: usize = 2;
    while (stage <= n) : (stage *= 2) {
        const half_stage = stage / 2;

        var i: usize = 0;
        while (i < n) : (i += stage) {
            var j: usize = 0;
            while (j < half_stage) : (j += 1) {
                const angle = -2 * math.pi * @as(f32, @floatFromInt(j)) / @as(f32, @floatFromInt(stage));
                const twiddle_re = math.cos(angle);
                const twiddle_im = math.sin(angle);

                const even_re = output_re[i + j];
                const even_im = output_im[i + j];
                const odd_re = output_re[i + j + half_stage];
                const odd_im = output_im[i + j + half_stage];

                const temp_re = twiddle_re * odd_re - twiddle_im * odd_im;
                const temp_im = twiddle_re * odd_im + twiddle_im * odd_re;

                output_re[i + j] = even_re + temp_re;
                output_im[i + j] = even_im + temp_im;
                output_re[i + j + half_stage] = even_re - temp_re;
                output_im[i + j + half_stage] = even_im - temp_im;
            }
        }
    }

    _ = allocator;
}

fn bitReverse(x: usize, bits: u6) usize {
    var result: usize = 0;
    var i: u6 = 0;
    while (i < bits) : (i += 1) {
        result = (result << 1) | ((x >> i) & 1);
    }
    return result;
}
```

### 7.2 高性能日志系统

```zig
// 无锁环形缓冲区
pub const LockFreeRingBuffer = struct {
    buffer: []u8,
    head: Atomic(usize),
    tail: Atomic(usize),

    pub fn init(allocator: std.mem.Allocator, size: usize) !LockFreeRingBuffer {
        return .{
            .buffer = try allocator.alloc(u8, size),
            .head = Atomic(usize).init(0),
            .tail = Atomic(usize).init(0),
        };
    }

    pub fn write(self: *LockFreeRingBuffer, data: []const u8) bool {
        const head = self.head.load(.Acquire);
        const tail = self.tail.load(.Monotonic);
        const size = self.buffer.len;

        const available = if (head >= tail)
            size - (head - tail)
        else
            tail - head - 1;

        if (data.len > available - 1) return false;

        // 写入数据
        for (data, 0..) |byte, i| {
            self.buffer[(head + i) % size] = byte;
        }

        self.head.store((head + data.len) % size, .Release);
        return true;
    }

    pub fn read(self: *LockFreeRingBuffer, out: []u8) usize {
        const head = self.head.load(.Monotonic);
        const tail = self.tail.load(.Acquire);
        const size = self.buffer.len;

        const available = if (head >= tail)
            head - tail
        else
            size - tail + head;

        const to_read = @min(out.len, available);

        for (0..to_read) |i| {
            out[i] = self.buffer[(tail + i) % size];
        }

        self.tail.store((tail + to_read) % size, .Release);
        return to_read;
    }
};

const Atomic = std.atomic.Atomic;
const std = @import("std");
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
> - 性能提示: 始终先分析，再优化


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

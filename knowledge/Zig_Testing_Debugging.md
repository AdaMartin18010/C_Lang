# Zig 测试与调试：确保代码质量

> **定位**: Zig 核心 / 测试调试 | **难度**: ⭐⭐⭐ | **目标**: 掌握 Zig 测试框架和调试技术

---

## 📋 目录

- [Zig 测试与调试：确保代码质量](#zig-测试与调试确保代码质量)
  - [📋 目录](#-目录)
  - [一、测试框架概述](#一测试框架概述)
  - [二、单元测试](#二单元测试)
    - [2.1 基本测试](#21-基本测试)
    - [2.2 常用断言](#22-常用断言)
    - [2.3 内存分配测试](#23-内存分配测试)
    - [2.4 模拟和桩](#24-模拟和桩)
  - [三、集成测试](#三集成测试)
    - [3.1 测试目录结构](#31-测试目录结构)
    - [3.2 集成测试示例](#32-集成测试示例)
    - [3.3 临时目录测试](#33-临时目录测试)
  - [四、调试技巧](#四调试技巧)
    - [4.1 断点和检查](#41-断点和检查)
    - [4.2 堆栈跟踪](#42-堆栈跟踪)
    - [4.3 日志级别](#43-日志级别)
  - [五、性能分析](#五性能分析)
    - [5.1 基准测试](#51-基准测试)
    - [5.2 性能计时器](#52-性能计时器)
  - [六、模糊测试](#六模糊测试)
  - [七、CI/CD 集成](#七cicd-集成)
    - [7.1 GitHub Actions](#71-github-actions)
    - [7.2 build.zig 测试配置](#72-buildzig-测试配置)

---

## 一、测试框架概述

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Zig 测试生态系统                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  内置测试框架                                                        │
│  ├── std.testing                     - 断言和帮助函数                │
│  ├── zig test                        - 运行测试                      │
│  └── @import("builtin").is_test      - 测试模式检测                  │
│                                                                      │
│  测试类型                                                            │
│  ├── 单元测试                        - 测试单个函数/模块             │
│  ├── 集成测试                        - 测试多个模块协作              │
│  ├── 文档测试                        - 代码示例测试                  │
│  └── 模糊测试                        - 自动生成测试用例              │
│                                                                      │
│  调试工具                                                            │
│  ├── @breakpoint()                   - 断点                          │
│  ├── @panic()                        - 可控崩溃                      │
│  ├── std.log                         - 日志                          │
│  └── 堆栈跟踪                        - 崩溃时自动打印                │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 二、单元测试

### 2.1 基本测试

```zig
// math.zig

const std = @import("std");
const testing = std.testing;

// 被测试的函数
pub fn add(a: i32, b: i32) i32 {
    return a + b;
}

pub fn factorial(n: u32) !u64 {
    if (n > 20) return error.Overflow;
    if (n <= 1) return 1;
    return n * try factorial(n - 1);
}

// 测试块
test "add function" {
    try testing.expectEqual(@as(i32, 5), add(2, 3));
    try testing.expectEqual(@as(i32, 0), add(-1, 1));
    try testing.expectEqual(@as(i32, -5), add(-2, -3));
}

test "factorial function" {
    try testing.expectEqual(@as(u64, 1), try factorial(0));
    try testing.expectEqual(@as(u64, 1), try factorial(1));
    try testing.expectEqual(@as(u64, 120), try factorial(5));
    try testing.expectEqual(@as(u64, 2432902008176640000), try factorial(20));

    // 测试错误情况
    try testing.expectError(error.Overflow, factorial(21));
}
```

### 2.2 常用断言

```zig
const testing = std.testing;

// 相等性测试
try testing.expectEqual(expected, actual);
try testing.expectEqualStrings("hello", actual_string);
try testing.expectEqualSlices(u8, &expected_array, actual_slice);

// 布尔测试
try testing.expect(condition);
try testing.expect(!condition);

// 错误测试
try testing.expectError(expected_error, actual_result);

// 指针测试
try testing.expectEqualPtr(expected_ptr, actual_ptr);
try testing.expect(null_pointer == null);

// 浮点数测试 (允许误差)
try testing.expectApproxEqAbs(expected: f64, actual: f64, tolerance: f64);
try testing.expectApproxEqRel(expected: f64, actual: f64, tolerance: f64);

// 自定义错误信息
try testing.expectEqual(@as(i32, 5), add(2, 3));
```

### 2.3 内存分配测试

```zig
const std = @import("std");
const testing = std.testing;

fn processData(allocator: std.mem.Allocator, input: []const u8) ![]u8 {
    const output = try allocator.alloc(u8, input.len);
    for (input, 0..) |c, i| {
        output[i] = c + 1;  // Caesar cipher
    }
    return output;
}

test "processData allocates and processes" {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer testing.expect(!gpa.detectLeaks());  // 检查内存泄漏

    const allocator = gpa.allocator();
    const input = "hello";

    const result = try processData(allocator, input);
    defer allocator.free(result);

    try testing.expectEqualStrings("ifmmp", result);
}

// 使用测试分配器
test "with test allocator" {
    var test_allocator = testing.allocator;

    const data = try test_allocator.alloc(u8, 100);
    defer test_allocator.free(data);

    // 测试分配器会自动检测泄漏
}

// 固定缓冲区分配器测试
test "with fixed buffer allocator" {
    var buffer: [1024]u8 = undefined;
    var fba = std.heap.FixedBufferAllocator.init(&buffer);
    const allocator = fba.allocator();

    const data = try allocator.alloc(u8, 100);
    allocator.free(data);

    // 验证没有缓冲区溢出
    try testing.expect(fba.end_index <= buffer.len);
}
```

### 2.4 模拟和桩

```zig
// 依赖注入用于测试

const Database = struct {
    queryFn: *const fn ([]const u8) anyerror![]const u8,

    pub fn query(self: Database, sql: []const u8) ![]const u8 {
        return self.queryFn(sql);
    }
};

fn realQuery(sql: []const u8) ![]const u8 {
    // 真实数据库查询
    _ = sql;
    return "real data";
}

fn mockQuery(sql: []const u8) ![]const u8 {
    // 模拟查询
    _ = sql;
    return "mock data";
}

test "with mock database" {
    const mock_db = Database{ .queryFn = mockQuery };
    const result = try mock_db.query("SELECT * FROM users");
    try testing.expectEqualStrings("mock data", result);
}
```

---

## 三、集成测试

### 3.1 测试目录结构

```
myproject/
├── build.zig
├── src/
│   ├── main.zig
│   ├── parser.zig
│   └── utils.zig
└── tests/
    ├── integration_tests.zig    # 集成测试
    └── fixtures/                # 测试数据
        ├── input1.txt
        └── expected1.txt
```

### 3.2 集成测试示例

```zig
// tests/integration_tests.zig

const std = @import("std");
const testing = std.testing;
const parser = @import("../src/parser.zig");

test "parse complete file" {
    const allocator = testing.allocator;

    // 读取测试文件
    const input = try std.fs.cwd().readFileAlloc(allocator, "tests/fixtures/input1.txt", 1024 * 1024);
    defer allocator.free(input);

    // 解析
    const result = try parser.parse(allocator, input);
    defer result.deinit();

    // 验证结果
    const expected = try std.fs.cwd().readFileAlloc(allocator, "tests/fixtures/expected1.txt", 1024 * 1024);
    defer allocator.free(expected);

    try testing.expectEqualStrings(expected, result.toString());
}

test "error handling" {
    const allocator = testing.allocator;

    const invalid_input = "invalid { json";

    // 应该返回错误
    const result = parser.parse(allocator, invalid_input);
    try testing.expectError(error.ParseError, result);
}
```

### 3.3 临时目录测试

```zig
test "file operations" {
    const allocator = testing.allocator;

    // 创建临时目录
    var tmp_dir = testing.tmpDir(.{});
    defer tmp_dir.cleanup();

    // 在临时目录中操作
    const file = try tmp_dir.dir.createFile("test.txt", .{});
    defer file.close();

    try file.writeAll("hello world");

    // 读取验证
    const content = try tmp_dir.dir.readFileAlloc(allocator, "test.txt", 1024);
    defer allocator.free(content);

    try testing.expectEqualStrings("hello world", content);
}
```

---

## 四、调试技巧

### 4.1 断点和检查

```zig
const std = @import("std");

fn debugMe(value: i32) i32 {
    // 编译时断点
    @breakpoint();

    // 条件断点
    if (value < 0) {
        @breakpoint();
    }

    // 使用日志
    std.log.debug("debugMe called with value: {}", .{value});

    return value * 2;
}

// 编译时断言
fn compileTimeCheck(comptime T: type) void {
    comptime {
        if (@sizeOf(T) > 1024) {
            @compileError("Type too large: " ++ @typeName(T));
        }
    }
}

// 运行时断言
fn runtimeCheck(value: i32) i32 {
    std.debug.assert(value >= 0);  // debug 模式检查

    if (value < 0) {
        @panic("Value must be non-negative");  // 总是检查
    }

    return value;
}
```

### 4.2 堆栈跟踪

```zig
// 获取堆栈跟踪
fn printStackTrace() void {
    const trace = @errorReturnTrace();
    if (trace) |t| {
        std.debug.print("Stack trace:\n", .{});
        for (t.instruction_addresses) |addr| {
            std.debug.print("  {x}\n", .{addr});
        }
    }
}

// 自定义 panic 处理器
pub fn panic(msg: []const u8, error_return_trace: ?*std.builtin.StackTrace, ret_addr: ?usize) noreturn {
    std.debug.print("PANIC: {s}\n", .{msg});

    if (error_return_trace) |trace| {
        std.debug.dumpStackTrace(trace.*);
    }

    _ = ret_addr;

    std.process.exit(1);
}
```

### 4.3 日志级别

```zig
const std = @import("std");

// 设置日志级别
pub const std_options = .{
    .log_level = .debug,  // .err, .warn, .info, .debug
};

fn doSomething() void {
    std.log.err("This is an error: {}", .{42});
    std.log.warn("This is a warning");
    std.log.info("This is info");
    std.log.debug("This is debug: {s}", .{"details"});
}

// 作用域日志
const my_log = std.log.scoped(.my_module);

fn scopedLogging() void {
    my_log.info("Module-specific log");
}
```

---

## 五、性能分析

### 5.1 基准测试

```zig
// benchmark.zig

const std = @import("std");
const testing = std.testing;

fn benchmark(name: []const u8, func: fn () void, iterations: usize) !void {
    const start = std.time.nanoTimestamp();

    var i: usize = 0;
    while (i < iterations) : (i += 1) {
        func();
    }

    const end = std.time.nanoTimestamp();
    const elapsed = @as(f64, @floatFromInt(end - start)) / std.time.ns_per_ms;

    std.debug.print("{s}: {d:.2} ms for {} iterations\n", .{
        name, elapsed, iterations
    });
}

fn functionToBenchmark() void {
    var sum: u64 = 0;
    var i: u32 = 0;
    while (i < 10000) : (i += 1) {
        sum += i;
    }
    std.mem.doNotOptimizeAway(sum);
}

test "benchmark example" {
    try benchmark("functionToBenchmark", functionToBenchmark, 1000);
}
```

### 5.2 性能计时器

```zig
const Timer = struct {
    name: []const u8,
    start: i128,

    pub fn begin(name: []const u8) Timer {
        return .{
            .name = name,
            .start = std.time.nanoTimestamp(),
        };
    }

    pub fn end(self: Timer) void {
        const elapsed = std.time.nanoTimestamp() - self.start;
        const ms = @as(f64, @floatFromInt(elapsed)) / std.time.ns_per_ms;
        std.debug.print("[{}] took {d:.3} ms\n", .{ self.name, ms });
    }
};

// 使用
fn measuredFunction() void {
    const timer = Timer.begin("operation");
    defer timer.end();

    // 执行操作
    std.time.sleep(10 * std.time.ns_per_ms);
}
```

---

## 六、模糊测试

```zig
// fuzz_test.zig

const std = @import("std");
const testing = std.testing;

// 被测试的解析函数
fn parseNumber(input: []const u8) !i32 {
    if (input.len == 0) return error.EmptyInput;

    var result: i32 = 0;
    var negative = false;
    var i: usize = 0;

    if (input[0] == '-') {
        negative = true;
        i += 1;
    }

    while (i < input.len) : (i += 1) {
        const c = input[i];
        if (c < '0' or c > '9') return error.InvalidCharacter;

        const digit = c - '0';

        // 检查溢出
        if (result > (@import("std").math.maxInt(i32) - digit) / 10) {
            return error.Overflow;
        }

        result = result * 10 + digit;
    }

    return if (negative) -result else result;
}

// 模糊测试入口
export fn zig_fuzz_init() void {}

export fn zig_fuzz_test(data: [*]const u8, len: usize) void {
    const input = data[0..len];

    // 尝试解析 - 不应崩溃
    _ = parseNumber(input) catch {};
}

// 手动模糊测试
test "fuzz with random inputs" {
    var prng = std.rand.DefaultPrng.init(0);
    const random = prng.random();

    var i: usize = 0;
    while (i < 10000) : (i += 1) {
        const len = random.uintLessThan(usize, 20);
        var input: [20]u8 = undefined;

        random.bytes(input[0..len]);

        // 不应崩溃
        _ = parseNumber(input[0..len]) catch {};
    }
}
```

---

## 七、CI/CD 集成

### 7.1 GitHub Actions

```yaml
# .github/workflows/ci.yml
name: CI

on: [push, pull_request]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        zig-version: [0.11.0, master]

    steps:
    - uses: actions/checkout@v3

    - name: Setup Zig
      uses: goto-bus-stop/setup-zig@v2
      with:
        version: ${{ matrix.zig-version }}

    - name: Run tests
      run: zig build test

    - name: Run integration tests
      run: zig test tests/integration_tests.zig

    - name: Check formatting
      run: zig fmt --check src/

    - name: Build release
      run: zig build -Doptimize=ReleaseFast
```

### 7.2 build.zig 测试配置

```zig
// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    // ... 其他配置

    // 单元测试
    const unit_tests = b.addTest(.{
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_unit_tests = b.addRunArtifact(unit_tests);

    // 集成测试
    const integration_tests = b.addTest(.{
        .root_source_file = .{ .path = "tests/integration_tests.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_integration_tests = b.addRunArtifact(integration_tests);

    // 测试步骤
    const test_step = b.step("test", "Run all tests");
    test_step.dependOn(&run_unit_tests.step);
    test_step.dependOn(&run_integration_tests.step);

    // 覆盖率测试 (使用 kcov)
    const coverage_step = b.step("coverage", "Generate coverage report");
    const kcov = b.addSystemCommand(&.{
        "kcov", "--clean",
        "--include-path=src",
        "coverage/",
    });
    kcov.addArtifactArg(unit_tests);
    coverage_step.dependOn(&kcov.step);
}
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10

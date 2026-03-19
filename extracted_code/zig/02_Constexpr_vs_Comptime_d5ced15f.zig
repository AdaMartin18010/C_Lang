//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 1396
// Language: zig
// Block ID: d5ced15f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 编译时错误处理
const std = @import("std");

// ========== @compileError ==========

// 基本编译时错误
fn requirePowerOfTwo(comptime n: usize) void {
    if (n & (n - 1) != 0) {
        @compileError("Value must be power of two, got " ++ @as([]const u8, @typeName(@TypeOf(n))));
    }
}

// 使用
comptime requirePowerOfTwo(1024);  // OK
// comptime requirePowerOfTwo(1000);  // 编译错误：详细错误信息

// ========== 编译时断言 ==========

// 自定义编译时断言
fn comptimeAssert(comptime condition: bool, comptime msg: []const u8) void {
    if (!condition) {
        @compileError(msg);
    }
}

comptimeAssert(@sizeOf(usize) == 8, "64-bit platform required");
comptimeAssert(@alignOf(u128) >= 8, "u128 must be 8-byte aligned");

// ========== 类型检查错误 ==========

// 函数参数类型检查
fn sortNumbers(comptime T: type, arr: []T) void {
    comptime {
        const info = @typeInfo(T);
        if (info != .Int and info != .Float) {
            @compileError("sortNumbers requires numeric type, got " ++ @typeName(T));
        }
    }
    // 排序实现...
    _ = arr;
}

// sortNumbers([]u8, &[_]u8{'a', 'b'});  // 编译错误：类型不匹配

// ========== 编译时日志 ==========

// @compileLog 用于调试编译时计算
fn debugComptime(comptime T: type) void {
    comptime {
        @compileLog("Type:", T);
        @compileLog("Size:", @sizeOf(T));
        @compileLog("Alignment:", @alignOf(T));
    }
}

// debugComptime(i32);  // 会在编译输出中打印信息

// ========== 条件编译错误 ==========

// 根据目标平台产生错误
fn platformSpecificFunction() void {
    if (@import("builtin").target.os.tag == .windows) {
        // Windows 实现
    } else if (@import("builtin").target.os.tag == .linux) {
        // Linux 实现
    } else {
        @compileError("Unsupported operating system");
    }
}

// ========== 泛型约束错误 ==========

// 接口检查
fn requireInterface(comptime T: type, comptime method: []const u8) void {
    comptime {
        if (!@hasDecl(T, method)) {
            @compileError(@typeName(T) ++ " must implement " ++ method);
        }
    }
}

const MyStruct = struct {
    pub fn process(self: MyStruct) void {
        _ = self;
    }
};

fn useProcessor(comptime T: type, obj: T) void {
    requireInterface(T, "process");
    obj.process();
}

// useProcessor(i32, 42);  // 编译错误：i32 没有 process 方法

// ========== 运行时与编译时错误区分 ==========

// 编译时检查参数
fn createBuffer(comptime size: usize) [size]u8 {
    comptime {
        if (size == 0) {
            @compileError("Buffer size cannot be zero");
        }
        if (size > 1024 * 1024) {
            @compileError("Buffer size too large for stack allocation");
        }
    }
    return undefined;
}

// 运行时检查（保留错误处理）
fn parseNumber(str: []const u8) !i32 {
    if (str.len == 0) {
        return error.EmptyString;
    }
    // 解析逻辑...
    return 42;
}

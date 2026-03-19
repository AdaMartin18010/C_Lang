//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 1279
// Language: zig
// Block ID: 7db21c0b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 的弱类型 → Zig 的强类型

const std = @import("std");

// C 代码的问题
/*
typedef int Handle;  // 只是 int 的别名
void close(Handle h);
// 可以传入任何 int，包括错误的值

void process(void* data);  // 丢失了类型信息
// 需要手动转换，容易出错

enum Color { RED, GREEN, BLUE };
// RED 在全局命名空间，可能冲突
*/

// Zig 解决方案

/// 不透明句柄类型
pub const FileHandle = enum(i32) {
    invalid = -1,
    _,  // 其他值有效

    pub fn close(self: FileHandle) void {
        if (self == .invalid) return;
        // 关闭逻辑
    }
};

/// 类型化的资源包装
pub fn Resource(comptime T: type) type {
    return struct {
        const Self = @This();

        ptr: *T,

        pub fn init(ptr: *T) Self {
            return .{ .ptr = ptr };
        }

        pub fn get(self: Self) *T {
            return self.ptr;
        }
    };
}

/// 命名空间枚举
pub const Color = enum {
    red,
    green,
    blue,

    // 枚举关联方法
    pub fn isPrimary(self: Color) bool {
        return self == .red or self == .blue;
    }
};

// 使用
const c = Color.red;  // 必须加命名空间
const primary = c.isPrimary();

/// 单位类型（零开销抽象）
pub const Bytes = struct {
    value: u64,

    pub fn fromBytes(n: u64) Bytes {
        return .{ .value = n };
    }

    pub fn fromKB(n: u64) Bytes {
        return .{ .value = n * 1024 };
    }

    pub fn fromMB(n: u64) Bytes {
        return .{ .value = n * 1024 * 1024 };
    }

    pub fn toKB(self: Bytes) u64 {
        return self.value / 1024;
    }
};

// 防止混淆
const size1 = Bytes.fromMB(1);
const size2 = Bytes.fromKB(1024);
// size1 和 size2 相等，但语义清晰

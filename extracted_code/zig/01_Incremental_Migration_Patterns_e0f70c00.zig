//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 1027
// Language: zig
// Block ID: e0f70c00
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 回调模式 → Zig 函数指针/闭包

const std = @import("std");

// C 原始代码
/*
typedef void (*progress_cb)(int percent, void* user_data);

typedef void (*data_cb)(const char* data, size_t len, void* user_data);

typedef struct {
    progress_cb on_progress;
    data_cb on_data;
    void* user_data;
} Callbacks;

void process_with_callbacks(const char* input, Callbacks* cbs);
*/

// Zig 转换 - 使用泛型和 comptime

/// 类型安全的回调接口
pub fn Processor(comptime Context: type) type {
    return struct {
        const Self = @This();

        pub const ProgressFn = *const fn (
            ctx: Context,
            percent: u8,
        ) void;

        pub const DataFn = *const fn (
            ctx: Context,
            data: []const u8,
        ) void;

        context: Context,
        on_progress: ?ProgressFn,
        on_data: ?DataFn,

        pub fn init(context: Context) Self {
            return .{
                .context = context,
                .on_progress = null,
                .on_data = null,
            };
        }

        pub fn process(self: *Self, input: []const u8) !void {
            const chunk_size = input.len / 10;

            var i: usize = 0;
            while (i < input.len) : (i += chunk_size) {
                const end = @min(i + chunk_size, input.len);
                const chunk = input[i..end];

                // 报告进度
                if (self.on_progress) |cb| {
                    const percent = @as(u8, @intCast(i * 100 / input.len));
                    cb(self.context, percent);
                }

                // 报告数据
                if (self.on_data) |cb| {
                    cb(self.context, chunk);
                }
            }

            // 最终进度
            if (self.on_progress) |cb| {
                cb(self.context, 100);
            }
        }
    };
}

// C 兼容版本（使用不透明指针）
const CCallbacks = extern struct {
    on_progress: ?*const fn (percent: c_int, user_data: ?*anyopaque) callconv(.C) void,
    on_data: ?*const fn (data: ?[*]const u8, len: usize, user_data: ?*anyopaque) callconv(.C) void,
    user_data: ?*anyopaque,
};

export fn process_with_callbacks(
    input: ?[*]const u8,
    input_len: usize,
    cbs: ?*const CCallbacks,
) c_int {
    const in_slice = input.?[0..input_len];
    const callbacks = cbs.?;

    const chunk_size = input_len / 10;
    var i: usize = 0;

    while (i < input_len) : (i += chunk_size) {
        const end = @min(i + chunk_size, input_len);
        const chunk = in_slice[i..end];

        if (callbacks.on_progress) |cb| {
            const percent = @as(c_int, @intCast(i * 100 / input_len));
            cb(percent, callbacks.user_data);
        }

        if (callbacks.on_data) |cb| {
            cb(chunk.ptr, chunk.len, callbacks.user_data);
        }
    }

    if (callbacks.on_progress) |cb| {
        cb(100, callbacks.user_data);
    }

    return 0;
}

//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 459
// Language: zig
// Block ID: 7820298e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// C 回调系统到 Zig 的转换
/// typedef void (*progress_cb)(int percent, void* user_data);
/// void download_async(const char* url, progress_cb cb, void* user_data);

// Zig 版本使用更有类型的方式
pub fn DownloadCallback(comptime UserData: type) type {
    return struct {
        ctx: *UserData,
        onProgress: *const fn (*UserData, percent: u8) void,
    };
}

pub fn downloadAsync(
    url: []const u8,
    comptime UserData: type,
    callback: DownloadCallback(UserData),
) !void {
    // 内部适配层将 C 回调转换为 Zig 调用
    const Adapter = struct {
        fn cCallback(percent: c_int, user_data: ?*anyopaque) callconv(.C) void {
            const cb: *DownloadCallback(UserData) = @ptrCast(@alignCast(user_data));
            cb.onProgress(cb.ctx, @intCast(percent));
        }
    };

    const c_url = try std.heap.c_allocator.dupeZ(u8, url);
    defer std.heap.c_allocator.free(c_url);

    c.download_async(
        c_url.ptr,
        Adapter.cCallback,
        &callback
    );
}

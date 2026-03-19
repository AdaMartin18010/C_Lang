//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 1830
// Language: zig
// Block ID: 046b9340
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 安全实践
const std = @import("std");

// 实践 1：优先使用切片而非指针
fn safe_slices() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const slice = try allocator.alloc(i32, 100);
    defer allocator.free(slice);

    // 切片自动边界检查
    slice[50] = 42;  // 安全

    // 子切片也安全
    const sub = slice[10..20];
    sub[5] = 100;  // 实际是 slice[15]
}

// 实践 2：使用可选指针处理可能为空
fn safe_optional() void {
    var maybe_ptr: ?*i32 = null;

    // 安全解包
    if (maybe_ptr) |ptr| {
        ptr.* = 42;
    } else {
        // 处理 null 情况
    }

    // 或使用 orelse
    const ptr = maybe_ptr orelse return;
    ptr.* = 42;
}

// 实践 3：使用类型安全的转换
fn safe_conversion() void {
    var bytes: [4]u8 = .{ 0, 0, 0x80, 0x41 };

    // 方式 1：bytesToValue
    const f = std.mem.bytesToValue(f32, &bytes);
    _ = f;

    // 方式 2：copy
    var f2: f32 = undefined;
    @memcpy(std.mem.asBytes(&f2), &bytes);
}

// 实践 4：使用 Arena 或固定分配器
fn safe_allocation() !void {
    // 方式 1：Arena（统一生命周期）
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();

    const ptr = try arena.allocator().create(i32);
    ptr.* = 42;
    // 无需单独释放

    // 方式 2：固定缓冲区
    var buffer: [1024]u8 = undefined;
    var fba = std.heap.FixedBufferAllocator.init(&buffer);
    const slice = try fba.allocator().alloc(i32, 10);
    _ = slice;
}

// 实践 5：使用 @setRuntimeSafety
fn safe_runtime_check(comptime enable: bool) void {
    @setRuntimeSafety(enable);

    var arr: [10]i32 = undefined;
    // 如果 enable=true，以下会触发边界检查 panic
    // arr[100] = 42;
}

// 实践 6：使用 comptime 验证
fn safe_comptime_check(comptime T: type, ptr: *T) void {
    comptime {
        // 编译时验证类型大小
        if (@sizeOf(T) > 64) {
            @compileError("类型过大");
        }
    }
    ptr.* = std.mem.zeroes(T);
}

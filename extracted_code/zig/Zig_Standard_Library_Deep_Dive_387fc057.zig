//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 123
// Language: zig
// Block ID: 387fc057
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// ============================================
// GeneralPurposeAllocator (GPA) - 最常用
// ============================================
fn gpa_example() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{
        // 配置选项
        .safety = true,           // 启用安全检查
        .verbose_log = false,     // 关闭详细日志
        .enable_memory_limit = true,  // 启用内存限制
        .memory_limit = 100 * 1024 * 1024,  // 100MB限制
    }){};
    defer {
        const status = gpa.deinit();
        if (status == .leak) {
            std.log.err("Memory leak detected!", .{});
        }
    }

    const allocator = gpa.allocator();

    // 基本分配
    const buf = try allocator.alloc(u8, 1024);
    defer allocator.free(buf);

    // 带对齐的分配
    const aligned = try allocator.alignedAlloc(u8, 16, 256);
    defer allocator.free(aligned);

    // 重新分配
    var dynamic = try allocator.alloc(u8, 100);
    defer allocator.free(dynamic);

    dynamic = try allocator.realloc(dynamic, 200);

    // 分配单个对象
    const node = try allocator.create(Node);
    defer allocator.destroy(node);

    node.* = .{ .value = 42, .next = null };
}

const Node = struct {
    value: i32,
    next: ?*Node,
};

// ============================================
// ArenaAllocator - 高性能批量分配
// ============================================
fn arena_example() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();  // 一次性释放所有内存

    const allocator = arena.allocator();

    // 在arena中分配大量临时对象
    for (0..1000) |i| {
        const str = try std.fmt.allocPrint(allocator, "Item {}", .{i});
        // 不需要单独释放！
        _ = str;
    }

    // 重置arena但不释放内存，可重用
    _ = arena.reset(.retain_capacity);
}

// ============================================
// FixedBufferAllocator - 无堆分配
// ============================================
fn fixed_buffer_example() void {
    // 栈上缓冲区
    var buffer: [4096]u8 = undefined;
    var fba = std.heap.FixedBufferAllocator.init(&buffer);
    const allocator = fba.allocator();

    // 所有分配在栈上
    const arr = allocator.alloc(u8, 100) catch {
        std.log.err("Out of stack memory!", .{});
        return;
    };
    _ = arr;

    // 检查剩余空间
    const remaining = fba.buffer.len - fba.end_index;
    std.log.info("Remaining: {} bytes", .{remaining});
}

// ============================================
// StackFallbackAllocator - 栈优先
// ============================================
fn stack_fallback_example() !void {
    var stack_buffer: [1024]u8 = undefined;
    var stack_fallback = std.heap.stackFallback(1024, std.heap.page_allocator);
    const allocator = stack_fallback.get();

    // 小分配在栈上，大分配转到后备分配器
    const small = try allocator.alloc(u8, 100);   // 栈上
    defer allocator.free(small);

    const large = try allocator.alloc(u8, 10000); // 堆上
    defer allocator.free(large);
}

// ============================================
// LoggingAllocator - 调试内存使用
// ============================================
fn logging_example() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();

    var logging = std.heap.loggingAllocator(gpa.allocator());
    const allocator = logging.allocator();

    // 所有分配/释放都会打印日志
    const buf = try allocator.alloc(u8, 100);
    defer allocator.free(buf);
    // 输出: [ALLOC] 100 bytes at 0x... -> 0x...
    //       [FREE]  0x...
}

// ============================================
// 自定义分配器：边界检查分配器
// ============================================
pub const GuardAllocator = struct {
    child_allocator: std.mem.Allocator,
    guard_size: usize,

    const Guard = struct {
        const MAGIC = 0xDEADBEEF;
        magic: usize,
        size: usize,
    };

    pub fn allocator(self: *GuardAllocator) std.mem.Allocator {
        return .{
            .ptr = self,
            .vtable = &.{
                .alloc = alloc,
                .resize = resize,
                .free = free,
            },
        };
    }

    fn alloc(ctx: *anyopaque, len: usize, ptr_align: u8, ret_addr: usize) ?[*]u8 {
        const self = @ptrCast(*GuardAllocator, @alignCast(ctx));
        const total_size = @sizeOf(Guard) + len + self.guard_size;

        const mem = self.child_allocator.rawAlloc(total_size, ptr_align, ret_addr) orelse return null;

        // 写入头部保护
        const guard = @ptrCast(*Guard, mem);
        guard.magic = Guard.MAGIC;
        guard.size = len;

        // 填充尾部保护
        const tail_start = @sizeOf(Guard) + len;
        @memset(mem[tail_start..total_size], 0xAA);

        return mem + @sizeOf(Guard);
    }

    fn checkGuard(ptr: [*]u8) void {
        const guard = @ptrCast(*Guard, ptr - @sizeOf(Guard));
        std.debug.assert(guard.magic == Guard.MAGIC);
        // 检查尾部...
    }

    fn resize(ctx: *anyopaque, buf: []u8, buf_align: u8, new_len: usize, ret_addr: usize) bool {
        _ = ctx; _ = buf; _ = buf_align; _ = new_len; _ = ret_addr;
        // 简化实现
        return false;
    }

    fn free(ctx: *anyopaque, buf: []u8, buf_align: u8, ret_addr: usize) void {
        const self = @ptrCast(*GuardAllocator, @alignCast(ctx));
        checkGuard(buf.ptr);

        const guard = @ptrCast(*Guard, buf.ptr - @sizeOf(Guard));
        const total_size = @sizeOf(Guard) + guard.size;

        self.child_allocator.rawFree(buf.ptr[0..total_size], buf_align, ret_addr);
    }
};

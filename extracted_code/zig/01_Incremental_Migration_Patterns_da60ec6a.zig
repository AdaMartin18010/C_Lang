//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 1207
// Language: zig
// Block ID: da60ec6a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 缓冲区操作 → Zig 切片边界检查

const std = @import("std");

// C 代码的问题
/*
void copy_data(char* dest, const char* src, size_t len) {
    // 危险：没有边界检查
    memcpy(dest, src, len);
    // 如果 dest 太小，发生溢出
}

void parse_header(const uint8_t* data, size_t len) {
    uint32_t magic = *(uint32_t*)data;  // 未对齐访问 + 可能越界
    uint16_t version = *(uint16_t*)(data + 4);
    // ...
}
*/

// Zig 安全版本
pub fn copyData(dest: []u8, src: []const u8) !void {
    if (dest.len < src.len) {
        return error.DestTooSmall;
    }
    @memcpy(dest[0..src.len], src);
}

pub const Header = extern struct {
    magic: u32,
    version: u16,
    flags: u16,

    pub fn decode(data: []const u8) !Header {
        if (data.len < @sizeOf(Header)) {
            return error.TooSmall;
        }

        // 安全地读取（处理对齐）
        return .{
            .magic = std.mem.readInt(u32, data[0..4], .little),
            .version = std.mem.readInt(u16, data[4..6], .little),
            .flags = std.mem.readInt(u16, data[6..8], .little),
        };
    }
};

// 运行时边界检查示例
pub fn safeAccess(arr: []i32, index: usize) !i32 {
    // Zig 会自动进行边界检查
    // 也可以通过 comptime 选择性禁用
    return arr[index];  // 如果越界，触发 panic 或返回错误
}

// 使用切片限制范围
pub fn processChunk(data: []const u8, offset: usize, len: usize) !void {
    // 确保不越界
    const end = try std.math.add(usize, offset, len);
    if (end > data.len) return error.OutOfBounds;

    const chunk = data[offset..end];
    // 现在可以安全处理 chunk
    process(chunk);
}

fn process(data: []const u8) void {
    _ = data;
}

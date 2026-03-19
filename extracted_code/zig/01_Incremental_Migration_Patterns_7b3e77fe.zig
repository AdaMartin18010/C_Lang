//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 930
// Language: zig
// Block ID: 7b3e77fe
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 字符串处理 → Zig 切片

const std = @import("std");

// C 原始代码
/*
char* string_concat(const char* a, const char* b) {
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    char* result = malloc(len_a + len_b + 1);
    if (!result) return NULL;
    memcpy(result, a, len_a);
    memcpy(result + len_a, b, len_b + 1);
    return result;
}

char* string_substr(const char* s, size_t start, size_t len) {
    char* result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, s + start, len);
    result[len] = '\0';
    return result;
}

int string_find(const char* haystack, const char* needle) {
    const char* found = strstr(haystack, needle);
    if (!found) return -1;
    return (int)(found - haystack);
}
*/

// Zig 转换代码

/// 安全的字符串连接
pub fn concat(
    allocator: std.mem.Allocator,
    a: []const u8,
    b: []const u8,
) ![]u8 {
    const result = try allocator.alloc(u8, a.len + b.len);
    @memcpy(result[0..a.len], a);
    @memcpy(result[a.len..], b);
    return result;
}

/// 安全的子串提取
pub fn substr(
    allocator: std.mem.Allocator,
    s: []const u8,
    start: usize,
    len: usize,
) ![]u8 {
    if (start > s.len) return error.InvalidRange;
    const actual_len = @min(len, s.len - start);
    const result = try allocator.alloc(u8, actual_len);
    @memcpy(result, s[start..start + actual_len]);
    return result;
}

/// 安全的字符串查找
pub fn find(haystack: []const u8, needle: []const u8) ?usize {
    if (needle.len == 0) return 0;
    if (needle.len > haystack.len) return null;

    var i: usize = 0;
    while (i <= haystack.len - needle.len) : (i += 1) {
        if (std.mem.eql(u8, haystack[i..i + needle.len], needle)) {
            return i;
        }
    }
    return null;
}

// C 兼容接口
export fn string_concat(
    a: [*:0]const u8,
    b: [*:0]const u8,
) ?[*:0]u8 {
    const len_a = std.mem.len(a);
    const len_b = std.mem.len(b);
    const result = std.heap.c_allocator.allocSentinel(u8, len_a + len_b, 0) catch return null;
    @memcpy(result[0..len_a], a[0..len_a]);
    @memcpy(result[len_a..len_a + len_b], b[0..len_b]);
    return result.ptr;
}

export fn string_free(s: ?[*:0]u8) void {
    if (s) |ptr| {
        const len = std.mem.len(ptr);
        std.heap.c_allocator.free(ptr[0..len + 1]);
    }
}

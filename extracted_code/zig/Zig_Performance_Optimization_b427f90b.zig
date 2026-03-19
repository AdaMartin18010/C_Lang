//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 413
// Language: zig
// Block ID: b427f90b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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

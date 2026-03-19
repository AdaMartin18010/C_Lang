//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 219
// Language: zig
// Block ID: 144d153c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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

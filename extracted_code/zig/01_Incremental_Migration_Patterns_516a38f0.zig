//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 1567
// Language: zig
// Block ID: 516a38f0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// SQLite 虚拟表 Zig 实现示例

const std = @import("std");

// C API 导入
const c = @cImport({
    @cInclude("sqlite3ext.h");
});

/// Zig 风格的虚拟表实现
pub const VirtualTable = struct {
    const Self = @This();

    base: c.sqlite3_vtab,
    allocator: std.mem.Allocator,
    data: std.ArrayList(Row),

    pub const Row = struct {
        id: i64,
        name: []const u8,
        value: f64,
    };

    /// 安全的列值提取
    pub fn column(self: *Self, ctx: *c.sqlite3_context, n: c_int) !void {
        const row = self.currentRow orelse return error.NoRow;

        switch (n) {
            0 => c.sqlite3_result_int64(ctx, row.id),
            1 => c.sqlite3_result_text(
                ctx,
                row.name.ptr,
                @intCast(row.name.len),
                c.SQLITE_TRANSIENT,
            ),
            2 => c.sqlite3_result_double(ctx, row.value),
            else => return error.InvalidColumn,
        }
    }

    /// 类型安全的过滤
    pub fn filter(
        self: *Self,
        idx_num: c_int,
        idx_str: ?[*:0]const u8,
        argc: c_int,
        argv: [*c]?*c.sqlite3_value,
    ) !void {
        _ = idx_str;

        // 解析约束条件
        var constraints = try self.parseConstraints(
            idx_num,
            argc,
            argv
        );
        defer constraints.deinit();

        // 应用过滤
        self.filteredData.clearRetainingCapacity();
        for (self.data.items) |row| {
            if (self.matchesConstraints(row, constraints)) {
                try self.filteredData.append(row);
            }
        }

        self.cursor = 0;
    }

    fn parseConstraints(
        self: *Self,
        idx_num: c_int,
        argc: c_int,
        argv: [*c]?*c.sqlite3_value,
    ) !ConstraintList {
        _ = self;
        _ = idx_num;

        var list = ConstraintList.init();
        for (0..@intCast(argc)) |i| {
            const value = argv[i] orelse continue;
            const constraint = try parseValue(value);
            try list.append(constraint);
        }
        return list;
    }

    fn parseValue(val: *c.sqlite3_value) !Constraint {
        const type_id = c.sqlite3_value_type(val);
        return switch (type_id) {
            c.SQLITE_INTEGER => .{
                .int = c.sqlite3_value_int64(val),
            },
            c.SQLITE_FLOAT => .{
                .float = c.sqlite3_value_double(val),
            },
            c.SQLITE_TEXT => blk: {
                const ptr = c.sqlite3_value_text(val);
                const len = c.sqlite3_value_bytes(val);
                break :blk .{
                    .text = ptr.?[0..@intCast(len)],
                };
            },
            else => error.UnsupportedType,
        };
    }
};

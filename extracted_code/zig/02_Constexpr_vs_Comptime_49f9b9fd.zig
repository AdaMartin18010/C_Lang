//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 1988
// Language: zig
// Block ID: 49f9b9fd
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 强大的 DSL 构建能力
const std = @import("std");

// ========== 嵌入式 SQL DSL ==========

// 编译时 SQL 解析和类型检查
fn Query(comptime sql: []const u8) type {
    // 解析 SQL 语句
    comptime {
        // 验证 SQL 语法（简化示例）
        if (!std.mem.startsWith(u8, sql, "SELECT ")) {
            @compileError("Only SELECT queries supported");
        }

        // 解析返回的列
        const columns = parseSelectColumns(sql);

        // 生成结果结构体
        return makeResultStruct(columns);
    }
}

fn parseSelectColumns(comptime sql: []const u8) []const Column {
    // 简化：解析 SELECT 后的列名
    var columns: []const Column = &[_]Column{};
    // ... 解析逻辑
    _ = sql;
    return columns;
}

const Column = struct {
    name: []const u8,
    type: type,
};

fn makeResultStruct(comptime columns: []const Column) type {
    var fields: [columns.len]std.builtin.Type.StructField = undefined;
    for (columns, 0..) |col, i| {
        fields[i] = .{
            .name = col.name,
            .type = col.type,
            .default_value = null,
            .is_comptime = false,
            .alignment = @alignOf(col.type),
        };
    }
    return @Type(.{
        .Struct = .{
            .layout = .auto,
            .fields = &fields,
            .decls = &.{},
            .is_tuple = false,
        },
    });
}

// 使用
// const UserQuery = Query("SELECT id, name, email FROM users");
// var result: UserQuery = .{ .id = 1, .name = "Alice", .email = "alice@example.com" };

// ========== 状态机 DSL ==========

fn StateMachine(comptime states: anytype) type {
    comptime {
        // 验证状态转换表
        const StateEnum = makeStateEnum(states);

        return struct {
            pub const State = StateEnum;
            current: State,

            pub fn transition(self: *@This(), event: []const u8) !void {
                // 根据当前状态和事件查找目标状态
                inline for (states) |trans| {
                    if (self.current == trans.from and
                        std.mem.eql(u8, event, trans.event)) {
                        self.current = trans.to;
                        return;
                    }
                }
                return error.InvalidTransition;
            }
        };
    }
}

fn makeStateEnum(comptime states: anytype) type {
    // 从转换表中提取所有状态
    var state_names: []const []const u8 = &[_][]const u8{};
    inline for (states) |trans| {
        // 去重并收集状态名
        state_names = state_names ++ &[_][]const u8{trans.from};
        state_names = state_names ++ &[_][]const u8{trans.to};
    }
    // 生成枚举...
    return enum { idle, running, stopped };
}

// ========== 构建器模式 DSL ==========

fn Builder(comptime T: type) type {
    const info = @typeInfo(T);
    if (info != .Struct) @compileError("Expected struct");

    var builder_fields: [info.Struct.fields.len]std.builtin.Type.StructField = undefined;
    var build_assignments: []const u8 = "";

    inline for (info.Struct.fields, 0..) |field, i| {
        // 可选类型字段
        builder_fields[i] = .{
            .name = field.name,
            .type = ?field.type,
            .default_value = &@as(?field.type, null),
            .is_comptime = false,
            .alignment = @alignOf(?field.type),
        };
    }

    return @Type(.{
        .Struct = .{
            .layout = .auto,
            .fields = &builder_fields,
            .decls = &.{},
            .is_tuple = false,
        },
    });
}

const Config = struct {
    host: []const u8,
    port: u16,
    timeout: u32,
};

const ConfigBuilder = Builder(Config);

// 使用
// var builder: ConfigBuilder = .{};
// builder.host = "localhost";
// builder.port = 8080;

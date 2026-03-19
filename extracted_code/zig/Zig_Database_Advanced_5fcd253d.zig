//
// Auto-generated from: Zig\Zig_Database_Advanced.md
// Line: 163
// Language: zig
// Block ID: 5fcd253d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 异步数据库操作
pub const AsyncDatabase = struct {
    pool: ConnectionPool,
    thread_pool: std.Thread.Pool,

    pub fn init(allocator: std.mem.Allocator, path: []const u8) !AsyncDatabase {
        var pool = try ConnectionPool.init(allocator, path, 10);

        var thread_pool: std.Thread.Pool = undefined;
        try std.Thread.Pool.init(&thread_pool, .{
            .allocator = allocator,
            .n_jobs = 4,
        });

        return .{
            .pool = pool,
            .thread_pool = thread_pool,
        };
    }

    pub fn queryAsync(
        self: *AsyncDatabase,
        sql: []const u8,
        callback: fn ([]const u8) void,
    ) !void {
        const conn = try self.pool.acquire();
        defer self.pool.release(conn);

        // 在线程池中执行
        try self.thread_pool.spawn(struct {
            fn run(c: *Connection, query: []const u8, cb: fn ([]const u8) void) void {
                executeQuery(c, query, cb);
            }
        }.run, .{ conn, sql, callback });
    }

    fn executeQuery(conn: *Connection, sql: []const u8, callback: fn ([]const u8) void) void {
        var stmt: ?*sqlite.sqlite3_stmt = null;
        _ = sqlite.sqlite3_prepare_v2(conn.db, sql.ptr, @intCast(sql.len + 1), &stmt, null);
        defer _ = sqlite.sqlite3_finalize(stmt);

        // 执行并回调结果
        while (sqlite.sqlite3_step(stmt) == sqlite.SQLITE_ROW) {
            const text = sqlite.sqlite3_column_text(stmt, 0);
            callback(std.mem.span(text));
        }
    }
};

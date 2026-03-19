//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 236
// Language: zig
// Block ID: d52cf504
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const c = @cImport({
    @cInclude("sqlite3.h");
});

pub fn main() !void {
    var db: ?*c.sqlite3 = null;

    // 打开数据库
    const rc = c.sqlite3_open("test.db", &db);
    if (rc != c.SQLITE_OK) {
        std.debug.print("Cannot open database: {s}\n",
            .{c.sqlite3_errmsg(db)});
        return error.DatabaseError;
    }
    defer _ = c.sqlite3_close(db);

    // 创建表
    var err_msg: [*c]u8 = undefined;
    const sql = "CREATE TABLE IF NOT EXISTS users (" ++
                "id INTEGER PRIMARY KEY, " ++
                "name TEXT NOT NULL);";

    const create_rc = c.sqlite3_exec(
        db, sql, null, null, &err_msg);

    if (create_rc != c.SQLITE_OK) {
        std.debug.print("SQL error: {s}\n", .{err_msg});
        c.sqlite3_free(err_msg);
        return error.SqlError;
    }

    std.debug.print("Table created successfully\n", .{});
}

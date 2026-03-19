//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\01_Zig_C_Interoperability.md
// Line: 132
// Language: zig
// Block ID: c4e60dea
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// sqlite.zig
const c = @cImport({
    @cInclude("sqlite3.h");
});

const std = @import("std");

pub const Database = struct {
    db: ?*c.sqlite3,

    pub fn open(path: []const u8) !Database {
        var db: ?*c.sqlite3 = null;
        const result = c.sqlite3_open(path.ptr, &db);

        if (result != c.SQLITE_OK) {
            return error.OpenFailed;
        }

        return .{ .db = db };
    }

    pub fn close(self: *Database) void {
        if (self.db) |db| {
            _ = c.sqlite3_close(db);
            self.db = null;
        }
    }

    pub fn execute(self: Database, sql: []const u8) !void {
        var err_msg: [*c]u8 = null;
        const result = c.sqlite3_exec(
            self.db,
            sql.ptr,
            null,
            null,
            &err_msg
        );

        if (result != c.SQLITE_OK) {
            if (err_msg) |msg| {
                std.log.err("SQL error: {s}", .{msg});
                c.sqlite3_free(msg);
            }
            return error.ExecutionFailed;
        }
    }
};

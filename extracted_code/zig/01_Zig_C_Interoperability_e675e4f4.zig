//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\01_Zig_C_Interoperability.md
// Line: 764
// Language: zig
// Block ID: e675e4f4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// sqlite_wrapper.zig - 完整的 SQLite Zig 绑定

const std = @import("std");
const c = @cImport({
    @cInclude("sqlite3.h");
});

pub const Error = error{
    Error,
    Internal,
    Perm,
    Abort,
    Busy,
    Locked,
    NoMem,
    ReadOnly,
    Interrupt,
    IOError,
    Corrupt,
    NotFound,
    Full,
    CantOpen,
    Protocol,
    Empty,
    Schema,
    TooBig,
    Constraint,
    Mismatch,
    Misuse,
    NoLFS,
    Auth,
    Format,
    Range,
    NotADB,
};

fn check(rc: c_int) !void {
    return switch (rc) {
        c.SQLITE_OK => {},
        c.SQLITE_ERROR => error.Error,
        c.SQLITE_INTERNAL => error.Internal,
        c.SQLITE_PERM => error.Perm,
        c.SQLITE_ABORT => error.Abort,
        c.SQLITE_BUSY => error.Busy,
        c.SQLITE_LOCKED => error.Locked,
        c.SQLITE_NOMEM => error.NoMem,
        c.SQLITE_READONLY => error.ReadOnly,
        c.SQLITE_INTERRUPT => error.Interrupt,
        c.SQLITE_IOERR => error.IOError,
        c.SQLITE_CORRUPT => error.Corrupt,
        c.SQLITE_NOTFOUND => error.NotFound,
        c.SQLITE_FULL => error.Full,
        c.SQLITE_CANTOPEN => error.CantOpen,
        c.SQLITE_PROTOCOL => error.Protocol,
        c.SQLITE_EMPTY => error.Empty,
        c.SQLITE_SCHEMA => error.Schema,
        c.SQLITE_TOOBIG => error.TooBig,
        c.SQLITE_CONSTRAINT => error.Constraint,
        c.SQLITE_MISMATCH => error.Mismatch,
        c.SQLITE_MISUSE => error.Misuse,
        c.SQLITE_NOLFS => error.NoLFS,
        c.SQLITE_AUTH => error.Auth,
        c.SQLITE_FORMAT => error.Format,
        c.SQLITE_RANGE => error.Range,
        c.SQLITE_NOTADB => error.NotADB,
        else => error.Error,
    };
}

pub const Database = struct {
    db: *c.sqlite3,

    pub fn open(path: []const u8) !Database {
        var db: ?*c.sqlite3 = null;
        try check(c.sqlite3_open(path.ptr, &db));
        return .{ .db = db.? };
    }

    pub fn close(self: Database) void {
        _ = c.sqlite3_close(self.db);
    }

    pub fn execute(self: Database, sql: []const u8) !void {
        try check(c.sqlite3_exec(self.db, sql.ptr, null, null, null));
    }

    pub fn prepare(self: Database, sql: []const u8) !Statement {
        var stmt: ?*c.sqlite3_stmt = null;
        try check(c.sqlite3_prepare_v2(self.db, sql.ptr, @intCast(sql.len + 1), &stmt, null));
        return .{ .stmt = stmt.? };
    }

    pub fn lastInsertRowId(self: Database) i64 {
        return c.sqlite3_last_insert_rowid(self.db);
    }

    pub fn changes(self: Database) usize {
        return @intCast(c.sqlite3_changes(self.db));
    }
};

pub const Statement = struct {
    stmt: *c.sqlite3_stmt,

    pub fn finalize(self: Statement) void {
        _ = c.sqlite3_finalize(self.stmt);
    }

    pub fn step(self: Statement) !bool {
        const rc = c.sqlite3_step(self.stmt);
        if (rc == c.SQLITE_ROW) return true;
        if (rc == c.SQLITE_DONE) return false;
        try check(rc);
        unreachable;
    }

    pub fn reset(self: Statement) !void {
        try check(c.sqlite3_reset(self.stmt));
    }

    // 参数绑定
    pub fn bindInt(self: Statement, idx: usize, value: i32) !void {
        try check(c.sqlite3_bind_int(self.stmt, @intCast(idx), value));
    }

    pub fn bindInt64(self: Statement, idx: usize, value: i64) !void {
        try check(c.sqlite3_bind_int64(self.stmt, @intCast(idx), value));
    }

    pub fn bindDouble(self: Statement, idx: usize, value: f64) !void {
        try check(c.sqlite3_bind_double(self.stmt, @intCast(idx), value));
    }

    pub fn bindText(self: Statement, idx: usize, value: []const u8) !void {
        try check(c.sqlite3_bind_text(self.stmt, @intCast(idx), value.ptr, @intCast(value.len), c.SQLITE_STATIC));
    }

    pub fn bindNull(self: Statement, idx: usize) !void {
        try check(c.sqlite3_bind_null(self.stmt, @intCast(idx)));
    }

    // 结果获取
    pub fn columnInt(self: Statement, idx: usize) i32 {
        return c.sqlite3_column_int(self.stmt, @intCast(idx));
    }

    pub fn columnInt64(self: Statement, idx: usize) i64 {
        return c.sqlite3_column_int64(self.stmt, @intCast(idx));
    }

    pub fn columnDouble(self: Statement, idx: usize) f64 {
        return c.sqlite3_column_double(self.stmt, @intCast(idx));
    }

    pub fn columnText(self: Statement, idx: usize) []const u8 {
        const ptr = c.sqlite3_column_text(self.stmt, @intCast(idx));
        const len = c.sqlite3_column_bytes(self.stmt, @intCast(idx));
        return ptr[0..len];
    }

    pub fn columnType(self: Statement, idx: usize) ColumnType {
        return @enumFromInt(c.sqlite3_column_type(self.stmt, @intCast(idx)));
    }
};

pub const ColumnType = enum(c_int) {
    integer = c.SQLITE_INTEGER,
    float = c.SQLITE_FLOAT,
    blob = c.SQLITE_BLOB,
    null = c.SQLITE_NULL,
    text = c.SQLITE3_TEXT,
};

// 使用示例
pub fn example() !void {
    const db = try Database.open("test.db");
    defer db.close();

    try db.execute(
        \\CREATE TABLE IF NOT EXISTS users (
        \\    id INTEGER PRIMARY KEY,
        \\    name TEXT,
        \\    age INTEGER
        \\)
    );

    const stmt = try db.prepare("INSERT INTO users (name, age) VALUES (?, ?)");
    defer stmt.finalize();

    try stmt.bindText(1, "Alice");
    try stmt.bindInt(2, 30);
    _ = try stmt.step();
    try stmt.reset();

    try stmt.bindText(1, "Bob");
    try stmt.bindInt(2, 25);
    _ = try stmt.step();
}

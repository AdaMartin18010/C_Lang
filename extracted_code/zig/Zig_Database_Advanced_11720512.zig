//
// Auto-generated from: Zig\Zig_Database_Advanced.md
// Line: 89
// Language: zig
// Block ID: 11720512
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const sqlite = @cImport({
    @cInclude("sqlite3.h");
});

pub const ConnectionPool = struct {
    const Connection = struct {
        db: ?*sqlite.sqlite3,
        in_use: bool,
    };

    allocator: std.mem.Allocator,
    connections: []Connection,
    path: []const u8,
    mutex: std.Thread.Mutex,
    semaphore: std.Thread.Semaphore,

    pub fn init(allocator: std.mem.Allocator, path: []const u8, size: usize) !ConnectionPool {
        var connections = try allocator.alloc(Connection, size);
        errdefer allocator.free(connections);

        for (connections) |*conn| {
            const result = sqlite.sqlite3_open(path.ptr, &conn.db);
            if (result != sqlite.SQLITE_OK) return error.OpenFailed;
            conn.in_use = false;
        }

        return .{
            .allocator = allocator,
            .connections = connections,
            .path = path,
            .mutex = .{},
            .semaphore = .{ .permits = size },
        };
    }

    pub fn deinit(self: *ConnectionPool) void {
        for (self.connections) |*conn| {
            if (conn.db) |db| {
                _ = sqlite.sqlite3_close(db);
            }
        }
        self.allocator.free(self.connections);
    }

    pub fn acquire(self: *ConnectionPool) !*Connection {
        self.semaphore.wait();

        self.mutex.lock();
        defer self.mutex.unlock();

        for (self.connections) |*conn| {
            if (!conn.in_use) {
                conn.in_use = true;
                return conn;
            }
        }

        return error.NoAvailableConnection;
    }

    pub fn release(self: *ConnectionPool, conn: *Connection) void {
        self.mutex.lock();
        defer self.mutex.unlock();

        conn.in_use = false;
        self.semaphore.post();
    }
};

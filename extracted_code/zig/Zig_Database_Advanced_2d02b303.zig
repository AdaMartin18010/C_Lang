//
// Auto-generated from: Zig\Zig_Database_Advanced.md
// Line: 216
// Language: zig
// Block ID: 2d02b303
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

pub const Migration = struct {
    version: i32,
    up: []const u8,
    down: []const u8,
};

pub const MigrationManager = struct {
    const migrations = [_]Migration{
        .{
            .version = 1,
            .up =
                \\CREATE TABLE users (
                \\    id INTEGER PRIMARY KEY,
                \\    name TEXT NOT NULL,
                \\    email TEXT UNIQUE
                \\);
            ,
            .down = "DROP TABLE users;",
        },
        .{
            .version = 2,
            .up =
                \\ALTER TABLE users ADD COLUMN created_at INTEGER;
            ,
            .down = "-- 无法回滚 ALTER TABLE",
        },
    };

    pub fn migrate(db: *sqlite.sqlite3) !void {
        // 创建迁移记录表
        _ = sqlite.sqlite3_exec(db,
            "CREATE TABLE IF NOT EXISTS schema_migrations (version INTEGER PRIMARY KEY);",
            null, null, null);

        // 获取当前版本
        var current_version: i32 = 0;
        var stmt: ?*sqlite.sqlite3_stmt = null;
        _ = sqlite.sqlite3_prepare_v2(db,
            "SELECT MAX(version) FROM schema_migrations;", -1, &stmt, null);
        if (sqlite.sqlite3_step(stmt) == sqlite.SQLITE_ROW) {
            current_version = sqlite.sqlite3_column_int(stmt, 0);
        }
        _ = sqlite.sqlite3_finalize(stmt);

        // 执行未应用的迁移
        for (migrations) |migration| {
            if (migration.version > current_version) {
                _ = sqlite.sqlite3_exec(db, migration.up.ptr, null, null, null);

                // 记录迁移
                var insert_stmt: ?*sqlite.sqlite3_stmt = null;
                _ = sqlite.sqlite3_prepare_v2(db,
                    "INSERT INTO schema_migrations (version) VALUES (?);", -1, &insert_stmt, null);
                _ = sqlite.sqlite3_bind_int(insert_stmt, 1, migration.version);
                _ = sqlite.sqlite3_step(insert_stmt);
                _ = sqlite.sqlite3_finalize(insert_stmt);
            }
        }
    }
};

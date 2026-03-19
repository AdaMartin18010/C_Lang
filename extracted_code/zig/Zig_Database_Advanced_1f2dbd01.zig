//
// Auto-generated from: Zig\Zig_Database_Advanced.md
// Line: 535
// Language: zig
// Block ID: 1f2dbd01
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

pub const LSMTree = struct {
    const MEMTABLE_SIZE = 4 * 1024 * 1024;  // 4MB

    allocator: std.mem.Allocator,
    memtable: std.TreeMap([]const u8, []const u8, std.mem.lessThanU8),
    wal: std.fs.File,
    level0_files: std.ArrayList([]const u8),

    pub fn init(allocator: std.mem.Allocator, dir: []const u8) !LSMTree {
        const wal_path = try std.fs.path.join(allocator, &.{ dir, "wal.log" });
        const wal = try std.fs.cwd().createFile(wal_path, .{ .read = true });

        return .{
            .allocator = allocator,
            .memtable = std.TreeMap([]const u8, []const u8, std.mem.lessThanU8).init(allocator),
            .wal = wal,
            .level0_files = std.ArrayList([]const u8).init(allocator),
        };
    }

    pub fn put(self: *LSMTree, key: []const u8, value: []const u8) !void {
        // 写入 WAL
        const entry = try std.fmt.allocPrint(self.allocator, "{d}:{s}:{s}\n", .{
            key.len, key, value
        });
        defer self.allocator.free(entry);
        _ = try self.wal.write(entry);

        // 写入 memtable
        const key_copy = try self.allocator.dupe(u8, key);
        const value_copy = try self.allocator.dupe(u8, value);
        try self.memtable.put(key_copy, value_copy);

        // 检查是否需要刷盘
        if (self.memtable.count() * 100 > MEMTABLE_SIZE) {
            try self.flush();
        }
    }

    pub fn get(self: *LSMTree, key: []const u8) !?[]const u8 {
        // 先查 memtable
        if (self.memtable.get(key)) |value| {
            return value;
        }

        // 查 SSTable 文件
        for (self.level0_files.items) |file| {
            if (try self.searchSSTable(file, key)) |value| {
                return value;
            }
        }

        return null;
    }

    fn flush(self: *LSMTree) !void {
        // 排序并写入 SSTable
        const filename = try std.fmt.allocPrint(self.allocator, "sst-{d}.db", .{
            std.time.timestamp()
        });
        defer self.allocator.free(filename);

        var file = try std.fs.cwd().createFile(filename, .{});
        defer file.close();

        var iter = self.memtable.iterator();
        while (iter.next()) |entry| {
            try file.writer().print("{s}:{s}\n", .{ entry.key_ptr.*, entry.value_ptr.* });
        }

        try self.level0_files.append(try self.allocator.dupe(u8, filename));

        // 清空 memtable
        self.memtable.clearAndFree();

        // 截断 WAL
        try self.wal.setEndPos(0);
    }

    fn searchSSTable(self: LSMTree, filename: []const u8, key: []const u8) !?[]const u8 {
        _ = self;
        var file = try std.fs.cwd().openFile(filename, .{});
        defer file.close();

        const content = try file.readToEndAlloc(std.heap.page_allocator, 100 * 1024 * 1024);
        defer std.heap.page_allocator.free(content);

        var lines = std.mem.split(u8, content, "\n");
        while (lines.next()) |line| {
            if (line.len == 0) continue;

            const sep = std.mem.indexOf(u8, line, ":") orelse continue;
            const k = line[0..sep];
            const v = line[sep + 1 ..];

            if (std.mem.eql(u8, k, key)) {
                return v;
            }
        }

        return null;
    }
};

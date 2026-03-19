//
// Auto-generated from: Zig\Zig_Database_Advanced.md
// Line: 840
// Language: zig
// Block ID: 4de9f1fc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

pub const KVStore = struct {
    allocator: std.mem.Allocator,
    index: std.StringHashMap(u64),  // key -> offset
    data_file: std.fs.File,

    pub fn init(allocator: std.mem.Allocator, path: []const u8) !KVStore {
        const data_file = try std.fs.cwd().createFile(path, .{ .read = true });

        var store = KVStore{
            .allocator = allocator,
            .index = std.StringHashMap(u64).init(allocator),
            .data_file = data_file,
        };

        // 加载索引
        try store.loadIndex();

        return store;
    }

    pub fn deinit(self: *KVStore) void {
        self.saveIndex() catch {};
        self.index.deinit();
        self.data_file.close();
    }

    pub fn put(self: *KVStore, key: []const u8, value: []const u8) !void {
        // 格式: [key_len:4][key][value_len:4][value]
        const offset = try self.data_file.getEndPos();

        const key_len = @as(u32, @intCast(key.len));
        try self.data_file.writer().writeIntLittle(u32, key_len);
        try self.data_file.writer().writeAll(key);

        const value_len = @as(u32, @intCast(value.len));
        try self.data_file.writer().writeIntLittle(u32, value_len);
        try self.data_file.writer().writeAll(value);

        // 更新索引
        const key_copy = try self.allocator.dupe(u8, key);
        try self.index.put(key_copy, offset);
    }

    pub fn get(self: KVStore, key: []const u8) !?[]const u8 {
        const offset = self.index.get(key) orelse return null;

        try self.data_file.seekTo(offset);
        const reader = self.data_file.reader();

        // 跳过 key
        const key_len = try reader.readIntLittle(u32);
        try self.data_file.seekBy(key_len);

        // 读取 value
        const value_len = try reader.readIntLittle(u32);
        const value = try self.allocator.alloc(u8, value_len);
        _ = try reader.readAll(value);

        return value;
    }

    fn loadIndex(self: *KVStore) !void {
        // 从文件重建索引
        const file_size = try self.data_file.getEndPos();
        if (file_size == 0) return;

        try self.data_file.seekTo(0);
        const reader = self.data_file.reader();

        var offset: u64 = 0;
        while (offset < file_size) {
            // 读取 key
            const key_len = try reader.readIntLittle(u32);
            const key = try self.allocator.alloc(u8, key_len);
            _ = try reader.readAll(key);

            // 跳过 value
            const value_len = try reader.readIntLittle(u32);
            try self.data_file.seekBy(value_len);

            try self.index.put(key, offset);
            offset = try self.data_file.getPos();
        }
    }

    fn saveIndex(self: KVStore) !void {
        // 保存索引到文件
        _ = self;
    }
};

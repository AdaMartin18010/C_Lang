//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\02_SQLite_BTree_Migration.md
// Line: 434
// Language: zig
// Block ID: 0b855595
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// pager.zig - 页面管理器
const std = @import("std");

pub const Pager = struct {
    allocator: std.mem.Allocator,
    fd: ?std.fs.File,
    page_size: u16,
    cache: PageCache,

    const PageCache = std.HashMap(u32, *Page, std.hash_map.defaultContext(u32), std.hash_map.default_max_load_percentage);

    pub fn init(allocator: std.mem.Allocator, page_size: u16) !Pager {
        return Pager{
            .allocator = allocator,
            .fd = null,
            .page_size = page_size,
            .cache = PageCache.init(allocator),
        };
    }

    pub fn deinit(self: *Pager) void {
        var it = self.cache.valueIterator();
        while (it.next()) |page| {
            self.allocator.destroy(page.*);
        }
        self.cache.deinit();
        if (self.fd) |fd| fd.close();
    }

    pub fn getPage(self: *Pager, pgno: u32) !*Page {
        // 检查缓存
        if (self.cache.get(pgno)) |page| {
            page.ref_count += 1;
            return page;
        }

        // 分配新页面
        const page = try self.allocator.create(Page);
        errdefer self.allocator.destroy(page);

        page.* = .{
            .pgno = pgno,
            .data = try self.allocator.alloc(u8, self.page_size),
            .ref_count = 1,
        };

        // 从磁盘读取
        if (self.fd) |fd| {
            const offset = pgno * self.page_size;
            try fd.seekTo(offset);
            _ = try fd.readAll(page.data);
        } else {
            @memset(page.data, 0);
        }

        try self.cache.put(pgno, page);
        return page;
    }

    pub fn releasePage(self: *Pager, page: *Page) void {
        page.ref_count -= 1;
        if (page.ref_count == 0) {
            // 延迟释放策略
        }
    }
};

const Page = struct {
    pgno: u32,
    data: []u8,
    ref_count: u32,

    pub fn cellCount(self: *const Page) u16 {
        // 从页面头解析
        return std.mem.readInt(u16, self.data[0..2], .big);
    }

    pub fn isLeaf(self: *const Page) bool {
        return (self.data[2] & 0x01) != 0;
    }

    pub fn getKey(self: *const Page, idx: u16) ![]const u8 {
        // 从 cell pointer array 获取
        const offset = std.mem.readInt(u16, self.data[4 + idx * 2..][0..2], .big);
        const len = std.mem.readInt(u16, self.data[offset..][0..2], .big);
        return self.data[offset + 2..][0..len];
    }
};

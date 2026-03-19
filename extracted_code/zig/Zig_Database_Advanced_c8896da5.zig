//
// Auto-generated from: Zig\Zig_Database_Advanced.md
// Line: 285
// Language: zig
// Block ID: c8896da5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub const PAGE_SIZE = 4096;
pub const MAX_PAGES = 1024;

pub const Page = struct {
    id: u32,
    data: [PAGE_SIZE]u8,
    is_dirty: bool,
    pin_count: u32,
};

pub const PageManager = struct {
    file: std.fs.File,
    page_table: std.AutoHashMap(u32, *Page),
    buffer_pool: [MAX_PAGES]Page,
    free_list: std.ArrayList(u32),
    next_page_id: u32,

    pub fn init(allocator: std.mem.Allocator, path: []const u8) !PageManager {
        const file = try std.fs.cwd().createFile(path, .{ .read = true });

        return .{
            .file = file,
            .page_table = std.AutoHashMap(u32, *Page).init(allocator),
            .buffer_pool = undefined,
            .free_list = std.ArrayList(u32).init(allocator),
            .next_page_id = 0,
        };
    }

    pub fn deinit(self: *PageManager) void {
        // 刷脏页
        self.flushAll() catch {};
        self.file.close();
        self.page_table.deinit();
        self.free_list.deinit();
    }

    pub fn allocatePage(self: *PageManager) !*Page {
        const id = if (self.free_list.popOrNull()) |free_id|
            free_id
        else blk: {
            const id = self.next_page_id;
            self.next_page_id += 1;
            break :blk id;
        };

        // 找到空闲槽
        for (&self.buffer_pool) |*page| {
            if (page.pin_count == 0) {
                page.* = .{
                    .id = id,
                    .data = [_]u8{0} ** PAGE_SIZE,
                    .is_dirty = true,
                    .pin_count = 1,
                };
                try self.page_table.put(id, page);
                return page;
            }
        }

        return error.OutOfMemory;
    }

    pub fn getPage(self: *PageManager, id: u32) !*Page {
        // 检查是否在内存
        if (self.page_table.get(id)) |page| {
            page.pin_count += 1;
            return page;
        }

        // 从磁盘加载
        for (&self.buffer_pool) |*page| {
            if (page.pin_count == 0) {
                try self.readPageFromDisk(id, page);
                page.pin_count = 1;
                try self.page_table.put(id, page);
                return page;
            }
        }

        return error.OutOfMemory;
    }

    pub fn unpinPage(self: *PageManager, page: *Page) void {
        _ = self;
        if (page.pin_count > 0) {
            page.pin_count -= 1;
        }
    }

    pub fn markDirty(self: *PageManager, page: *Page) void {
        _ = self;
        page.is_dirty = true;
    }

    fn readPageFromDisk(self: PageManager, id: u32, page: *Page) !void {
        const offset = id * PAGE_SIZE;
        try self.file.seekTo(offset);
        _ = try self.file.read(&page.data);
        page.id = id;
        page.is_dirty = false;
    }

    fn writePageToDisk(self: PageManager, page: *Page) !void {
        const offset = page.id * PAGE_SIZE;
        try self.file.seekTo(offset);
        _ = try self.file.write(&page.data);
        page.is_dirty = false;
    }

    fn flushAll(self: *PageManager) !void {
        for (self.buffer_pool) |*page| {
            if (page.is_dirty) {
                try self.writePageToDisk(page);
            }
        }
    }
};

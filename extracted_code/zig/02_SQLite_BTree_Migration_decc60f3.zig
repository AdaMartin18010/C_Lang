//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\02_SQLite_BTree_Migration.md
// Line: 93
// Language: zig
// Block ID: decc60f3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// btree.zig - Zig B-Tree 实现
const std = @import("std");
const Allocator = std.mem.Allocator;

// C ABI 兼容导出
pub const Btree = opaque {};
pub const BtCursor = opaque {};
pub const Pgno = u32;

// Zig 内部结构
const BtreeImpl = struct {
    db: ?*sqlite3,
    shared: ?*BtShared,
    in_trans: TransactionState,
    sharable: bool,

    const TransactionState = enum(u8) {
        none = 0,
        read = 1,
        write = 2,
    };
};

const BtShared = struct {
    pager: ?*Pager,
    db: ?*sqlite3,
    cursors: CursorList,
    tmp_space: []u8,
    page_size: u16,
    usable_size: u16,
    n_page: u32,
    max_local: u16,
    min_local: u16,
    max_leaf: u16,
    min_leaf: u16,

    // Zig 内存管理
    allocator: Allocator,

    const CursorList = struct {
        first: ?*BtCursorImpl,
        count: usize,
    };
};

const BtCursorImpl = struct {
    btree: ?*BtreeImpl,
    shared: ?*BtShared,
    next: ?*BtCursorImpl,
    prev: ?*BtCursorImpl,
    key_info: ?*KeyInfo,
    pgno_root: Pgno,
    info: CellInfo,
    int_key: bool,
    flags: CursorFlags,
    idx: [BTCURSOR_MAX_DEPTH]u16,
    page: ?*MemPage,

    const CursorFlags = packed struct {
        valid_n_key: bool = false,
        skip_next: bool = false,
        valid_ovfl: bool = false,
        _padding: u5 = 0,
    };
};

const CellInfo = extern struct {
    n_size: u64,
    n_payload: u32,
    n_local: u16,
    i_overflow: u16,
    n_key: u64,
    p_payload: [*]u8,
};

const BTCURSOR_MAX_DEPTH = 20;

// ═══════════════════════════════════════════════════════════════════════════
// C ABI 导出函数
// ═══════════════════════════════════════════════════════════════════════════

/// 打开 B-Tree (C ABI)
export fn sqlite3BtreeOpen(
    p_vfs: ?*sqlite3_vfs,
    z_filename: [*c]const u8,
    db: ?*sqlite3,
    pp_btree: *?*Btree,
    flags: c_int,
    vfs_flags: c_int,
) c_int {
    const allocator = std.heap.c_allocator;

    // Zig 实现
    const btree = allocator.create(BtreeImpl) catch {
        return SQLITE_NOMEM;
    };
    errdefer allocator.destroy(btree);

    btree.* = .{
        .db = db,
        .shared = null,
        .in_trans = .none,
        .sharable = (flags & SQLITE_OPEN_SHAREDCACHE) != 0,
    };

    // 初始化 shared...

    pp_btree.* = @ptrCast(btree);
    return SQLITE_OK;
}

/// 关闭 B-Tree (C ABI)
export fn sqlite3BtreeClose(p: ?*Btree) c_int {
    const btree = @as(?*BtreeImpl, @ptrCast(p)) orelse return SQLITE_OK;
    const allocator = std.heap.c_allocator;

    // 清理资源...

    allocator.destroy(btree);
    return SQLITE_OK;
}

/// 开始事务 (C ABI)
export fn sqlite3BtreeBeginTrans(p: ?*Btree, wrflag: c_int) c_int {
    const btree = @as(?*BtreeImpl, @ptrCast(p)) orelse return SQLITE_MISUSE;

    // 使用 Zig 的错误处理
    beginTransaction(btree, wrflag != 0) catch |err| {
        return switch (err) {
            error.NoMem => SQLITE_NOMEM,
            error.Busy => SQLITE_BUSY,
            error.ReadOnly => SQLITE_READONLY,
            else => SQLITE_ERROR,
        };
    };

    return SQLITE_OK;
}

// ═══════════════════════════════════════════════════════════════════════════
// Zig 内部实现
// ═══════════════════════════════════════════════════════════════════════════

const BtreeError = error{
    NoMem,
    Busy,
    ReadOnly,
    Corrupt,
    Full,
    Empty,
    Schema,
    TooBig,
    Constraint,
    Mismatch,
};

fn beginTransaction(btree: *BtreeImpl, write: bool) BtreeError!void {
    if (btree.in_trans != .none) {
        return error.Busy;
    }

    const shared = btree.shared orelse return error.Corrupt;

    // Zig 的安全检查
    if (write and btree.db) |db| {
        if (db.flags & SQLITE_ReadOnly != 0) {
            return error.ReadOnly;
        }
    }

    // 开始事务逻辑...
    btree.in_trans = if (write) .write else .read;
}

/// B-Tree 游标操作
const Cursor = struct {
    impl: *BtCursorImpl,

    pub fn init(allocator: Allocator, btree: *BtreeImpl, root_page: Pgno) !Cursor {
        const impl = try allocator.create(BtCursorImpl);
        errdefer allocator.destroy(impl);

        impl.* = .{
            .btree = btree,
            .shared = btree.shared,
            .next = null,
            .prev = null,
            .key_info = null,
            .pgno_root = root_page,
            .info = undefined,
            .int_key = false,
            .flags = .{},
            .idx = undefined,
            .page = null,
        };

        // 添加到游标链表
        addCursorToList(btree.shared.?, impl);

        return Cursor{ .impl = impl };
    }

    pub fn deinit(self: *Cursor, allocator: Allocator) void {
        removeCursorFromList(self.impl);
        allocator.destroy(self.impl);
        self.* = undefined;
    }

    pub fn moveTo(self: *Cursor, key: []const u8) BtreeError!bool {
        // 使用 Zig 的切片进行安全访问
        return try self.search(key);
    }

    fn search(self: *Cursor, key: []const u8) BtreeError!bool {
        const shared = self.impl.shared orelse return error.Corrupt;

        var page = shared.pager.?.getPage(self.impl.pgno_root)
            catch return error.Corrupt;

        // B-Tree 搜索算法...
        while (true) {
            const cell_count = page.cellCount();

            // 二分查找
            var low: u16 = 0;
            var high: u16 = cell_count;

            while (low < high) {
                const mid = (low + high) / 2;
                const cell_key = try page.getKey(mid);

                const cmp = std.mem.order(u8, key, cell_key);
                if (cmp == .eq) {
                    self.impl.idx[0] = mid;
                    return true;
                } else if (cmp == .lt) {
                    high = mid;
                } else {
                    low = mid + 1;
                }
            }

            // 检查子页面...
            if (page.isLeaf()) break;

            const child_pgno = try page.getChildPage(low);
            page = shared.pager.?.getPage(child_pgno)
                catch return error.Corrupt;
        }

        return false;
    }
};

// 辅助函数
fn addCursorToList(shared: *BtShared, cursor: *BtCursorImpl) void {
    cursor.next = shared.cursors.first;
    if (shared.cursors.first) |first| {
        first.prev = cursor;
    }
    shared.cursors.first = cursor;
    shared.cursors.count += 1;
}

fn removeCursorFromList(cursor: *BtCursorImpl) void {
    if (cursor.prev) |prev| {
        prev.next = cursor.next;
    }
    if (cursor.next) |next| {
        next.prev = cursor.prev;
    }
    if (cursor.btree) |btree| {
        if (btree.shared) |shared| {
            if (shared.cursors.first == cursor) {
                shared.cursors.first = cursor.next;
            }
            shared.cursors.count -= 1;
        }
    }
}

// 测试
test "Btree basic operations" {
    const allocator = std.testing.allocator;

    // 创建模拟 db
    var db: sqlite3 = undefined;

    var btree: ?*Btree = null;
    const result = sqlite3BtreeOpen(null, ":memory:", &db, &btree,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
    try std.testing.expectEqual(SQLITE_OK, result);

    defer _ = sqlite3BtreeClose(btree);

    // 开始事务
    const begin_result = sqlite3BtreeBeginTrans(btree, 1);
    try std.testing.expectEqual(SQLITE_OK, begin_result);
}

// SQLite 错误码
const SQLITE_OK = 0;
const SQLITE_ERROR = 1;
const SQLITE_INTERNAL = 2;
const SQLITE_PERM = 3;
const SQLITE_ABORT = 4;
const SQLITE_BUSY = 5;
const SQLITE_LOCKED = 6;
const SQLITE_NOMEM = 7;
const SQLITE_READONLY = 8;
const SQLITE_INTERRUPT = 9;
const SQLITE_IOERR = 10;
const SQLITE_CORRUPT = 11;
const SQLITE_NOTFOUND = 12;
const SQLITE_FULL = 13;
const SQLITE_CANTOPEN = 14;
const SQLITE_PROTOCOL = 15;
const SQLITE_EMPTY = 16;
const SQLITE_SCHEMA = 17;
const SQLITE_TOOBIG = 18;
const SQLITE_CONSTRAINT = 19;
const SQLITE_MISMATCH = 20;
const SQLITE_MISUSE = 21;

// 前向声明
const sqlite3 = opaque {};
const sqlite3_vfs = opaque {};
const Pager = opaque {};
const KeyInfo = opaque {};
const MemPage = opaque {};
const SQLITE_OPEN_SHAREDCACHE = 0x00020000;
const SQLITE_OPEN_READWRITE = 0x00000002;
const SQLITE_OPEN_CREATE = 0x00000004;
const SQLITE_ReadOnly = 0x00000001;

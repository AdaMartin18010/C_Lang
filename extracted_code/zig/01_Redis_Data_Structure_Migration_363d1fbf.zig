//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\01_Redis_Data_Structure_Migration.md
// Line: 438
// Language: zig
// Block ID: 363d1fbf
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// skiplist.zig - Redis 跳表 Zig 实现
const std = @import("std");
const Allocator = std.mem.Allocator;

pub fn SkipList(comptime T: type, comptime compare: fn (T, T) i32) type {
    return struct {
        const Self = @This();
        const MAX_LEVEL = 32;
        const P = 0.25;

        pub const Node = struct {
            obj: T,
            backward: ?*Node,
            level: []Level,

            const Level = struct {
                forward: ?*Node,
                span: usize,
            };
        };

        header: *Node,
        tail: ?*Node,
        length: usize,
        level: u8,
        allocator: Allocator,

        pub fn init(allocator: Allocator) !Self {
            const header = try createNode(allocator, MAX_LEVEL, undefined);
            for (header.level) |*lvl| {
                lvl.forward = null;
                lvl.span = 0;
            }
            header.backward = null;

            return Self{
                .header = header,
                .tail = null,
                .length = 0,
                .level = 1,
                .allocator = allocator,
            };
        }

        pub fn deinit(self: *Self) void {
            var node = self.header;
            while (node) |n| {
                const next = if (n.level.len > 0) n.level[0].forward else null;
                self.freeNode(n);
                node = next;
            }
        }

        fn createNode(allocator: Allocator, level: u8, obj: T) !*Node {
            const node = try allocator.create(Node);
            node.* = .{
                .obj = obj,
                .backward = null,
                .level = try allocator.alloc(Node.Level, level),
            };
            return node;
        }

        fn freeNode(self: *Self, node: *Node) void {
            self.allocator.free(node.level);
            self.allocator.destroy(node);
        }

        fn randomLevel() u8 {
            var level: u8 = 1;
            var rng = std.rand.DefaultPrng.init(@intCast(std.time.nanoTimestamp()));
            while (level < MAX_LEVEL and rng.random().float(f64) < P) {
                level += 1;
            }
            return level;
        }

        pub fn insert(self: *Self, obj: T) !void {
            var update: [MAX_LEVEL]?*Node = undefined;
            var rank: [MAX_LEVEL]usize = undefined;

            var x = self.header;
            var i = @as(isize, @intCast(self.level - 1));
            while (i >= 0) : (i -= 1) {
                const ui = @as(usize, @intCast(i));
                rank[ui] = if (i == @as(isize, @intCast(self.level - 1))) 0 else rank[ui + 1];

                while (x.level[ui].forward) |next| {
                    if (compare(next.obj, obj) < 0) {
                        rank[ui] += x.level[ui].span;
                        x = next;
                    } else break;
                }
                update[ui] = x;
            }

            const level = randomLevel();
            if (level > self.level) {
                var l = self.level;
                while (l < level) : (l += 1) {
                    rank[l] = 0;
                    update[l] = self.header;
                    update[l].?.level[l].span = self.length;
                }
                self.level = level;
            }

            x = try createNode(self.allocator, level, obj);

            var l: u8 = 0;
            while (l < level) : (l += 1) {
                x.level[l].forward = update[l].?.level[l].forward;
                update[l].?.level[l].forward = x;
                x.level[l].span = update[l].?.level[l].span - (rank[0] - rank[l]);
                update[l].?.level[l].span = (rank[0] - rank[l]) + 1;
            }

            var ll = level;
            while (ll < self.level) : (ll += 1) {
                update[ll].?.level[ll].span += 1;
            }

            x.backward = if (update[0] == self.header) null else update[0];
            if (x.level[0].forward) |next| {
                next.backward = x;
            } else {
                self.tail = x;
            }

            self.length += 1;
        }
    };
}

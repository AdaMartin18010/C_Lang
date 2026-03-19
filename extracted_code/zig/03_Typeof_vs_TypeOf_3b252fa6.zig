//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 1877
// Language: zig
// Block ID: 3b252fa6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig comptime 实现

fn Vector(comptime T: type) type {
    return struct {
        data: []T,
        len: usize,
        cap: usize,
        allocator: std.mem.Allocator,

        const Self = @This();

        fn init(allocator: std.mem.Allocator) Self {
            return .{
                .data = &[_]T{},
                .len = 0,
                .cap = 0,
                .allocator = allocator,
            };
        }

        fn push(self: *Self, val: T) !void {
            if (self.len >= self.cap) {
                const new_cap = if (self.cap == 0) 8 else self.cap * 2;
                self.data = try self.allocator.realloc(self.data, new_cap);
                self.cap = new_cap;
            }
            self.data[self.len] = val;
            self.len += 1;
        }

        fn deinit(self: *Self) void {
            self.allocator.free(self.data);
        }
    };
}

// 使用
const IntVec = Vector(i32);
const DoubleVec = Vector(f64);

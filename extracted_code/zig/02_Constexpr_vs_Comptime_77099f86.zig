//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 2835
// Language: zig
// Block ID: 77099f86
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 泛型容器
fn Stack(comptime T: type, comptime capacity: usize) type {
    return struct {
        data: [capacity]T,
        top: isize = -1,

        const Self = @This();
        const Error = error{ StackOverflow, StackUnderflow };

        pub fn init() Self {
            return .{
                .data = undefined,
                .top = -1,
            };
        }

        pub fn push(self: *Self, value: T) Error!void {
            if (self.top >= capacity - 1) return error.StackOverflow;
            self.top += 1;
            self.data[@intCast(self.top)] = value;
        }

        pub fn pop(self: *Self) Error!T {
            if (self.top < 0) return error.StackUnderflow;
            const value = self.data[@intCast(self.top)];
            self.top -= 1;
            return value;
        }

        pub fn isEmpty(self: Self) bool {
            return self.top < 0;
        }

        pub fn isFull(self: Self) bool {
            return self.top >= capacity - 1;
        }
    };
}

// 实例化
const IntStack = Stack(i32, 100);
const DoubleStack = Stack(f64, 50);

// 使用
fn example() !void {
    var stack = IntStack.init();
    try stack.push(42);
    const val = try stack.pop();
}

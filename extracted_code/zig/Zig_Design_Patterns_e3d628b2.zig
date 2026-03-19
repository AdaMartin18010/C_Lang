//
// Auto-generated from: Zig\Zig_Design_Patterns.md
// Line: 169
// Language: zig
// Block ID: e3d628b2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用 anytype 实现静态多态

// 可排序接口
fn Comparable(comptime T: type) type {
    return struct {
        pub fn lessThan(a: T, b: T) bool {
            return a < b;
        }
    };
}

// 泛型排序 (编译时单态化)
fn bubbleSort(comptime T: type, items: []T) void {
    const N = items.len;
    var i: usize = 0;
    while (i < N) : (i += 1) {
        var j: usize = 0;
        while (j < N - i - 1) : (j += 1) {
            if (items[j] > items[j + 1]) {
                const temp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = temp;
            }
        }
    }
}

// 鸭子类型接口
fn drawAll(items: anytype) void {
    const Item = @TypeOf(items);
    const info = @typeInfo(Item);

    if (info != .Pointer or info.Pointer.size != .Slice) {
        @compileError("Expected slice");
    }

    for (items) |item| {
        item.draw();
    }
}

// VTable 动态分发
const Drawable = struct {
    ptr: *anyopaque,
    vtable: *const VTable,

    const VTable = struct {
        draw: *const fn (*anyopaque) void,
        getArea: *const fn (*anyopaque) f32,
    };

    pub fn draw(self: Drawable) void {
        self.vtable.draw(self.ptr);
    }

    pub fn getArea(self: Drawable) f32 {
        return self.vtable.getArea(self.ptr);
    }
};

const Circle = struct {
    radius: f32,

    pub fn draw(self: *Circle) void {
        std.log.info("Drawing circle with radius {}", .{self.radius});
    }

    pub fn getArea(self: *Circle) f32 {
        return std.math.pi * self.radius * self.radius;
    }

    pub fn asDrawable(self: *Circle) Drawable {
        return .{
            .ptr = self,
            .vtable = &.{
                .draw = @ptrCast(&draw),
                .getArea = @ptrCast(&getArea),
            },
        };
    }
};

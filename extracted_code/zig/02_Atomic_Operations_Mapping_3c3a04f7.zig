//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 868
// Language: zig
// Block ID: 3c3a04f7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 引用计数
fn Arc(comptime T: type) type {
    return struct {
        const Inner = struct {
            count: Atomic(u32),
            data: T,
        };

        ptr: *Inner,

        pub fn clone(self: @This()) @This() {
            _ = self.ptr.count.fetchAdd(1, .Monotonic);
            return self;
        }

        pub fn drop(self: *@This()) void {
            if (self.ptr.count.fetchSub(1, .Release) == 1) {
                std.atomic.fence(.Acquire);
                // 销毁...
            }
        }
    };
}

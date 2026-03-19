//
// Auto-generated from: Zig\Zig_Design_Patterns.md
// Line: 436
// Language: zig
// Block ID: f4af971a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 编译时策略
fn Sorter(comptime compare: fn (i32, i32) bool) type {
    return struct {
        pub fn sort(items: []i32) void {
            // 使用 compare 进行排序
            std.sort.sort(i32, items, {}, struct {
                pub fn lessThan(_: void, a: i32, b: i32) bool {
                    return compare(a, b);
                }
            }.lessThan);
        }
    };
}

fn ascending(a: i32, b: i32) bool { return a < b; }
fn descending(a: i32, b: i32) bool { return a > b; }

const AscendingSort = Sorter(ascending);
const DescendingSort = Sorter(descending);

// 运行时策略
const CompressionStrategy = union(enum) {
    none,
    gzip: GzipCompressor,
    zlib: ZlibCompressor,

    pub fn compress(self: CompressionStrategy, data: []const u8, out: []u8) !usize {
        switch (self) {
            .none => {
                @memcpy(out[0..data.len], data);
                return data.len;
            },
            .gzip => |c| return c.compress(data, out),
            .zlib => |c| return c.compress(data, out),
        }
    }
};

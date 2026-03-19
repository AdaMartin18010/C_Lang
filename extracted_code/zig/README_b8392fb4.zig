//
// Auto-generated from: Zig\README.md
// Line: 1145
// Language: zig
// Block ID: b8392fb4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 编译时分派
fn printValue(value: anytype) void {
    const T = @TypeOf(value);

    if (comptime @typeInfo(T) == .Int) {
        std.debug.print("Integer: {d}\n", .{value});
    } else if (comptime @typeInfo(T) == .Float) {
        std.debug.print("Float: {e}\n", .{value});
    } else if (comptime @typeInfo(T) == .Pointer and
               @typeInfo(T).Pointer.size == .Slice and
               @typeInfo(T).Pointer.child == u8) {
        std.debug.print("String: {s}\n", .{value});
    }
}

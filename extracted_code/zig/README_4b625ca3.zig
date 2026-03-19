//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 220
// Language: zig
// Block ID: 4b625ca3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 中使用 C 枚举
const c = @cImport(@cInclude("status.h"));

pub fn enumDemo() void {
    var status: c.Status = c.STATUS_OK;

    switch (status) {
        c.STATUS_OK => std.debug.print("OK\n", .{}),
        c.STATUS_ERROR => std.debug.print("Error\n", .{}),
        c.STATUS_PENDING => std.debug.print("Pending\n", .{}),
        else => unreachable,
    }
}

//
// Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
// Line: 494
// Language: zig
// Block ID: 447b1407
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const Error = error{
    NullInput,
    FileNotFound,
    // NoMemory是Zig内置
};

fn doSomething(allocator: std.mem.Allocator, input: []const u8) Error![]u8 {
    if (input.len == 0) return error.NullInput;

    var buffer = try allocator.alloc(u8, 100);
    errdefer allocator.free(buffer);

    var file = std.fs.cwd().openFile(input, .{})
        catch return error.FileNotFound;
    defer file.close();

    // ... 错误自动传播

    return buffer;
}

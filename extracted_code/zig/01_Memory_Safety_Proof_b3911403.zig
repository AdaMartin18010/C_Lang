//
// Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\01_Memory_Safety_Proof.md
// Line: 145
// Language: zig
// Block ID: b3911403
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 自动边界检查
fn arrayMax(arr: []const i32) ?i32 {
    if (arr.len == 0) return null;
    var max = arr[0];
    for (arr[1..]) |val| {
        if (val > max) max = val;
    }
    return max;
}

// 强制空检查
fn safeIncrement(p: ?*const i32) i32 {
    const ptr = p orelse return 0;
    return ptr.* + 1;
}

// 通用分配器模式
fn managedLifecycle(allocator: std.mem.Allocator) !void {
    const arr = try allocator.alloc(i32, 10);
    defer allocator.free(arr);  // 确保释放
    arr[0] = 42;
}

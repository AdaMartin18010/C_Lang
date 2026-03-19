//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 619
// Language: zig
// Block ID: ecbfa179
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 策略1: 错误码 + 输出参数
export fn safe_divide(a: c_int, b: c_int, out: *c_int) c_int {
    if (b == 0) return 1;  // 错误码1: 除零
    out.* = @divTrunc(a, b);
    return 0;  // 成功
}

// 策略2: 特殊返回值 (适用于指针)
export fn find_element(arr: [*]const c_int, len: c_int,
                       target: c_int) ?*const c_int {
    for (0..@intCast(len)) |i| {
        if (arr[i] == target) {
            return &arr[i];
        }
    }
    return null;  // 未找到
}

// 策略3: 回调处理错误
export fn process_with_callback(
    data: [*]const u8,
    len: usize,
    on_error: ?*const fn (code: c_int, msg: [*c]const u8) callconv(.C) void
) c_int {
    // ... 处理 ...
    if (some_error and on_error != null) {
        on_error.?(42, "error message");
    }
    return 0;
}

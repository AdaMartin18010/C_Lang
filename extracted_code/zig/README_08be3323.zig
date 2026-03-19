//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 256
// Language: zig
// Block ID: 08be3323
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// 当结构体过大无法通过寄存器传递时
const LargeStruct = extern struct {
    data: [256]u64,
    count: usize,
};

// C 编译器可能隐式转换为指针传递
// Zig 需要显式处理
export fn process_large_struct(ptr: *const LargeStruct) void {
    // 通过指针访问，避免不必要的拷贝
    for (ptr.data[0..ptr.count]) |item| {
        _ = item;
    }
}

// 如果需要 C 兼容的按值语义
export fn copy_large_struct(s: LargeStruct) LargeStruct {
    return s;  // 编译器将生成适当的拷贝代码
}

//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 284
// Language: zig
// Block ID: a8c18d21
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 指针类型语义

// 1. 单元素指针 (*T)
// 语义: 指向单个 T 类型值的指针
var x: i32 = 10;
var ptr: *i32 = &x;  // ptr 指向 x
ptr.* = 20;          // 解引用赋值

// 2. 多元素指针 ([*]T)
// 语义: 指向未知数量 T 类型元素的指针
var arr: [10]i32 = undefined;
var many_ptr: [*]i32 = &arr;  // 可以执行指针算术
_ = many_ptr + 5;  // 有效

// 3. 切片 ([]T)
// 语义: 指针 + 长度的组合 (胖指针)
var slice: []i32 = arr[2..5];  // 长度 3 的切片
// 内存布局: [ptr: usize, len: usize]

// 4. C 指针 (?*T, ?[*]T)
// 语义: 可为空的指针，用于 C 互操作
var c_ptr: ?*i32 = null;

// 5. 可选指针 (?*T)
// 语义: 可为空的 Zig 指针，使用 null 表示无效
var opt_ptr: ?*i32 = null;

// 指针属性
const PtrAttrs = struct {
    // const - 不可通过此指针修改
    const_ptr: *const i32 = &x;  // 只读指针

    // volatile - 每次访问都必须从内存读取
    const volatile_ptr: *volatile i32 = &x;

    // allowzero - 允许空指针 (用于内核/嵌入式)
    // var zero_ptr: *allowzero i32 = @ptrFromInt(0);
};

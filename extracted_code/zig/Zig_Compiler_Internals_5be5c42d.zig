//
// Auto-generated from: Zig\Zig_Compiler_Internals.md
// Line: 120
// Language: zig
// Block ID: 5be5c42d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const AirInst = struct {
    tag: enum {
        // 常量
        constant,

        // 算术
        add, sub, mul, div, mod,

        // 内存
        alloc, store, load,
        arg, ret, ret_void,

        // 控制流
        br, cond_br, switch_br,

        // 调用
        call,

        // 类型转换
        intcast, floatcast,

        // 指针
        ptr_elem_ptr, ptr_field_ptr,

        // 聚合
        array_elem_val, struct_field_val,
    },
    data: union { ... },
};

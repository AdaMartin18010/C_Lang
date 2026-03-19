//
// Auto-generated from: Zig\Zig_Compiler_Internals.md
// Line: 99
// Language: zig
// Block ID: ca8cbf7f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const Type = union(enum) {
    int: struct { signedness: Signedness, bits: u16 },
    float: struct { bits: u16 },
    pointer: Pointer,
    array: struct { len: u64, elem_type: *Type },
    slice: struct { elem_type: *Type },
    struct_type: Struct,
    optional: struct { child_type: *Type },
    error_union: struct { error_set: *Type, payload: *Type },
    function: Function,
    // ...
};

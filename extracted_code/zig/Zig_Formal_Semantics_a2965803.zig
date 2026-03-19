//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 145
// Language: zig
// Block ID: a2965803
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 类型系统实现

// 基本类型
const Type = union(enum) {
    int: IntType,
    float: FloatType,
    pointer: PointerType,
    array: ArrayType,
    struct_type: StructType,
    error_union: ErrorUnionType,
    comptime_int,
    comptime_float,
    void,
};

const IntType = struct {
    signedness: Signedness,
    bits: u16,
};

const PointerType = struct {
    size: Size,  // One, Many, C, Slice
    is_const: bool,
    is_volatile: bool,
    pointee_type: *Type,
};

// 类型检查函数
fn checkTypes(expr: *Expr, ctx: *TypeContext) TypeError!Type {
    switch (expr.*) {
        .integer_literal => |n| {
            if (n.fitsIn(i32)) return .{ .int = .{ .signedness = .signed, .bits = 32 } };
            return .{ .int = .{ .signedness = .signed, .bits = 64 } };
        },
        .add => |bin| {
            const left = try checkTypes(bin.left, ctx);
            const right = try checkTypes(bin.right, ctx);

            // 类型一致性检查
            if (!typesEqual(left, right)) {
                return TypeError.TypeMismatch;
            }

            // 整数提升
            return promoteType(left, right);
        },
        // ...
    }
}

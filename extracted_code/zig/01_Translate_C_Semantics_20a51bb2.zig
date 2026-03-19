//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 240
// Language: zig
// Block ID: 20a51bb2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 类型 → Zig 类型转换核心函数

fn transQualType(c: *Context, qt: clang.QualType, loc: clang.SourceLocation) !ZigType {
    const ty = qt.getTypePtr();

    switch (ty.getTypeClass()) {
        .Builtin => return transBuiltinType(c, ty.cast(.Builtin).?),
        .Pointer => return transPointerType(c, ty.cast(.Pointer).?, loc),
        .Record => return transRecordType(c, ty.cast(.Record).?),
        .Enum => return transEnumType(c, ty.cast(.Enum).?),
        .Array => return transArrayType(c, ty.cast(.Array).?),
        .FunctionProto => return transFunctionType(c, ty.cast(.FunctionProto).?),
        .Typedef => return transTypedefType(c, ty.cast(.Typedef).?),
        .Elaborated => return transElaboratedType(c, ty.cast(.Elaborated).?),
        .Paren => return transParenType(c, ty.cast(.Paren).?),
        .Decayed => return transDecayedType(c, ty.cast(.Decayed).?),
        .Attributed => return transAttributedType(c, ty.cast(.Attributed).?),
        // ... 其他类型
    }
}

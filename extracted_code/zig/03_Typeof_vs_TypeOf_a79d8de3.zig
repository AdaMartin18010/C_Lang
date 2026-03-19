//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 813
// Language: zig
// Block ID: a79d8de3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 指针类型推导

fn zigPointerTypeof() void {
    var x: i32 = 42;
    const cx: i32 = 100;

    // 基本指针
    var p: *i32 = &x;
    var cp: *const i32 = &cx;

    // 指针推导
    const PType = @TypeOf(p);    // *i32
    const CPType = @TypeOf(cp);  // *const i32

    var p2: PType = &x;
    var cp2: CPType = &cx;

    // 多级指针
    var pp: **i32 = &p;
    const PPType = @TypeOf(pp);     // **i32
    const DerefType = @TypeOf(pp.*); // *i32
    const AddrType = @TypeOf(&p);   // **i32

    var pp2: PPType = &p;
    _ = pp2;

    // 可选指针
    var opt_p: ?*i32 = &x;
    const OptType = @TypeOf(opt_p);  // ?*i32
    var opt_p2: OptType = null;
    _ = opt_p2;

    // 多元素指针
    var many: [*]i32 = undefined;
    const ManyType = @TypeOf(many);  // [*]i32
    var many2: ManyType = undefined;
    _ = many2;
}

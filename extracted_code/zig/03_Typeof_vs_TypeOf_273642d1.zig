//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 598
// Language: zig
// Block ID: 273642d1
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig "限定符"等价物

fn zigQualifierEquivalents() void {
    // ========== const 等价物 ==========

    // Zig 使用 var/const 声明区分可变性
    const ci: i32 = 42;           // 常量 (类似 C 的 const)
    var vi: i32 = 100;            // 变量

    // 指针可变性
    var ptr: *i32 = &vi;          // 可变指针 (可修改指向的值)
    const cptr: *const i32 = &ci; // 只读指针 (类似 C 的 const int*)

    // 指针本身也可以是 const
    const ptr_const: *i32 = &vi;  // 指针变量是 const (类似 int* const)
    // ptr_const = &vi;           // 错误：不能修改 const 变量

    // ========== volatile 等价物 ==========

    // Zig 使用 volatile 关键字
    var vol_ptr: *volatile i32 = undefined;  // volatile 指针
    // 每次访问都会被编译器保留

    // ========== restrict 等价物 ==========

    // Zig 没有 restrict，编译器自动优化
    // 可以使用 @setRuntimeSafety(false) 等提示

    // ========== Atomic 等价物 ==========

    var ai: std.atomic.Atomic(i32) = std.atomic.Atomic(i32).init(0);
    // 或使用原子操作内置函数
}

// ========== 类型推导中的可变性 ==========

fn typeofMutability() void {
    const x: i32 = 42;
    var y: i32 = 100;

    // @TypeOf 推导类型，不保留 var/const 属性
    const Tx = @TypeOf(x);  // i32
    const Ty = @TypeOf(y);  // i32 (也是 i32，不是可变类型)

    // 如果需要可变性，显式声明
    var vx: Tx = 10;        // var 决定可变性
    const vy: Ty = 20;      // const 决定不可变性

    _ = .{ vx, vy };
}

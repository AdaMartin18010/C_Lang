//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 736
// Language: zig
// Block ID: e3eaf488
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 数组类型推导

fn zigArrayTypeof() void {
    var arr: [10]i32 = undefined;
    const carr = [5]i32{ 1, 2, 3, 4, 5 };
    var matrix: [3][4]i32 = undefined;

    // 基本数组推导
    const ArrType = @TypeOf(arr);      // [10]i32
    const CArrType = @TypeOf(carr);    // [5]i32

    var arr2: ArrType = undefined;
    var carr2: CArrType = undefined;

    // 数组元素推导
    const ElemType = @TypeOf(arr[0]);  // i32
    const CElemType = @TypeOf(carr[0]); // i32 (值类型，不保留 const)

    var elem: ElemType = 0;
    _ = elem;

    // 多维数组推导
    const MatType = @TypeOf(matrix);       // [3][4]i32
    const RowType = @TypeOf(matrix[0]);    // [4]i32
    const ItemType = @TypeOf(matrix[0][0]); // i32

    var mat2: MatType = undefined;
    var row: RowType = undefined;
    var item: ItemType = 0;

    _ = .{ arr2, carr2, mat2, row, item };
}

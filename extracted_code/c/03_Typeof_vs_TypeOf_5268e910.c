/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 701
 * Language: c
 * Block ID: 5268e910
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 数组类型推导

void c23_array_typeof(void) {
    int arr[10];
    const int carr[5] = {1, 2, 3, 4, 5};
    int matrix[3][4];

    // 基本数组推导
    typeof(arr) arr2;           // int[10] - 完整数组类型
    typeof(carr) carr2;         // const int[5]

    // sizeof 验证
    static_assert(sizeof(arr2) == sizeof(int[10]), "");
    static_assert(sizeof(carr2) == sizeof(const int[5]), "");

    // 数组元素推导
    typeof(arr[0]) elem;        // int
    typeof(carr[0]) celem;      // const int

    // 多维数组推导
    typeof(matrix) mat2;        // int[3][4]
    typeof(matrix[0]) row;      // int[4]
    typeof(matrix[0][0]) item;  // int

    // typeof_unqual 效果
    typeof_unqual(carr) uarr;   // int[5] - 移除 const

    // 数组指针
    int (*arr_ptr)[10];
    typeof(arr_ptr) ap2;        // int (*)[10]
    typeof(*arr_ptr) arr3;      // int[10]
}

/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 316
 * Language: c
 * Block ID: 5ab77844
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void example(int n, int m) {
    int vla2d[n][m];

    size_t dim1 = _Countof(vla2d);      // 返回 n
    size_t dim2 = _Countof(vla2d[0]);   // 返回 m

    // 使用VLA的大小时需要注意生命周期
    // vla2d 离开作用域后，相关信息不再有效
}

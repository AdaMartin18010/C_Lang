/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\01_Relocation_Group_Theory.md
 * Line: 456
 * Language: c
 * Block ID: 23947657
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：跨文件的全局变量初始化顺序不确定
// file1.c
extern int y;
int x = y + 1;  // y可能未初始化！

// file2.c
extern int x;
int y = x + 1;

// 解决：使用函数返回静态局部变量
int& get_x() {
    static int x = get_y() + 1;
    return x;
}

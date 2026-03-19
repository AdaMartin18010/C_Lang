/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 370
 * Language: c
 * Block ID: 61511f6f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 变长数组需要在运行时分配栈空间
void vla_function(int n) {
    int vla[n];  // 运行时确定大小

    // 汇编伪代码：
    // mov eax, n
    // shl eax, 2          ; * sizeof(int)
    // sub rsp, rax        ; 动态分配
    // and rsp, -16        ; 16字节对齐

    for (int i = 0; i < n; i++) {
        vla[i] = i;
    }

    // 函数返回时自动释放
}

/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 565
 * Language: c
 * Block ID: ffc8e66f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器如何计算栈帧大小

void example_frame(void) {
    int a;           // 4字节，偏移 -4
    double b;        // 8字节，偏移 -16（对齐到8字节）
    char c;          // 1字节，偏移 -17
    int d[10];       // 40字节，偏移 -64（对齐到16字节）
    // 填充：7字节到 -64

    // 总栈帧大小：64字节（16字节对齐）
    // 加上保存的寄存器：
    // - 返回地址：8字节
    // - 旧RBP：8字节
    // - 被调用者保存寄存器：按需
}

// GCC可以输出栈使用量
// -fstack-usage
// 生成.su文件显示每个函数的栈使用

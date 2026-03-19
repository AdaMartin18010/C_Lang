/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 420
 * Language: c
 * Block ID: c614e069
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 存储期类别
void example(void) {
    auto int a = 1;        // 自动存储期（默认，可省略）
    static int b = 2;      // 静态存储期，持久存在
    extern int c;          // 外部链接
    register int d = 4;    // 建议存储在寄存器（C11废弃）
}

// 线程存储期（C11）
_Thread_local int thread_local_var;  // 每个线程独立

// 作用域示例
static int file_scope;     // 文件作用域，内部链接

void func(void) {
    int block_scope;       // 块作用域

    {
        int inner_scope;   // 嵌套块作用域，隐藏外部同名变量
    }
}

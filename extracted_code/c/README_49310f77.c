/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\README.md
 * Line: 443
 * Language: c
 * Block ID: 49310f77
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 非 PIC 代码 (x86-64)
// mov global_var(%rip), %eax  直接访问

// PIC 代码 - 通过 GOT 访问全局变量
// 编译: gcc -fPIC -S test.c

static int local_var = 10;  // 内部符号，不需要 GOT
extern int shared_var;       // 外部符号，需要 GOT

int get_shared() {
    return shared_var;       // 通过 GOT 访问
}

// 生成的汇编 (简化):
// get_shared:
//     movq shared_var@GOTPCREL(%rip), %rax  # 获取 GOT 条目地址
//     movl (%rax), %eax                      # 解引用获取值
//     ret

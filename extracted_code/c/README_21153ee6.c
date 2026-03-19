/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 52
 * Language: c
 * Block ID: 21153ee6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C源代码
int add(int a, int b) {
    return a + b;
}

// 生成的x86-64汇编 (GCC -O2)
add:
    lea eax, [rdi + rsi]    # 使用LEA进行加法，结果放入EAX
    ret

// 生成的ARM64汇编
add:
    add w0, w0, w1          # W0 = W0 + W1
    ret

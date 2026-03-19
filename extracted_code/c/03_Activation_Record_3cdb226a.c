/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 527
 * Language: c
 * Block ID: 3cdb226a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 尾调用优化（TCO）
// 函数最后操作是调用另一个函数时，可以复用当前栈帧

// 无尾调用优化
int factorial_recursive(int n) {
    if (n <= 1) return 1;
    return n * factorial_recursive(n - 1);  // 需要保存n
}

// 尾递归形式
int factorial_tail(int n, int acc) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);  // 尾调用
}

// 汇编对比（尾调用优化后）
/*
无优化：
    push rbp
    mov rbp, rsp
    sub rsp, 16
    ...
    call factorial_recursive
    add rsp, 16
    pop rbp
    ret

有TCO：
    ; 复用当前栈帧
    mov edi, n-1
    mov esi, n*acc
    jmp factorial_tail  ; 不是call！
*/

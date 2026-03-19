/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete\01_RV32I_Single_Cycle.md
 * Line: 1336
 * Language: c
 * Block ID: 45b5fda1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C程序: 计算数组元素之和
int sum_array(int arr[], int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum = sum + arr[i];
    }
    return sum;
}

// 等效的RISC-V汇编
// x10 = arr (数组基地址)
// x11 = n (数组长度)
// x5 = sum (累加和)
// x6 = i (循环计数器)

sum_array:
    addi x5, x0, 0          # sum = 0
    addi x6, x0, 0          # i = 0
loop:
    bge  x6, x11, done      # if i >= n, goto done
    slli x7, x6, 2          # x7 = i * 4 (字节偏移)
    add  x7, x10, x7        # x7 = arr + i*4 (元素地址)
    lw   x8, 0(x7)          # x8 = arr[i]
    add  x5, x5, x8         # sum = sum + arr[i]
    addi x6, x6, 1          # i++
    jal  x0, loop           # goto loop
done:
    addi x10, x5, 0         # return sum (放入x10)
    jalr x0, 0(x1)          # return

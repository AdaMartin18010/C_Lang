/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\03_Memory_Circuits\02_Register_File.md
 * Line: 880
 * Language: c
 * Block ID: 041090f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C语言代码示例
int add(int a, int b) {
    int result = a + b;
    return result;
}

void caller() {
    int x = 10;
    int y = 20;
    int z = add(x, y);
}

/*
 * 编译后的汇编（RISC-V示例）及寄存器映射：
 *
 * add函数:
 * add:
 *     add a0, a0, a1      # a0 = a0 + a1 (result存入a0)
 *     ret                 # 返回 (jr ra)
 *
 * 寄存器映射:
 * ┌─────────────────────────────────────────────────────────────┐
 * │  C变量        汇编寄存器    物理寄存器文件位置                  │
 * │  ─────────────────────────────────────────────────────────  │
 * │  参数a        a0           x10 → registers[10]              │
 * │  参数b        a1           x11 → registers[11]              │
 * │  result       a0           x10 → registers[10]              │
 * │  返回地址     ra           x1  → registers[1]               │
 * └─────────────────────────────────────────────────────────────┘
 *
 * caller函数:
 * caller:
 *     addi sp, sp, -16        # 分配栈帧
 *     sw   ra, 12(sp)         # 保存返回地址
 *     sw   s0, 8(sp)          # 保存帧指针
 *     addi s0, sp, 16         # 设置帧指针
 *
 *     li   a0, 10             # x = 10 (存入a0)
 *     li   a1, 20             # y = 20 (存入a1)
 *     call add                # 调用add
 *     sw   a0, -12(s0)        # z = result (从a0存入栈)
 *
 *     lw   ra, 12(sp)         # 恢复返回地址
 *     lw   s0, 8(sp)          # 恢复帧指针
 *     addi sp, sp, 16         # 释放栈帧
 *     ret
 *
 * 寄存器映射:
 * ┌─────────────────────────────────────────────────────────────┐
 * │  C变量        汇编寄存器    说明                              │
 * │  ─────────────────────────────────────────────────────────  │
 * │  x            a0           临时存储，传递参数前               │
 * │  y            a1           临时存储，传递参数前               │
 * │  z            a0           函数返回后存储结果                 │
 * │  sp           x2           栈指针                             │
 * │  s0           x8           帧指针                             │
 * │  ra           x1           返回地址                           │
 * └─────────────────────────────────────────────────────────────┘
 */

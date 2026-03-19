/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\01_Rocket_Chip_Case_Study.md
 * Line: 673
 * Language: c
 * Block ID: dad452ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// startup.s - RISC-V启动汇编
.section .text.start
.global _start
.global _trap_vector

_start:
    // 关闭中断
    csrrc x0, mstatus, 0x8

    // 设置栈指针
    la sp, _stack_top

    // 设置陷阱向量
    la t0, _trap_vector
    csrw mtvec, t0

    // 清除BSS段
    la t0, _bss_start
    la t1, _bss_end
    bge t0, t1, 2f
1:
    sd x0, 0(t0)
    addi t0, t0, 8
    blt t0, t1, 1b
2:

    // 初始化数据段
    la t0, _data_start
    la t1, _data_end
    la t2, _data_load_addr
    bge t0, t1, 4f
3:
    ld t3, 0(t2)
    sd t3, 0(t0)
    addi t0, t0, 8
    addi t1, t1, 8
    addi t2, t2, 8
    blt t0, t1, 3b
4:

    // 调用main
    call main

    // 退出
    call _exit

_trap_vector:
    // 保存上下文
    addi sp, sp, -256
    sd x1, 0(sp)
    sd x2, 8(sp)
    // ... 保存所有寄存器

    // 调用陷阱处理函数
    call trap_handler

    // 恢复上下文
    ld x1, 0(sp)
    ld x2, 8(sp)
    // ... 恢复所有寄存器
    addi sp, sp, 256

    mret

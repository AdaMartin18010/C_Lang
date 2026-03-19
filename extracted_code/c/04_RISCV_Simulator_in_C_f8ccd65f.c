/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete\04_RISCV_Simulator_in_C.md
 * Line: 1056
 * Language: c
 * Block ID: f8ccd65f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

//////////////////////////////////////////////////////////////////////////////
// test_programs.c - 测试程序合集
//////////////////////////////////////////////////////////////////////////////

// 程序1: Hello World
const u32 hello_world[] = {
    // 在内存0x100处存储字符串 "Hello, World!\n"
    // x3 = 0x100 (字符串地址)
    // 系统调用: print_string (syscall 2)
    0x100001b7,  // lui  x3, 0x10000
    0x00200193,  // addi x3, x0, 2     # syscall number
    0x10000537,  // lui  x10, 0x10000  # string address
    0x0001a023,  // sw   x3, 0(x3)     # set syscall
    0x00000073,  // ecall
    0x00000073,  // ecall (exit)
};

// 程序2: 求和 1到100
const u32 sum_1_to_100[] = {
    0x06400513,  // addi x10, x0, 100  # n = 100
    0x00000593,  // addi x11, x0, 0    # sum = 0
    0x00100613,  // addi x12, x0, 1    # i = 1
    // loop:
    0x00a64863,  // blt  x12, x10, cont
    0x00058613,  // addi x12, x11, 0   # return sum
    0x00c00513,  // addi x10, x12, 0
    0x00000073,  // ecall
    // cont:
    0x00c585b3,  // add  x11, x11, x12 # sum += i
    0x00160613,  // addi x12, x12, 1   # i++
    0xfe0006e3,  // beq  x0, x0, loop
};

// 程序3: 快速排序 (简化版)
// 在内存中排序数组
const u32 quicksort[] = {
    // 简化实现, 使用冒泡排序
    // x10 = 数组起始地址
    // x11 = 数组长度
    // 外层循环: for i = 0 to n-1
    // 内层循环: for j = 0 to n-i-1
    // 如果 a[j] > a[j+1], 交换

    // 初始化
    0x20000537,  // lui  x10, 0x20000  # array addr
    0x00a00593,  // addi x11, x0, 10   # n = 10
    0x00000613,  // addi x12, x0, 0    # i = 0
    // outer_loop:
    0x02b64463,  // blt  x12, x11, inner_init
    0x00000073,  // ecall (done)
    // inner_init:
    0x00000693,  // addi x13, x0, 0    # j = 0
    0x40c58633,  // sub  x12, x11, x12 # n - i
    0xfff60613,  // addi x12, x12, -1  # n - i - 1
    // inner_loop:
    0x02c6c463,  // blt  x13, x12, compare
    0x00160613,  // addi x12, x12, 1   # restore i
    0x00160613,  // addi x12, x12, 1   # i++
    0xfc0006e3,  // beq  x0, x0, outer_loop
    // compare:
    // ... (略)
    0x00168693,  // addi x13, x13, 1   # j++
    0xfe0006e3,  // beq  x0, x0, inner_loop
};

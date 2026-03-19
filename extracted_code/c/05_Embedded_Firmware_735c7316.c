/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\05_Embedded_Firmware.md
 * Line: 95
 * Language: c
 * Block ID: 735c7316
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// startup_stm32f4xx.s (简化版)
// 启动文件 - 汇编部分

.section .text.Reset_Handler
.global Reset_Handler

Reset_Handler:
    // 1. 初始化栈指针
    ldr sp, =_estack

    // 2. 复制.data段从Flash到RAM
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    b CopyDataInit

CopyDataLoop:
    ldr r3, [r2], #4
    str r3, [r0], #4

CopyDataInit:
    cmp r0, r1
    blo CopyDataLoop

    // 3. 清零.bss段
    ldr r0, =_sbss
    ldr r1, =_ebss
    mov r2, #0
    b ZeroBssInit

ZeroBssLoop:
    str r2, [r0], #4

ZeroBssInit:
    cmp r0, r1
    blo ZeroBssLoop

    // 4. 调用SystemInit (时钟配置)
    bl SystemInit

    // 5. 调用main函数
    bl main

    // 6. main返回后的处理
    b InfiniteLoop

InfiniteLoop:
    b InfiniteLoop

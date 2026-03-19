/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 191
 * Language: c
 * Block ID: 40011d14
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 自定义链接器脚本 - 精确控制内存布局
 * 针对: STM32F103C8 (20KB RAM, 64KB Flash)
 * ============================================================================ */

/* memory.ld */
MEMORY
{
    /* Flash - 只读 */
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 64K

    /* RAM - 读写 */
    RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 20K

    /* CCM/Core Coupled Memory (如果有) */
    CCM (rw)        : ORIGIN = 0x10000000, LENGTH = 0K  /* F103无CCM */
}

SECTIONS
{
    /* 代码段 - 放入Flash */
    .text :
    {
        . = ALIGN(4);
        _stext = .;           /* 代码起始 */
        KEEP(*(.isr_vector))  /* 中断向量表 */
        *(.text*)             /* 代码 */
        *(.rodata*)           /* 只读数据 */
        *(.glue_7)            /* ARM/Thumb glue */
        *(.glue_7t)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;           /* 代码结束 */
    } > FLASH

    /* 初始化数据 - 运行时放入RAM，加载时从Flash复制 */
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;           /* 数据起始 */
        *(.data*)
        . = ALIGN(4);
        _edata = .;           /* 数据结束 */
    } > RAM AT > FLASH

    /* 未初始化数据 - 运行时清零 */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;            /* BSS起始 */
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;            /* BSS结束 */
    } > RAM

    /* 栈底初始化值 */
    __stack_bottom = ORIGIN(RAM) + LENGTH(RAM);
}

/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 1575
 * Language: c
 * Block ID: 7aa65b7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * GCC编译器优化选项 - 最小化代码体积
 * ============================================================================ */

/* Makefile 优化配置 */
/*
# 优化级别 - 代码大小
CFLAGS += -Os                    # 优化代码大小 (不是-O2或-O3)
CFLAGS += -Oz                    # 极致大小优化 (GCC 4.8+)

# 去除未使用代码
CFLAGS += -ffunction-sections    # 每个函数放入独立段
CFLAGS += -fdata-sections        # 每个数据放入独立段
LDFLAGS += -Wl,--gc-sections     # 链接时删除未使用段

# 链接时优化
CFLAGS += -flto                  # 链接时优化
LDFLAGS += -flto

# 禁用不必要功能
CFLAGS += -fno-builtin-printf    # 不使用内置printf
CFLAGS += -fno-unwind-tables     # 禁用异常展开表
CFLAGS += -fno-asynchronous-unwind-tables
CFLAGS += -fno-exceptions        # C++禁用异常
CFLAGS += -fno-rtti              # C++禁用RTTI
CFLAGS += -fno-stack-protector   # 禁用栈保护
CFLAGS += -fno-ident             # 去除编译器标识

# ARM特定优化
CFLAGS += -mthumb                # Thumb指令 (更紧凑)
CFLAGS += -mcpu=cortex-m0        # 针对特定CPU优化
CFLAGS += -mlong-calls           # 允许长跳转

# 调试信息控制
CFLAGS += -g0                    # 无调试信息 (发布版本)
# 或 CFLAGS += -g3                # 完整调试信息 (调试版本)

# 警告选项 (帮助发现优化机会)
CFLAGS += -Wall -Wextra -Wunused
CFLAGS += -Wunused-function
CFLAGS += -Wunused-variable
CFLAGS += -Wunused-parameter

# 链接器优化
LDFLAGS += -Wl,--relax           # 优化跳转和调用
LDFLAGS += -Wl,--strip-all       # 去除所有符号
LDFLAGS += -Wl,--strip-debug     # 去除调试信息
LDFLAGS += -nostartfiles         # 自定义启动文件
LDFLAGS += -nodefaultlibs        # 不使用默认库
*/

/* 链接脚本优化 */
/*
SECTIONS
{
    .text :
    {
        *(.text.startup*)      /* 启动代码 */
        *(.text.interrupt*)    /* 中断处理 */
        *(.text*)              /* 其他代码 */

        /* 合并常量到.text */
        *(.rodata*)
        *(.rodata1)
    } > FLASH

    /* 去除.comment和.note段 */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.gnu.*)
    }
}
*/

/* 检查代码大小的方法 */
/*
# 查看各段大小
arm-none-eabi-size firmware.elf
   text    data     bss     dec     hex filename
   8456     256     512    9224    2408 firmware.elf

# 查看详细符号大小
arm-none-eabi-nm --print-size --size-sort --radix=d firmware.elf

# 生成内存映射
arm-none-eabi-ld -Map=firmware.map ...

# 反汇编查看代码
arm-none-eabi-objdump -d firmware.elf > firmware.asm
*/

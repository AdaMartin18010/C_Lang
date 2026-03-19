/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 61
 * Language: c
 * Block ID: 2fec63cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// hal.h - 硬件抽象层接口
#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// 编译器屏障
#define barrier() __asm__ __volatile__("" ::: "memory")

// 内存屏障
#define mb() __asm__ __volatile__("fence iorw, iorw" ::: "memory")
#define rmb() __asm__ __volatile__("fence ir, ir" ::: "memory")
#define wmb() __asm__ __volatile__("fence ow, ow" ::: "memory")

// 中断控制
#define local_irq_disable() __asm__ __volatile__("csrc mstatus, 8" ::: "memory")
#define local_irq_enable()  __asm__ __volatile__("csrs mstatus, 8" ::: "memory")
#define local_irq_save(flags) \
    do { \
        flags = read_csr(mstatus); \
        local_irq_disable(); \
    } while(0)
#define local_irq_restore(flags) \
    write_csr(mstatus, flags)

// 寄存器访问
#define read_csr(reg) ({ \
    unsigned long __tmp; \
    __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp; \
})

#define write_csr(reg, val) \
    __asm__ __volatile__("csrw " #reg ", %0" :: "r"(val))

#define set_csr(reg, bit) \
    __asm__ __volatile__("csrs " #reg ", %0" :: "r"(bit))

#define clear_csr(reg, bit) \
    __asm__ __volatile__("csrc " #reg ", %0" :: "r"(bit))

// 系统控制
typedef enum {
    SYS_RESET = 0,
    SYS_SHUTDOWN = 1,
    SYS_SLEEP = 2,
} sys_cmd_t;

void system_control(sys_cmd_t cmd);
uint64_t get_time_ns(void);
void udelay(uint32_t us);

#endif

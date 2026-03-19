/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\01_MMU_Architecture.md
 * Line: 941
 * Language: c
 * Block ID: 30664160
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 特权级与内存访问权限的关系
 * 展示不同特权级如何使用MMU
 */

#include <stdio.h>
#include <stdint.h>

/*
 * x86-64页表项中的U/S位控制用户/超级用户访问
 */
#define PTE_PRESENT     (1ULL << 0)
#define PTE_RW          (1ULL << 1)   // 读写位
#define PTE_US          (1ULL << 2)   // 用户/超级用户位
#define PTE_ACCESSED    (1ULL << 5)
#define PTE_DIRTY       (1ULL << 6)
#define PTE_GLOBAL      (1ULL << 8)
#define PTE_NX          (1ULL << 63)  // 执行禁用

/*
 * 页表项权限组合
 */
typedef enum {
    // 内核页 (U/S=0): 只有Ring 0-2可以访问
    KERNEL_RW = PTE_PRESENT | PTE_RW,           // 内核读写
    KERNEL_RO = PTE_PRESENT,                     // 内核只读
    KERNEL_RX = PTE_PRESENT | PTE_RW,            // 内核可执行 (NX=0)

    // 用户页 (U/S=1): Ring 3可以访问
    USER_RW   = PTE_PRESENT | PTE_RW | PTE_US,  // 用户读写
    USER_RO   = PTE_PRESENT | PTE_US,            // 用户只读
    USER_RX   = PTE_PRESENT | PTE_US,            // 用户可执行 (NX=0)
    USER_NONE = PTE_PRESENT | PTE_US,            // 用户存在但不可访问

    // 不可执行页 (NX=1)
    KERNEL_DATA = PTE_PRESENT | PTE_RW | PTE_NX,       // 内核数据
    USER_DATA   = PTE_PRESENT | PTE_RW | PTE_US | PTE_NX, // 用户数据
} pte_permissions_t;

/*
 * 特权级检查伪代码
 */
int check_access_privilege(uint64_t pte, int cpl, int access_type) {
    /*
     * cpl: Current Privilege Level (0=内核, 3=用户)
     * access_type: 0=读, 1=写, 2=执行
     */

    int present = pte & PTE_PRESENT;
    int rw = pte & PTE_RW;
    int us = pte & PTE_US;
    int nx = pte & PTE_NX;

    // 检查存在位
    if (!present) {
        return -1;  // 页不存在异常
    }

    // 检查用户/超级用户权限
    if (cpl == 3) {
        // 用户模式访问
        if (!us) {
            return -2;  // 特权级违规 - 用户访问内核页
        }
    }
    // 内核模式可以访问所有页 (SMAP除外)

    // 检查读写权限
    if (access_type == 1 && !rw) {
        return -3;  // 写保护异常
    }

    // 检查执行权限
    if (access_type == 2 && nx) {
        return -4;  // 执行禁用异常
    }

    return 0;  // 访问允许
}

/*
 * C语言中的特权级概念
 * 虽然C代码运行在特定特权级，但可以通过系统调用切换
 */
void privilege_level_example(void) {
    int stack_var = 42;

    printf("在用户模式 (Ring 3 / EL0 / U-mode) 运行\n");
    printf("栈变量地址: %p\n", (void*)&stack_var);

    /*
     * 尝试访问内核地址会导致段错误
     * 因为MMU会阻止用户模式访问内核页
     */
    // int *kernel_ptr = (int*)0xFFFFFFFF80000000ULL;
    // *kernel_ptr = 0;  // 这将触发Page Fault!

    /*
     * 系统调用会切换到内核模式
     * 例如: write() 系统调用
     */
    write(1, "Hello\n", 6);  // 内部执行 syscall 指令

    /*
     * syscall指令执行流程:
     * 1. 用户程序在Ring 3执行syscall
     * 2. CPU切换到Ring 0 (内核模式)
     * 3. 进入内核syscall入口
     * 4. 内核执行write操作 (可以访问所有内存)
     * 5. sysret指令返回Ring 3
     */
}

/*
 * Linux内核中的特权级处理
 */
#ifdef __KERNEL__

/*
 * 内核中检查当前特权级
 */
static inline int current_privilege_level(void) {
    // 在Linux内核中，我们总是在内核模式运行
    // 但用户上下文通过thread_info区分
    return user_mode(current_pt_regs()) ? 3 : 0;
}

/*
 * 设置页表项权限
 */
static inline void set_pte_permissions(pte_t *pte, pgprot_t prot) {
    /*
     * pgprot_t包含页表项的权限位
     * __PAGE_KERNEL: 内核页权限
     * __PAGE_USER: 用户页权限
     */
    set_pte(pte, pfn_pte(page_to_pfn(page), prot));
}

/*
 * 用户空间访问检查
 * 内核代码在访问用户空间指针时需要小心
 */
int access_user_space(void __user *user_ptr, size_t size) {
    // 检查用户指针是否有效
    if (!access_ok(user_ptr, size)) {
        return -EFAULT;
    }

    // 使用copy_from_user/copy_to_user安全访问
    // 这些函数会处理可能的页错误
    return 0;
}

#endif /* __KERNEL__ */

int main() {
    printf("=== 特权级与MMU ===\n");
    printf("当前运行模式: 用户模式 (受MMU保护)\n");

    privilege_level_example();

    return 0;
}

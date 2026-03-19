/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\01_MMU_Architecture.md
 * Line: 1112
 * Language: c
 * Block ID: 2a56061d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核页表管理核心代码分析
 * 基于x86-64架构
 */

#ifndef __ASSEMBLY__

#include <linux/mm_types.h>
#include <linux/pgtable.h>
#include <asm/page.h>

/*
 * Linux使用四级页表 (x86-64):
 * PGD (Page Global Directory)  - 页全局目录
 * P4D (Page 4th Directory)     - 第四级目录 (5级页表中使用)
 * PUD (Page Upper Directory)   - 页上级目录
 * PMD (Page Middle Directory)  - 页中间目录
 * PTE (Page Table Entry)       - 页表项
 */

/*
 * 页表项操作宏
 */
#define pgd_index(address) (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define pud_index(address) (((address) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define pmd_index(address) (((address) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define pte_index(address) (((address) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

/*
 * 页表遍历函数
 */
static inline pte_t *pte_offset_kernel(pmd_t *pmd, unsigned long address) {
    return (pte_t *)pmd_page_vaddr(*pmd) + pte_index(address);
}

/*
 * Linux页表结构 (简化视图)
 */
struct mm_struct {
    pgd_t *pgd;                    // 页全局目录指针
    atomic_t mm_users;             // 使用计数
    atomic_t mm_count;             // 引用计数

    unsigned long start_code;      // 代码段起始
    unsigned long end_code;        // 代码段结束
    unsigned long start_data;      // 数据段起始
    unsigned long end_data;        // 数据段结束
    unsigned long start_brk;       // 堆起始
    unsigned long brk;             // 堆结束
    unsigned long start_stack;     // 栈起始

    // ... 更多字段
};

/*
 * 页表项标志位 (Linux抽象)
 */
#define _PAGE_PRESENT   (1UL << 0)   // 页存在
#define _PAGE_RW        (1UL << 1)   // 可写
#define _PAGE_USER      (1UL << 2)   // 用户可访问
#define _PAGE_PWT       (1UL << 3)   // 写透
#define _PAGE_PCD       (1UL << 4)   // 缓存禁用
#define _PAGE_ACCESSED  (1UL << 5)   // 已访问
#define _PAGE_DIRTY     (1UL << 6)   // 脏页
#define _PAGE_PSE       (1UL << 7)   // 页大小扩展 (大页)
#define _PAGE_GLOBAL    (1UL << 8)   // 全局页
#define _PAGE_SOFTW1    (1UL << 9)   // 软件使用位1
#define _PAGE_SOFTW2    (1UL << 10)  // 软件使用位2
#define _PAGE_SOFTW3    (1UL << 11)  // 软件使用位3
#define _PAGE_PAT       (1UL << 12)  // PAT索引
#define _PAGE_NX        (1UL << 63)  // 执行禁用

/*
 * 标准页保护组合
 */
#define PAGE_NONE       __pgprot(0)
#define PAGE_READONLY   __pgprot(_PAGE_PRESENT)
#define PAGE_READONLY_EXEC __pgprot(_PAGE_PRESENT)
#define PAGE_RW         __pgprot(_PAGE_PRESENT | _PAGE_RW)
#define PAGE_RW_EXEC    __pgprot(_PAGE_PRESENT | _PAGE_RW)
#define PAGE_USER_READONLY __pgprot(_PAGE_PRESENT | _PAGE_USER)
#define PAGE_USER_RW    __pgprot(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER)

/*
 * 内核页表初始化
 */
void __init paging_init(void) {
    /*
     * 1. 初始化内核页表
     * 2. 设置直接映射 (linear mapping)
     * 3. 设置vmalloc区域
     * 4. 加载CR3寄存器
     */

    // 创建直接映射 (物理地址 -> 虚拟地址的线性映射)
    // 例如: 物理地址0x1000映射到虚拟地址0xFFFF800000001000

    // 设置内核文本段权限 (只读、可执行)
    // 设置内核数据段权限 (读写、不可执行)

    // 加载页表基址到CR3
    // write_cr3(__pa(init_top_pgt));
}

/*
 * 处理页错误
 */
int do_page_fault(struct pt_regs *regs, unsigned long error_code,
                  unsigned long address) {
    struct vm_area_struct *vma;
    struct mm_struct *mm;
    int fault;

    mm = current->mm;

    /*
     * error_code位定义:
     * bit 0: 0=页不存在, 1=保护错误
     * bit 1: 0=读访问, 1=写访问
     * bit 2: 0=内核模式, 1=用户模式
     * bit 3: 0=普通访问, 1=保留位违规
     * bit 4: 0=取指令, 1=数据访问
     */

    // 查找对应的VMA (虚拟内存区域)
    vma = find_vma(mm, address);
    if (!vma) {
        return SIGSEGV;  // 段错误 - 访问无效地址
    }

    // 检查访问权限
    if (!(vma->vm_flags & VM_READ) && (error_code & PF_WRITE)) {
        return SIGSEGV;  // 尝试写入只读区域
    }

    if (!(vma->vm_flags & VM_WRITE) && (error_code & PF_WRITE)) {
        return SIGSEGV;  // 写入无写权限区域
    }

    // 处理缺页
    fault = handle_mm_fault(vma, address,
                            error_code & PF_WRITE ? FAULT_FLAG_WRITE : 0);

    if (fault & VM_FAULT_OOM) {
        return SIGKILL;  // OOM killer
    }

    return 0;
}

/*
 * 处理缺页 (需求分页)
 */
int handle_mm_fault(struct vm_area_struct *vma, unsigned long address,
                    unsigned int flags) {
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    pgd = pgd_offset(vma->vm_mm, address);
    p4d = p4d_alloc(vma->vm_mm, pgd, address);
    if (!p4d)
        return VM_FAULT_OOM;

    pud = pud_alloc(vma->vm_mm, p4d, address);
    if (!pud)
        return VM_FAULT_OOM;

    pmd = pmd_alloc(vma->vm_mm, pud, address);
    if (!pmd)
        return VM_FAULT_OOM;

    // 处理PTE级别的缺页
    return handle_pte_fault(vma, address, pte, pmd, flags);
}

/*
 * 刷新TLB
 */
void flush_tlb_all(void) {
    // 刷新所有TLB条目
    // x86: 重新加载CR3或执行INVPCID
    __native_flush_tlb_global();
}

void flush_tlb_mm(struct mm_struct *mm) {
    // 刷新指定进程的TLB条目
    if (current->mm == mm) {
        __native_flush_tlb();
    }
}

void flush_tlb_page(struct vm_area_struct *vma, unsigned long addr) {
    // 刷新单个页的TLB条目
    __flush_tlb_one_user(addr);
}

/*
 * 修改页表项
 */
int set_memory_nx(unsigned long addr, int numpages) {
    // 将内存区域设置为不可执行
    // 用于保护内核数据区
    return change_page_attr_set_clr(&addr, numpages,
                                    __pgprot(_PAGE_NX), __pgprot(0),
                                    0, 0, NULL);
}

int set_memory_ro(unsigned long addr, int numpages) {
    // 将内存区域设置为只读
    // 用于保护内核代码区
    return change_page_attr_set_clr(&addr, numpages,
                                    __pgprot(0), __pgprot(_PAGE_RW),
                                    0, 0, NULL);
}

#endif /* __ASSEMBLY__ */

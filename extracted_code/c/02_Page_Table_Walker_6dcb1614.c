/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\02_Page_Table_Walker.md
 * Line: 1295
 * Language: c
 * Block ID: 6dcb1614
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核页错误处理流程
 * 对应硬件检测到的页错误
 */

#include <linux/mm.h>
#include <linux/sched.h>
#include <asm/traps.h>

/*
 * 页错误入口 (由硬件调用)
 * 当MMU检测到页错误时，自动跳转到这里
 */
void do_page_fault(struct pt_regs *regs, unsigned long error_code) {
    unsigned long address = read_cr2();  // 读取CR2获取错误虚拟地址
    struct task_struct *tsk = current;
    struct mm_struct *mm = tsk->mm;
    int fault;

    /*
     * 错误码解析
     */
    int present = error_code & PF_PRESENT;   // 页是否存在
    int write = error_code & PF_WRITE;       // 写访问
    int user = error_code & PF_USER;         // 用户模式
    int reserved = error_code & PF_RSVD;     // 保留位违规
    int instruction = error_code & PF_INSTR; // 指令获取

    printk(KERN_DEBUG "Page fault at 0x%lx, error_code: 0x%lx\n",
           address, error_code);

    /*
     * 情况1: 保留位违规 - 立即终止
     */
    if (reserved) {
        printk(KERN_ALERT "Reserved bit violation at 0x%lx\n", address);
        force_sig(SIGSEGV);
        return;
    }

    /*
     * 情况2: 内核空间页错误 (address >= TASK_SIZE_MAX)
     */
    if (unlikely(address >= TASK_SIZE_MAX)) {
        /*
         * 可能是vmalloc地址 - 需要更新内核页表
         */
        if (is_vmalloc_addr((void *)address)) {
            fault = vmalloc_fault(address);
            if (fault == 0)
                return;  // 成功处理
        }

        /*
         * 内核访问无效地址 - 内核恐慌或oops
         */
        if (user_mode(regs)) {
            // 用户程序通过系统调用传入坏指针
            force_sig(SIGSEGV);
        } else {
            // 内核bug
            die("Oops", regs, error_code);
        }
        return;
    }

    /*
     * 情况3: 用户空间页错误
     */
    if (user) {
        // 检查是否在内核中断上下文
        if (in_atomic() || !mm) {
            force_sig(SIGSEGV);
            return;
        }
    }

    /*
     * 查找对应的VMA (Virtual Memory Area)
     */
    struct vm_area_struct *vma = find_vma(mm, address);

    if (!vma) {
        // 地址不在任何VMA中 - 无效访问
        printk(KERN_DEBUG "No VMA for address 0x%lx\n", address);
        force_sig(SIGSEGV);
        return;
    }

    /*
     * 检查访问权限
     */
    if (write) {
        // 写访问
        if (!(vma->vm_flags & VM_WRITE)) {
            printk(KERN_DEBUG "Write to read-only region\n");
            force_sig(SIGSEGV);
            return;
        }
    } else if (instruction) {
        // 指令获取
        if (!(vma->vm_flags & VM_EXEC)) {
            printk(KERN_DEBUG "Execute from non-executable region\n");
            force_sig(SIGSEGV);
            return;
        }
    } else {
        // 读访问
        if (!(vma->vm_flags & (VM_READ | VM_EXEC))) {
            printk(KERN_DEBUG "Read from non-readable region\n");
            force_sig(SIGSEGV);
            return;
        }
    }

    /*
     * 处理缺页 (需求分页)
     */
    fault = handle_mm_fault(vma, address,
                            write ? FAULT_FLAG_WRITE : 0);

    if (fault & VM_FAULT_OOM) {
        // 内存不足
        pagefault_out_of_memory();
    } else if (fault & VM_FAULT_SIGBUS) {
        // 总线错误
        force_sig(SIGBUS);
    } else if (fault & VM_FAULT_SIGSEGV) {
        // 段错误
        force_sig(SIGSEGV);
    }

    /*
     * 成功处理，MMU TLB会在返回用户空间前自动更新
     * 指令会重新执行，这次会成功
     */
}

/*
 * 处理需求分页
 */
int handle_mm_fault(struct vm_area_struct *vma, unsigned long address,
                    unsigned int flags) {
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    /*
     * 获取或分配各级页表
     */
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

    /*
     * 处理PTE级别的缺页
     * 这可能是:
     * 1. 匿名页 - 分配零页或复制COW页
     * 2. 文件映射 - 从文件读取页
     * 3. 共享内存 - 连接共享页
     */
    return handle_pte_fault(vma, address, pmd, flags);
}

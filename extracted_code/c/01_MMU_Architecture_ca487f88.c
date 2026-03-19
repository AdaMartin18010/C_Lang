/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\01_MMU_Architecture.md
 * Line: 1336
 * Language: c
 * Block ID: ca487f88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 用户空间程序与MMU的交互示例
 * 展示mmap、mprotect、madvise等系统调用如何影响页表
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <setjmp.h>

static jmp_buf jump_buffer;
static volatile int page_fault_occurred = 0;

/*
 * 页错误信号处理程序
 */
void segfault_handler(int sig, siginfo_t *si, void *unused) {
    page_fault_occurred = 1;
    printf("捕获到页错误! 地址: %p\n", si->si_addr);
    longjmp(jump_buffer, 1);
}

/*
 * 演示: mmap创建映射
 */
void demonstrate_mmap(void) {
    printf("\n=== mmap演示 ===\n");

    size_t size = 4096;  // 4KB页

    // 创建匿名私有映射
    // 这会分配虚拟内存，但可能不立即分配物理页
    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        return;
    }

    printf("mmap返回地址: %p\n", addr);
    printf("此时尚未分配物理页 (需求分页)\n");

    // 第一次写入 - 触发缺页异常，分配物理页
    printf("第一次写入...\n");
    *(int*)addr = 42;  // 这里触发Page Fault，内核分配物理页
    printf("写入完成，物理页已分配\n");

    // 修改内存保护
    printf("\n修改保护为只读...\n");
    if (mprotect(addr, size, PROT_READ) == -1) {
        perror("mprotect");
    }

    // 尝试写入只读页
    if (setjmp(jump_buffer) == 0) {
        printf("尝试写入只读页...\n");
        *(int*)addr = 100;  // 这将触发SIGSEGV
        printf("写入成功 (不应该到这里)\n");
    } else {
        printf("预期中的页错误已捕获\n");
    }

    // 清理
    munmap(addr, size);
}

/*
 * 演示: 大页支持
 */
void demonstrate_hugepage(void) {
    printf("\n=== 大页(Huge Page)演示 ===\n");

    size_t hugepage_size = 2 * 1024 * 1024;  // 2MB大页

    // 尝试使用大页
    void *addr = mmap(NULL, hugepage_size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);

    if (addr == MAP_FAILED) {
        printf("大页分配失败 (可能需要预先配置大页)\n");

        // 使用透明大页 (THP)
        addr = mmap(NULL, hugepage_size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (addr != MAP_FAILED) {
            // 建议内核使用大页
            madvise(addr, hugepage_size, MADV_HUGEPAGE);
            printf("使用透明大页 (THP) 替代\n");
        }
    } else {
        printf("成功分配2MB大页: %p\n", addr);
    }

    if (addr != MAP_FAILED) {
        munmap(addr, hugepage_size);
    }
}

/*
 * 演示: 内存锁定 (mlock)
 */
void demonstrate_mlock(void) {
    printf("\n=== mlock演示 ===\n");

    size_t size = 4096;
    char *addr = malloc(size);

    if (!addr) {
        perror("malloc");
        return;
    }

    // 锁定内存，防止被交换出去
    // 这会强制分配物理页
    if (mlock(addr, size) == -1) {
        perror("mlock");
    } else {
        printf("内存已锁定，保证在物理内存中\n");

        // 写入数据
        strcpy(addr, "Locked memory");
        printf("数据: %s\n", addr);

        // 解锁
        munlock(addr, size);
        printf("内存已解锁\n");
    }

    free(addr);
}

/*
 * 演示: 内存建议 (madvise)
 */
void demonstrate_madvise(void) {
    printf("\n=== madvise演示 ===\n");

    size_t size = 4096 * 100;  // 100页
    char *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        return;
    }

    // 预读建议
    printf("给予MADV_WILLNEED建议\n");
    madvise(addr, size, MADV_WILLNEED);
    // 内核会预读这些页到内存

    // 顺序访问建议
    printf("给予MADV_SEQUENTIAL建议\n");
    madvise(addr, size, MADV_SEQUENTIAL);
    // 内核会优化预读，假设顺序访问

    // 随机访问建议
    printf("给予MADV_RANDOM建议\n");
    madvise(addr, size, MADV_RANDOM);
    // 内核会禁用预读

    // 不再访问建议
    printf("给予MADV_DONTNEED建议\n");
    madvise(addr, size, MADV_DONTNEED);
    // 内核可以释放这些页的物理内存

    munmap(addr, size);
}

/*
 * 演示: mincore - 检查页是否在内存中
 */
void demonstrate_mincore(void) {
    printf("\n=== mincore演示 ===\n");

    size_t size = 4096 * 10;  // 10页
    unsigned char vec[10];

    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        return;
    }

    // 检查哪些页在内存中
    if (mincore(addr, size, vec) == -1) {
        perror("mincore");
    } else {
        printf("页驻留状态:\n");
        for (int i = 0; i < 10; i++) {
            printf("  页%d: %s\n", i, (vec[i] & 1) ? "在内存中" : "不在内存中");
        }
    }

    // 访问第3页
    printf("\n访问第3页...\n");
    ((char*)addr)[3 * 4096] = 'x';

    // 再次检查
    if (mincore(addr, size, vec) == -1) {
        perror("mincore");
    } else {
        printf("访问后页驻留状态:\n");
        for (int i = 0; i < 10; i++) {
            printf("  页%d: %s\n", i, (vec[i] & 1) ? "在内存中" : "不在内存中");
        }
    }

    munmap(addr, size);
}

int main() {
    // 设置页错误处理程序
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segfault_handler;
    sigaction(SIGSEGV, &sa, NULL);

    printf("=== Linux用户空间与MMU交互 ===\n");

    demonstrate_mmap();
    demonstrate_hugepage();
    demonstrate_mlock();
    demonstrate_madvise();
    demonstrate_mincore();

    return 0;
}

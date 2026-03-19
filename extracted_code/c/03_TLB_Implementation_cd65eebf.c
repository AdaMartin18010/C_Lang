/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\03_TLB_Implementation.md
 * Line: 996
 * Language: c
 * Block ID: cd65eebf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * C语言指针本质上是虚拟地址
 * 程序员看到的地址都是虚拟地址，由MMU透明转换
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * C指针 = 虚拟地址
 */
void pointer_is_virtual_address(void) {
    int x = 42;
    int *ptr = &x;

    /*
     * ptr存储的是虚拟地址
     * 打印出来的地址是虚拟地址空间的地址
     * 实际的物理地址由MMU通过TLB/页表转换
     */
    printf("Virtual address of x: %p\n", (void*)ptr);
    printf("Value at that address: %d\n", *ptr);

    /*
     * 虚拟地址空间布局 (典型Linux x86-64):
     *
     * 0x0000_7FFF_FFFF_FFFF  用户空间上限 (128TB)
     *           ...
     * 0x0000_7F00_0000_0000  堆区域
     *           ...
     * 0x0000_0000_0040_0000  代码段
     * 0x0000_0000_0000_0000  NULL
     *
     * 注意: 这些地址都是虚拟的，实际的物理内存分布完全不同
     */
}

/*
 * 不同进程的相同虚拟地址
 */
void same_virtual_different_physical(void) {
    /*
     * 进程A和进程B可以有相同的虚拟地址，
     * 但映射到不同的物理地址
     */

    /*
     * 进程A:
     *   int *ptr = malloc(sizeof(int));
     *   // ptr = 0x00007F1234567000 (虚拟地址)
     *   // 映射到物理地址 0x00000012345000
     *
     * 进程B:
     *   int *ptr = malloc(sizeof(int));
     *   // ptr = 0x00007F1234567000 (相同的虚拟地址!)
     *   // 映射到物理地址 0x000000ABCDEF00 (不同的物理地址!)
     *
     * TLB使用ASID区分不同进程的相同虚拟地址
     */
}

/*
 * TLB对指针解引用的影响
 */
void tlb_and_pointer_dereference(void) {
    int array[1000];

    // 第一次访问 array[0] - 可能发生TLB未命中
    // 需要页表遍历，耗时较长
    array[0] = 1;

    // 后续访问 array[1..511] - TLB命中
    // 同一页内的访问，TLB条目已缓存
    for (int i = 1; i < 512; i++) {
        array[i] = i;  // TLB命中，快速
    }

    // 访问 array[512] - 可能TLB未命中 (跨越页边界)
    // 需要访问新的页表项
    array[512] = 512;

    /*
     * 优化提示: 顺序访问连续内存可以最大化TLB命中率
     * 随机访问或大跨度跳跃会导致更多TLB未命中
     */
}

/*
 * 大页与指针性能
 */
void huge_pages_and_pointers(void) {
    /*
     * 标准4KB页:
     * - 1GB内存需要 262,144 个页表项
     * - TLB只能缓存其中少量条目
     * - 大范围随机访问会导致大量TLB未命中
     */

    /*
     * 2MB大页:
     * - 1GB内存只需要 512 个页表项
     * - TLB可以缓存更多映射
     * - 更好的TLB命中率
     */

    /*
     * 1GB大页:
     * - 整个1GB只需要 1 个页表项
     * - 单个TLB条目覆盖整个1GB
     * - 最佳TLB效率
     */

    // 使用大页的代码示例
    // 需要特殊分配: mmap with MAP_HUGETLB
    void *huge_page = mmap(NULL, 2 * 1024 * 1024,
                           PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                           -1, 0);

    if (huge_page != MAP_FAILED) {
        int *array = (int*)huge_page;

        // 整个2MB范围内的访问都使用同一个TLB条目
        for (int i = 0; i < (2 * 1024 * 1024) / sizeof(int); i++) {
            array[i] = i;  // 所有访问都是TLB命中
        }

        munmap(huge_page, 2 * 1024 * 1024);
    }
}

/*
 * 指针别名和TLB
 */
void pointer_aliasing(void) {
    int x = 10;
    int *p1 = &x;
    int *p2 = &x;  // p1和p2是别名

    /*
     * 编译器优化挑战:
     * *p1 = 5;
     * *p2 = 10;  // 必须执行，影响*p1
     *
     * 如果编译器不知道p1和p2指向同一地址，
     * 可能错误优化掉*p2的赋值
     *
     * TLB层面: 两个指针的虚拟地址相同，
     * 使用同一个TLB条目
     */

    *p1 = 5;
    *p2 = 10;

    printf("x = %d (should be 10)\n", x);
}

/*
 * volatile和TLB
 */
void volatile_and_tlb(void) {
    volatile int *hardware_reg = (volatile int*)0xFEBC0000;

    /*
     * volatile告诉编译器:
     * 1. 不要优化掉对此地址的访问
     * 2. 每次访问都要真正读写内存
     *
     * TLB层面:
     * 0xFEBC0000通常是MMIO地址
     * 页表项可能设置了PWT/PCD标志 (禁用缓存)
     * 或者映射为Device Memory类型
     *
     * 注意: volatile不控制TLB行为，
     * 它控制的是编译器优化和CPU缓存行为
     */

    int val = *hardware_reg;  // 强制内存读取
    *hardware_reg = 0x1;       // 强制内存写入
}

/*
 * 通过/proc/pid/pagemap查看物理地址
 */
void show_physical_address(void) {
    int x = 42;
    uintptr_t vaddr = (uintptr_t)&x;

    printf("Virtual address: 0x%lx\n", vaddr);

    /*
     * 读取 /proc/self/pagemap 可以获取对应的物理页框号
     * 需要root权限
     *
     * 这展示了虚拟地址到物理地址的映射关系
     * 正是TLB缓存的内容
     */

    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd >= 0) {
        uint64_t offset = (vaddr / 4096) * sizeof(uint64_t);
        lseek(fd, offset, SEEK_SET);

        uint64_t entry;
        read(fd, &entry, sizeof(entry));

        if (entry & (1ULL << 63)) {
            uint64_t pfn = entry & ((1ULL << 55) - 1);
            uint64_t paddr = (pfn << 12) | (vaddr & 0xFFF);
            printf("Physical address: 0x%lx\n", paddr);
        }

        close(fd);
    }
}

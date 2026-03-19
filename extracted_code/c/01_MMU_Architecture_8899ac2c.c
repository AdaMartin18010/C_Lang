/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\01_MMU_Architecture.md
 * Line: 463
 * Language: c
 * Block ID: 8899ac2c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 从C语言程序员的角度看地址转换
 * 这些代码展示了虚拟地址如何在MMU作用下映射到物理地址
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

/*
 * 虚拟地址结构 (从软件视角)
 * 注意：这只是概念模型，实际MMU在硬件中实现
 */
typedef struct {
    uint64_t unused  : 16;  // 高位未使用 (符号扩展)
    uint64_t pml4    : 9;   // PML4索引
    uint64_t pdpt    : 9;   // PDPT索引
    uint64_t pd      : 9;   // PD索引
    uint64_t pt      : 9;   // PT索引
    uint64_t offset  : 12;  // 页内偏移
} virtual_addr_t;

/*
 * 页表项结构 (x86-64, 64位)
 */
typedef union {
    uint64_t value;
    struct {
        uint64_t present    : 1;   // P - 存在位
        uint64_t rw         : 1;   // R/W - 读写位
        uint64_t us         : 1;   // U/S - 用户/超级用户
        uint64_t pwt        : 1;   // PWT - 页级写透
        uint64_t pcd        : 1;   // PCD - 页级缓存禁用
        uint64_t accessed   : 1;   // A - 访问位
        uint64_t dirty      : 1;   // D - 脏位
        uint64_t ps         : 1;   // PS - 页大小
        uint64_t global     : 1;   // G - 全局页
        uint64_t ignored    : 3;   // 忽略
        uint64_t pfn        : 40;  // 页框号 (物理地址[51:12])
        uint64_t reserved   : 11;  // 保留
        uint64_t nx         : 1;   // XD/NX - 执行禁用
    } bits;
} page_table_entry_t;

/*
 * 演示：C程序如何"感知"虚拟内存
 */
void demonstrate_virtual_memory(void) {
    int local_var;           // 栈变量
    static int static_var;   // BSS段
    int* heap_var;           // 堆变量

    heap_var = malloc(sizeof(int));

    printf("=== C程序中的虚拟地址 ===\n");
    printf("代码段地址:   %p\n", (void*)demonstrate_virtual_memory);
    printf("栈变量地址:   %p\n", (void*)&local_var);
    printf("静态变量地址: %p\n", (void*)&static_var);
    printf("堆变量地址:   %p\n", (void*)heap_var);

    /*
     * 输出示例 (x86-64 Linux):
     * 代码段地址:   0x55a3b2c0c1a9  (高地址 - 接近0xFFFFFFFF)
     * 栈变量地址:   0x7ffcc2a8f4ac  (高地址 - 向下增长)
     * 静态变量地址: 0x55a3b2c0d024  (中等地址)
     * 堆变量地址:   0x55a3b3e2a2a0  (中等地址 - 向上增长)
     *
     * 注意：这些都是虚拟地址！
     * 实际的物理地址只有MMU知道
     */

    free(heap_var);
}

/*
 * 读取进程的页表信息 (通过/proc/[pid]/pagemap)
 * 需要root权限
 */
int get_physical_address(uintptr_t virtual_addr, uint64_t* physical_addr) {
    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) {
        perror("open pagemap");
        return -1;
    }

    // 计算pagemap中的偏移
    // 每个页对应一个64位条目
    uint64_t page_size = 4096;
    uint64_t vpn = virtual_addr / page_size;
    uint64_t offset = vpn * sizeof(uint64_t);

    if (lseek(fd, offset, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return -1;
    }

    uint64_t entry;
    if (read(fd, &entry, sizeof(entry)) != sizeof(entry)) {
        perror("read");
        close(fd);
        return -1;
    }

    close(fd);

    // 检查页是否存在
    if (!(entry & (1ULL << 63))) {
        printf("Page not present in memory\n");
        return -1;
    }

    // 提取页框号
    uint64_t pfn = entry & ((1ULL << 55) - 1);
    uint64_t page_offset = virtual_addr % page_size;

    *physical_addr = (pfn * page_size) + page_offset;
    return 0;
}

/*
 * 演示：获取虚拟地址对应的物理地址
 */
void show_address_translation(void) {
    int test_var = 0x12345678;
    uintptr_t virtual_addr = (uintptr_t)&test_var;
    uint64_t physical_addr;

    printf("\n=== 地址转换演示 ===\n");
    printf("虚拟地址: 0x%016lx\n", virtual_addr);

    if (get_physical_address(virtual_addr, &physical_addr) == 0) {
        printf("物理地址: 0x%016lx\n", physical_addr);
    }
}

/*
 * 模拟MMU地址转换过程 (概念性代码)
 */
typedef struct {
    uint64_t pml4_base;  // PML4物理基址
} mmu_state_t;

/*
 * 模拟四级页表遍历
 * 实际MMU在硬件中完成，这里仅用于理解
 */
uint64_t mmu_translate(mmu_state_t* mmu, uint64_t virtual_addr) {
    // 分解虚拟地址
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index   = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index   = (virtual_addr >> 12) & 0x1FF;
    uint64_t offset     = virtual_addr & 0xFFF;

    printf("\n=== MMU地址转换过程 (模拟) ===\n");
    printf("虚拟地址: 0x%016lx\n", virtual_addr);
    printf("  PML4 index: %3lu (0x%03lx)\n", pml4_index, pml4_index);
    printf("  PDPT index: %3lu (0x%03lx)\n", pdpt_index, pdpt_index);
    printf("  PD   index: %3lu (0x%03lx)\n", pd_index, pd_index);
    printf("  PT   index: %3lu (0x%03lx)\n", pt_index, pt_index);
    printf("  Page offset: 0x%03lx\n", offset);

    // 步骤1: 读取PML4项
    uint64_t pml4_entry_phys = mmu->pml4_base + (pml4_index * 8);
    printf("\n步骤1: 读取PML4项\n");
    printf("  PML4物理基址: 0x%lx\n", mmu->pml4_base);
    printf("  PML4项物理地址: 0x%lx\n", pml4_entry_phys);

    // 步骤2-4: 类似地遍历PDPT、PD、PT...
    // 这里省略具体的内存读取操作

    // 最终组合物理地址
    uint64_t pfn = 0x12345;  // 假设的页框号
    uint64_t physical_addr = (pfn << 12) | offset;

    printf("\n步骤6: 组合物理地址\n");
    printf("  页框号: 0x%lx\n", pfn);
    printf("  物理地址: 0x%016lx\n", physical_addr);

    return physical_addr;
}

int main() {
    demonstrate_virtual_memory();
    // show_address_translation();  // 需要root权限

    mmu_state_t mmu = { .pml4_base = 0x100000 };
    mmu_translate(&mmu, 0x00007FFF80001000ULL);

    return 0;
}

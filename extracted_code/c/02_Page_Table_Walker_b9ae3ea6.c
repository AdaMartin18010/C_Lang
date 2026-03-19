/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\02_Page_Table_Walker.md
 * Line: 444
 * Language: c
 * Block ID: b9ae3ea6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 页表遍历器的C语言模拟实现
 * 用于理解页表遍历的算法逻辑
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
 * 页表项定义 (x86-64风格)
 */
typedef union {
    uint64_t value;
    struct {
        uint64_t present    : 1;   // 0
        uint64_t rw         : 1;   // 1
        uint64_t us         : 1;   // 2
        uint64_t pwt        : 1;   // 3
        uint64_t pcd        : 1;   // 4
        uint64_t accessed   : 1;   // 5
        uint64_t dirty      : 1;   // 6
        uint64_t ps         : 1;   // 7
        uint64_t global     : 1;   // 8
        uint64_t avl        : 3;   // 9-11
        uint64_t pfn        : 40;  // 12-51 (页框号)
        uint64_t reserved   : 11;  // 52-62
        uint64_t nx         : 1;   // 63
    } bits;
} pte_t;

/*
 * 模拟物理内存 (简化模型)
 */
#define PHYS_MEM_SIZE (256 * 1024 * 1024)  // 256MB物理内存
#define PAGE_SIZE 4096
#define PFN_SHIFT 12

static uint8_t physical_memory[PHYS_MEM_SIZE];

/*
 * 页表基址 (模拟CR3)
 */
static uint64_t cr3_base = 0x10000;  // PML4表在物理地址0x10000

/*
 * 读取物理内存 (模拟内存总线)
 */
static uint64_t read_physical_qword(uint64_t phys_addr) {
    if (phys_addr + 8 > PHYS_MEM_SIZE) {
        printf("ERROR: Physical address out of range: 0x%lx\n", phys_addr);
        return 0;
    }
    uint64_t value;
    memcpy(&value, &physical_memory[phys_addr], sizeof(value));
    return value;
}

/*
 * 写入物理内存
 */
static void write_physical_qword(uint64_t phys_addr, uint64_t value) {
    if (phys_addr + 8 > PHYS_MEM_SIZE) {
        printf("ERROR: Physical address out of range: 0x%lx\n", phys_addr);
        return;
    }
    memcpy(&physical_memory[phys_addr], &value, sizeof(value));
}

/*
 * 初始化页表 (设置一个恒等映射)
 */
void init_page_tables(void) {
    printf("=== 初始化页表 ===\n");

    // PML4表地址
    uint64_t pml4_addr = cr3_base;
    // PDPT表地址 (紧跟PML4)
    uint64_t pdpt_addr = pml4_addr + PAGE_SIZE;
    // PD表地址
    uint64_t pd_addr = pdpt_addr + PAGE_SIZE;
    // PT表地址
    uint64_t pt_addr = pd_addr + PAGE_SIZE;

    printf("PML4 表地址: 0x%lx\n", pml4_addr);
    printf("PDPT 表地址: 0x%lx\n", pdpt_addr);
    printf("PD   表地址: 0x%lx\n", pd_addr);
    printf("PT   表地址: 0x%lx\n", pt_addr);

    // 设置PML4[0] -> PDPT
    pte_t pml4e = { .value = 0 };
    pml4e.bits.present = 1;
    pml4e.bits.rw = 1;
    pml4e.bits.us = 1;
    pml4e.bits.pfn = pdpt_addr >> PFN_SHIFT;
    write_physical_qword(pml4_addr, pml4e.value);
    printf("PML4[0] = 0x%016lx (指向PDPT)\n", pml4e.value);

    // 设置PDPT[0] -> PD
    pte_t pdpte = { .value = 0 };
    pdpte.bits.present = 1;
    pdpte.bits.rw = 1;
    pdpte.bits.us = 1;
    pdpte.bits.pfn = pd_addr >> PFN_SHIFT;
    write_physical_qword(pdpt_addr, pdpte.value);
    printf("PDPT[0] = 0x%016lx (指向PD)\n", pdpte.value);

    // 设置PD[0] -> PT
    pte_t pde = { .value = 0 };
    pde.bits.present = 1;
    pde.bits.rw = 1;
    pde.bits.us = 1;
    pde.bits.pfn = pt_addr >> PFN_SHIFT;
    write_physical_qword(pd_addr, pde.value);
    printf("PD[0]   = 0x%016lx (指向PT)\n", pde.value);

    // 设置PT[0..511] -> 物理页 (恒等映射前2MB)
    for (int i = 0; i < 512; i++) {
        pte_t pte = { .value = 0 };
        pte.bits.present = 1;
        pte.bits.rw = 1;
        pte.bits.us = 1;
        pte.bits.pfn = i;  // 恒等映射
        write_physical_qword(pt_addr + i * 8, pte.value);
    }
    printf("PT[0..511] 设置为恒等映射\n");
}

/*
 * 页表遍历结果
 */
typedef struct {
    bool success;
    uint64_t physical_addr;
    pte_t final_pte;
    const char* page_size;
    int access_count;  // 内存访问次数
    const char* error_msg;
} walk_result_t;

/*
 * 页表遍历器 (核心算法)
 */
walk_result_t walk_page_table(uint64_t vaddr) {
    walk_result_t result = {0};
    result.access_count = 0;

    printf("\n=== 页表遍历: VA = 0x%016lx ===\n", vaddr);

    // 分解虚拟地址
    uint64_t pml4_index = (vaddr >> 39) & 0x1FF;
    uint64_t pdpt_index = (vaddr >> 30) & 0x1FF;
    uint64_t pd_index   = (vaddr >> 21) & 0x1FF;
    uint64_t pt_index   = (vaddr >> 12) & 0x1FF;
    uint64_t offset     = vaddr & 0xFFF;

    printf("虚拟地址分解:\n");
    printf("  PML4 index: %3lu (0x%03lx)\n", pml4_index, pml4_index);
    printf("  PDPT index: %3lu (0x%03lx)\n", pdpt_index, pdpt_index);
    printf("  PD   index: %3lu (0x%03lx)\n", pd_index, pd_index);
    printf("  PT   index: %3lu (0x%03lx)\n", pt_index, pt_index);
    printf("  Offset:     0x%03lx\n", offset);

    // 步骤1: 读取PML4项
    uint64_t pml4_entry_addr = cr3_base + (pml4_index * 8);
    printf("\n步骤1: 读取PML4项\n");
    printf("  PML4表基址: 0x%lx (来自CR3)\n", cr3_base);
    printf("  PML4项地址: 0x%lx + %lu×8 = 0x%lx\n",
           cr3_base, pml4_index, pml4_entry_addr);

    pte_t pml4e = { .value = read_physical_qword(pml4_entry_addr) };
    result.access_count++;
    printf("  PML4E值: 0x%016lx\n", pml4e.value);
    printf("    Present: %lu, R/W: %lu, U/S: %lu\n",
           pml4e.bits.present, pml4e.bits.rw, pml4e.bits.us);

    if (!pml4e.bits.present) {
        result.error_msg = "PML4 entry not present";
        goto fault;
    }

    // 步骤2: 读取PDPT项
    uint64_t pdpt_base = pml4e.bits.pfn << PFN_SHIFT;
    uint64_t pdpt_entry_addr = pdpt_base + (pdpt_index * 8);
    printf("\n步骤2: 读取PDPT项\n");
    printf("  PDPT表基址: 0x%lx (来自PML4E)\n", pdpt_base);
    printf("  PDPT项地址: 0x%lx + %lu×8 = 0x%lx\n",
           pdpt_base, pdpt_index, pdpt_entry_addr);

    pte_t pdpte = { .value = read_physical_qword(pdpt_entry_addr) };
    result.access_count++;
    printf("  PDPTE值: 0x%016lx\n", pdpte.value);
    printf("    Present: %lu, PS: %lu\n", pdpte.bits.present, pdpte.bits.ps);

    if (!pdpte.bits.present) {
        result.error_msg = "PDPT entry not present";
        goto fault;
    }

    // 检查1GB大页
    if (pdpte.bits.ps) {
        printf("\n  >>> 1GB大页! <<<?\n");
        result.page_size = "1GB";
        result.physical_addr = (pdpte.bits.pfn << PFN_SHIFT) | (vaddr & 0x3FFFFFFF);
        result.final_pte = pdpte;
        goto success;
    }

    // 步骤3: 读取PD项
    uint64_t pd_base = pdpte.bits.pfn << PFN_SHIFT;
    uint64_t pd_entry_addr = pd_base + (pd_index * 8);
    printf("\n步骤3: 读取PD项\n");
    printf("  PD表基址: 0x%lx (来自PDPTE)\n", pd_base);
    printf("  PD项地址: 0x%lx + %lu×8 = 0x%lx\n",
           pd_base, pd_index, pd_entry_addr);

    pte_t pde = { .value = read_physical_qword(pd_entry_addr) };
    result.access_count++;
    printf("  PDE值: 0x%016lx\n", pde.value);
    printf("    Present: %lu, PS: %lu\n", pde.bits.present, pde.bits.ps);

    if (!pde.bits.present) {
        result.error_msg = "PD entry not present";
        goto fault;
    }

    // 检查2MB大页
    if (pde.bits.ps) {
        printf("\n  >>> 2MB大页! <<<\n");
        result.page_size = "2MB";
        result.physical_addr = (pde.bits.pfn << PFN_SHIFT) | (vaddr & 0x1FFFFF);
        result.final_pte = pde;
        goto success;
    }

    // 步骤4: 读取PT项
    uint64_t pt_base = pde.bits.pfn << PFN_SHIFT;
    uint64_t pt_entry_addr = pt_base + (pt_index * 8);
    printf("\n步骤4: 读取PT项\n");
    printf("  PT表基址: 0x%lx (来自PDE)\n", pt_base);
    printf("  PT项地址: 0x%lx + %lu×8 = 0x%lx\n",
           pt_base, pt_index, pt_entry_addr);

    pte_t pte = { .value = read_physical_qword(pt_entry_addr) };
    result.access_count++;
    printf("  PTE值: 0x%016lx\n", pte.value);
    printf("    Present: %lu, R/W: %lu, U/S: %lu\n",
           pte.bits.present, pte.bits.rw, pte.bits.us);

    if (!pte.bits.present) {
        result.error_msg = "PTE not present";
        goto fault;
    }

    printf("\n  >>> 4KB页 <<<\n");
    result.page_size = "4KB";
    result.physical_addr = (pte.bits.pfn << PFN_SHIFT) | offset;
    result.final_pte = pte;

success:
    result.success = true;
    printf("\n=== 遍历成功 ===\n");
    printf("页大小: %s\n", result.page_size);
    printf("物理地址: 0x%016lx\n", result.physical_addr);
    printf("内存访问次数: %d\n", result.access_count);
    return result;

fault:
    result.success = false;
    printf("\n=== 页错误 ===\n");
    printf("错误: %s\n", result.error_msg);
    printf("内存访问次数: %d\n", result.access_count);
    return result;
}

int main() {
    // 初始化页表
    init_page_tables();

    // 测试地址转换
    // 虚拟地址0x1000应该映射到物理地址0x1000 (恒等映射)
    walk_result_t r1 = walk_page_table(0x1000);

    // 虚拟地址0x2000应该映射到物理地址0x2000
    walk_result_t r2 = walk_page_table(0x2000);

    // 虚拟地址0x12345678的映射
    walk_result_t r3 = walk_page_table(0x12345678);

    return 0;
}

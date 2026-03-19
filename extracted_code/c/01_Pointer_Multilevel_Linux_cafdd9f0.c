/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 356
 * Language: c
 * Block ID: cafdd9f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux x86_64页表: arch/x86/include/asm/pgtable.h
// 四级页表: PGD → PUD → PMD → PTE → Page

// 页表项类型定义（简化）
typedef struct { unsigned long pte; } pte_t;      // Level 1
typedef struct { unsigned long pmd; } pmd_t;      // Level 2
typedef struct { unsigned long pud; } pud_t;      // Level 3
typedef struct { unsigned long pgd; } pgd_t;      // Level 4

// 页表遍历函数展示多级指针
// 虚拟地址: [PGD索引 | PUD索引 | PMD索引 | PTE索引 | 页内偏移]

#define PGDIR_SHIFT     39
#define PUD_SHIFT       30
#define PMD_SHIFT       21
#define PAGE_SHIFT      12

// 获取各级索引
#define pgd_index(addr) (((addr) >> PGDIR_SHIFT) & 0x1FF)
#define pud_index(addr) (((addr) >> PUD_SHIFT) & 0x1FF)
#define pmd_index(addr) (((addr) >> PMD_SHIFT) & 0x1FF)
#define pte_index(addr) (((addr) >> PAGE_SHIFT) & 0x1FF)

// 四级页表遍历（展示多级指针语义）
pte_t *lookup_address(unsigned long addr, pgd_t *pgd_base) {
    // Level 4: PGD (Page Global Directory)
    pgd_t *pgd = pgd_base + pgd_index(addr);
    if (!pgd_present(*pgd))
        return NULL;

    // Level 3: PUD (Page Upper Directory)
    pud_t *pud = pud_offset(pgd, addr);
    if (!pud_present(*pud))
        return NULL;

    // Level 2: PMD (Page Middle Directory)
    pmd_t *pmd = pmd_offset(pud, addr);
    if (!pmd_present(*pmd))
        return NULL;

    // Level 1: PTE (Page Table Entry)
    pte_t *pte = pte_offset_kernel(pmd, addr);

    return pte;  // 返回最终页表项指针
}

// 对应的多级指针理解:
// pgd_base → pgd → pud → pmd → pte → page
// (四级)    (三级) (二级) (一级) (零级:实际物理页)

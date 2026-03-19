/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\02_Page_Table_Walker.md
 * Line: 1010
 * Language: c
 * Block ID: 3034140a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 页表缓存的C语言模拟实现
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
 * PTE缓存条目
 */
#define PML4_CACHE_SIZE 8
#define PDPT_CACHE_SIZE 16
#define PD_CACHE_SIZE 32

typedef struct {
    uint16_t tag;       // 部分VPN作为标签
    uint64_t pte;       // 页表项
    bool valid;
    uint8_t lru_counter;
} pte_cache_entry_t;

/*
 * PTE缓存结构
 */
typedef struct {
    pte_cache_entry_t entries[PML4_CACHE_SIZE];
    const char* name;
} pte_cache_t;

/*
 * 三级PTE缓存
 */
typedef struct {
    pte_cache_t pml4_cache;
    pte_cache_t pdpt_cache;
    pte_cache_t pd_cache;

    // 统计信息
    uint64_t hits;
    uint64_t misses;
} pte_cache_system_t;

/*
 * 初始化PTE缓存
 */
void init_pte_cache(pte_cache_system_t* cache) {
    memset(cache, 0, sizeof(*cache));
    cache->pml4_cache.name = "PML4";
    cache->pdpt_cache.name = "PDPT";
    cache->pd_cache.name = "PD";
}

/*
 * PTE缓存查找
 */
bool lookup_pte_cache(pte_cache_t* cache, uint16_t tag, uint64_t* pte) {
    for (int i = 0; i < PML4_CACHE_SIZE; i++) {
        if (cache->entries[i].valid && cache->entries[i].tag == tag) {
            *pte = cache->entries[i].pte;
            // 更新LRU
            cache->entries[i].lru_counter = 0;
            for (int j = 0; j < PML4_CACHE_SIZE; j++) {
                if (j != i && cache->entries[j].valid) {
                    cache->entries[j].lru_counter++;
                }
            }
            return true;
        }
    }
    return false;
}

/*
 * PTE缓存更新
 */
void update_pte_cache(pte_cache_t* cache, uint16_t tag, uint64_t pte) {
    // 查找空闲条目或LRU最老的条目
    int victim_idx = -1;
    uint8_t max_lru = 0;

    for (int i = 0; i < PML4_CACHE_SIZE; i++) {
        if (!cache->entries[i].valid) {
            victim_idx = i;
            break;
        }
        if (cache->entries[i].lru_counter > max_lru) {
            max_lru = cache->entries[i].lru_counter;
            victim_idx = i;
        }
    }

    if (victim_idx >= 0) {
        cache->entries[victim_idx].valid = true;
        cache->entries[victim_idx].tag = tag;
        cache->entries[victim_idx].pte = pte;
        cache->entries[victim_idx].lru_counter = 0;
    }
}

/*
 * 带PTE缓存的页表遍历
 */
typedef struct {
    bool success;
    uint64_t physical_addr;
    int memory_accesses;  // 实际内存访问次数
    int cache_hits;       // 缓存命中次数
} cached_walk_result_t;

cached_walk_result_t walk_with_pte_cache(pte_cache_system_t* cache,
                                          uint64_t vaddr,
                                          uint64_t cr3_base) {
    cached_walk_result_t result = {0};

    uint64_t pml4_index = (vaddr >> 39) & 0x1FF;
    uint64_t pdpt_index = (vaddr >> 30) & 0x1FF;
    uint64_t pd_index   = (vaddr >> 21) & 0x1FF;
    uint64_t pt_index   = (vaddr >> 12) & 0x1FF;
    uint64_t offset     = vaddr & 0xFFF;

    uint64_t pte;
    bool hit;

    // 步骤1: 查找PML4缓存
    hit = lookup_pte_cache(&cache->pml4_cache, pml4_index, &pte);
    if (!hit) {
        // 从内存读取PML4项
        result.memory_accesses++;
        pte = read_physical_qword(cr3_base + pml4_index * 8);
        update_pte_cache(&cache->pml4_cache, pml4_index, pte);
    } else {
        result.cache_hits++;
    }

    if (!(pte & 1)) {  // P=0
        result.success = false;
        return result;
    }

    uint64_t pdpt_base = (pte >> 12) << 12;

    // 步骤2: 查找PDPT缓存
    uint16_t pdpt_tag = (pml4_index << 9) | pdpt_index;
    hit = lookup_pte_cache(&cache->pdpt_cache, pdpt_tag, &pte);
    if (!hit) {
        result.memory_accesses++;
        pte = read_physical_qword(pdpt_base + pdpt_index * 8);
        update_pte_cache(&cache->pdpt_cache, pdpt_tag, pte);
    } else {
        result.cache_hits++;
    }

    if (!(pte & 1)) {
        result.success = false;
        return result;
    }

    if (pte & (1ULL << 7)) {  // PS=1, 1GB页
        result.success = true;
        result.physical_addr = ((pte >> 12) << 12) | (vaddr & 0x3FFFFFFF);
        return result;
    }

    uint64_t pd_base = (pte >> 12) << 12;

    // 步骤3: 查找PD缓存
    uint16_t pd_tag = (pdpt_tag << 9) | pd_index;
    hit = lookup_pte_cache(&cache->pd_cache, pd_tag, &pte);
    if (!hit) {
        result.memory_accesses++;
        pte = read_physical_qword(pd_base + pd_index * 8);
        update_pte_cache(&cache->pd_cache, pd_tag, pte);
    } else {
        result.cache_hits++;
    }

    if (!(pte & 1)) {
        result.success = false;
        return result;
    }

    if (pte & (1ULL << 7)) {  // PS=1, 2MB页
        result.success = true;
        result.physical_addr = ((pte >> 12) << 12) | (vaddr & 0x1FFFFF);
        return result;
    }

    uint64_t pt_base = (pte >> 12) << 12;

    // 步骤4: 读取PTE (最后一级不缓存，直接读取)
    result.memory_accesses++;
    pte = read_physical_qword(pt_base + pt_index * 8);

    if (!(pte & 1)) {
        result.success = false;
        return result;
    }

    result.success = true;
    result.physical_addr = ((pte >> 12) << 12) | offset;
    return result;
}

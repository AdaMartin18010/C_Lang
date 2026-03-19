/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\03_TLB_Implementation.md
 * Line: 544
 * Language: c
 * Block ID: 547d96d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * TLB LRU替换策略的C语言实现
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define TLB_SIZE 64
#define VPN_BITS 36
#define PFN_BITS 40

/*
 * TLB条目
 */
typedef struct {
    uint64_t vpn;        // 虚拟页号 (标签)
    uint64_t pfn;        // 物理页号
    uint16_t asid;       // 地址空间ID
    uint8_t  flags;      // 权限标志
    bool     valid;      // 有效位
    bool     global;     // 全局页
    uint8_t  lru_counter; // LRU计数器
} tlb_entry_t;

/*
 * TLB结构
 */
typedef struct {
    tlb_entry_t entries[TLB_SIZE];
    uint32_t access_count;  // 访问计数，用于LRU更新
} tlb_t;

/*
 * 初始化TLB
 */
void tlb_init(tlb_t *tlb) {
    memset(tlb, 0, sizeof(tlb_t));
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb->entries[i].valid = false;
        tlb->entries[i].lru_counter = 0;
    }
    tlb->access_count = 0;
}

/*
 * 查找TLB条目
 */
bool tlb_lookup(tlb_t *tlb, uint64_t vpn, uint16_t asid,
                uint64_t *pfn, uint8_t *flags) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb->entries[i].valid &&
            tlb->entries[i].vpn == vpn &&
            (tlb->entries[i].global || tlb->entries[i].asid == asid)) {

            // 命中!
            *pfn = tlb->entries[i].pfn;
            *flags = tlb->entries[i].flags;

            // 更新LRU计数器 (置为最大值)
            tlb->entries[i].lru_counter = 255;

            // 减少其他条目的计数器
            for (int j = 0; j < TLB_SIZE; j++) {
                if (j != i && tlb->entries[j].valid &&
                    tlb->entries[j].lru_counter > 0) {
                    tlb->entries[j].lru_counter--;
                }
            }

            tlb->access_count++;
            return true;
        }
    }

    // 未命中
    return false;
}

/*
 * 插入TLB条目 (使用LRU替换)
 */
void tlb_insert(tlb_t *tlb, uint64_t vpn, uint64_t pfn,
                uint16_t asid, uint8_t flags, bool global) {
    int victim_idx = -1;
    uint8_t min_lru = 255;

    // 查找空闲条目或LRU最小的条目
    for (int i = 0; i < TLB_SIZE; i++) {
        if (!tlb->entries[i].valid) {
            victim_idx = i;
            break;
        }
        if (tlb->entries[i].lru_counter < min_lru) {
            min_lru = tlb->entries[i].lru_counter;
            victim_idx = i;
        }
    }

    // 插入新条目
    tlb->entries[victim_idx].valid = true;
    tlb->entries[victim_idx].vpn = vpn;
    tlb->entries[victim_idx].pfn = pfn;
    tlb->entries[victim_idx].asid = asid;
    tlb->entries[victim_idx].flags = flags;
    tlb->entries[victim_idx].global = global;
    tlb->entries[victim_idx].lru_counter = 255;

    printf("TLB Insert: VPN=0x%lx, PFN=0x%lx, Index=%d (LRU replaced %d)\n",
           vpn, pfn, victim_idx, min_lru);
}

/*
 * 打印TLB状态
 */
void tlb_print(tlb_t *tlb) {
    printf("\n=== TLB Status ===\n");
    int valid_count = 0;
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb->entries[i].valid) {
            valid_count++;
            printf("Entry %2d: VPN=0x%010lx, PFN=0x%010lx, ASID=%d, LRU=%d\n",
                   i, tlb->entries[i].vpn, tlb->entries[i].pfn,
                   tlb->entries[i].asid, tlb->entries[i].lru_counter);
        }
    }
    printf("Valid entries: %d/%d\n", valid_count, TLB_SIZE);
}

/*
 * 演示LRU行为
 */
int main() {
    tlb_t tlb;
    uint64_t pfn;
    uint8_t flags;

    tlb_init(&tlb);

    printf("=== TLB LRU Replacement Demo ===\n\n");

    // 插入8个条目
    printf("Inserting 8 entries...\n");
    for (int i = 0; i < 8; i++) {
        tlb_insert(&tlb, 0x1000 + i, 0x2000 + i, 0, 0x07, false);
    }

    // 访问其中几个，更新LRU
    printf("\nAccessing entries 0, 2, 4, 6...\n");
    tlb_lookup(&tlb, 0x1000, 0, &pfn, &flags);
    tlb_lookup(&tlb, 0x1002, 0, &pfn, &flags);
    tlb_lookup(&tlb, 0x1004, 0, &pfn, &flags);
    tlb_lookup(&tlb, 0x1006, 0, &pfn, &flags);

    tlb_print(&tlb);

    // 插入新条目，应该替换LRU最低的 (1, 3, 5, 7)
    printf("\nInserting new entry (should replace LRU=0)...\n");
    tlb_insert(&tlb, 0x2000, 0x3000, 0, 0x07, false);

    tlb_print(&tlb);

    return 0;
}

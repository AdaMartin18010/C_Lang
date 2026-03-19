/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\README.md
 * Line: 754
 * Language: c
 * Block ID: 2365d90f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DRAM + PMEM 分层存储
#include <memkind.h>

struct memkind *pmem_kind = NULL;

void init_tiered_memory(const char *pmem_path, size_t pmem_size) {
    // 初始化持久内存 kind
    memkind_create_pmem(pmem_path, pmem_size, &pmem_kind);
}

void* tiered_alloc(size_t size, int hot_data) {
    if (hot_data) {
        // 热数据放在 DRAM
        return malloc(size);
    } else {
        // 冷数据放在持久内存
        return memkind_malloc(pmem_kind, size);
    }
}

// 自动数据放置（Auto-tiering）
// 基于访问频率自动在 DRAM 和 PMEM 间迁移

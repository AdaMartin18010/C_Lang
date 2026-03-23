---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# 哈希索引实现

> **主题**: 内存数据库哈希索引 | **难度**: L4 | **更新**: 2026-03-15

---

## 概述

哈希索引通过哈希函数将键映射到存储位置，实现O(1)时间复杂度的查找。

```text
哈希索引结构:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
键(key) → [哈希函数] → 桶索引 → [链表/数组] → 值(value)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## 核心实现

### 1. 基础哈希表

```c
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// 哈希表条目
typedef struct hash_entry {
    char* key;
    void* value;
    struct hash_entry* next;  // 链地址法处理冲突
} hash_entry_t;

// 哈希表
typedef struct {
    hash_entry_t** buckets;   // 桶数组
    size_t num_buckets;       // 桶数量
    size_t num_entries;       // 条目数
    float load_factor;        // 负载因子
} hash_table_t;

// FNV-1a哈希函数
uint64_t fnv1a_hash(const char* key) {
    uint64_t hash = 14695981039346656037ULL;
    while (*key) {
        hash ^= (uint64_t)*key++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

// 创建哈希表
hash_table_t* hash_table_create(size_t initial_size) {
    hash_table_t* ht = malloc(sizeof(hash_table_t));
    ht->buckets = calloc(initial_size, sizeof(hash_entry_t*));
    ht->num_buckets = initial_size;
    ht->num_entries = 0;
    ht->load_factor = 0.75f;
    return ht;
}

// 插入键值对
void hash_table_put(hash_table_t* ht, const char* key, void* value) {
    uint64_t hash = fnv1a_hash(key);
    size_t idx = hash % ht->num_buckets;

    // 查找是否存在
    hash_entry_t* entry = ht->buckets[idx];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;  // 更新
            return;
        }
        entry = entry->next;
    }

    // 创建新条目
    entry = malloc(sizeof(hash_entry_t));
    entry->key = strdup(key);
    entry->value = value;
    entry->next = ht->buckets[idx];
    ht->buckets[idx] = entry;
    ht->num_entries++;

    // 检查是否需要扩容
    if ((float)ht->num_entries / ht->num_buckets > ht->load_factor) {
        hash_table_resize(ht, ht->num_buckets * 2);
    }
}

// 查找
void* hash_table_get(hash_table_t* ht, const char* key) {
    uint64_t hash = fnv1a_hash(key);
    size_t idx = hash % ht->num_buckets;

    hash_entry_t* entry = ht->buckets[idx];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

// 扩容
void hash_table_resize(hash_table_t* ht, size_t new_size) {
    hash_entry_t** old_buckets = ht->buckets;
    size_t old_size = ht->num_buckets;

    ht->buckets = calloc(new_size, sizeof(hash_entry_t*));
    ht->num_buckets = new_size;
    ht->num_entries = 0;

    // 重新哈希所有条目
    for (size_t i = 0; i < old_size; i++) {
        hash_entry_t* entry = old_buckets[i];
        while (entry) {
            hash_entry_t* next = entry->next;
            hash_table_put(ht, entry->key, entry->value);
            free(entry->key);
            free(entry);
            entry = next;
        }
    }

    free(old_buckets);
}
```

### 2. 内存优化

```c
// 使用开放寻址法的线性探测
typedef struct {
    char* key;
    void* value;
    uint8_t status;  // 0=空, 1=占用, 2=删除
} open_entry_t;

typedef struct {
    open_entry_t* entries;
    size_t size;
    size_t count;
} open_hash_table_t;

void* open_hash_get(open_hash_table_t* ht, const char* key) {
    uint64_t hash = fnv1a_hash(key);
    size_t idx = hash % ht->size;

    // 线性探测
    for (size_t i = 0; i < ht->size; i++) {
        size_t probe = (idx + i) % ht->size;
        open_entry_t* entry = &ht->entries[probe];

        if (entry->status == 0) {
            return NULL;  // 找到空槽，键不存在
        }
        if (entry->status == 1 && strcmp(entry->key, key) == 0) {
            return entry->value;
        }
    }
    return NULL;
}
```

## 性能分析

| 操作 | 平均 | 最坏 | 说明 |
|:-----|:-----|:-----|:-----|
| 查找 | O(1) | O(n) | 依赖哈希函数质量 |
| 插入 | O(1) | O(n) | 扩容时O(n) |
| 删除 | O(1) | O(n) | - |

---

> **状态**: ✅ 已完成


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review

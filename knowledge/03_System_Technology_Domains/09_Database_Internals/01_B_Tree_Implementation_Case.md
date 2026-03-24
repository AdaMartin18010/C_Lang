# B-Tree数据库内核实现案例

> **层级**: L0 (案例层) + L3 (系统层)
> **目标**: 完整实现生产级B-Tree存储引擎，理解数据库内核设计原理
> **关联**: [高级数据结构](../01_Core_Knowledge_System/05_Data_Structures_Algorithms/README.md) | [文件系统](../03_System_Technology_Domains/03_OS_Internals/README.md)

---

## 1. B-Tree理论基础

### 1.1 B-Tree定义与性质

```
┌─────────────────────────────────────────────────────────────┐
│              B-Tree的形式化定义                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  定义 (B-Tree of order m):                                    │
│  满足以下性质的m叉搜索树：                                    │
│                                                              │
│  1. 每个节点最多有 m 个子节点                                 │
│  2. 每个非根节点至少有 ⌈m/2⌉ 个子节点                        │
│  3. 根节点至少有 2 个子节点 (除非它是叶子)                   │
│  4. 所有叶子节点在同一层                                      │
│  5. 有 k 个子节点的非叶子节点包含 k-1 个键                   │
│  6. 键按升序排列，子树键值范围有序                            │
│                                                              │
│  B-Tree节点结构：                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  ┌────────┬────────┬────────┬────────┬────────┐    │   │
│  │  │ child0 │  key0  │ child1 │  key1  │ child2 │ ...│   │
│  │  │ (ptr)  │ (K,V)  │ (ptr)  │ (K,V)  │ (ptr)  │    │   │
│  │  └────────┴────────┴────────┴────────┴────────┘    │   │
│  │                                                      │   │
│  │  特点: 键-值对存储在内部节点和叶子节点               │   │
│  │                                                      │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  B+Tree节点结构：                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  内部节点:                                           │   │
│  │  ┌────────┬────────┬────────┬────────┐             │   │
│  │  │ child0 │  key0  │ child1 │  key1  │ ...         │   │
│  │  │ (ptr)  │ (sep)  │ (ptr)  │ (sep)  │             │   │
│  │  └────────┴────────┴────────┴────────┘             │   │
│  │  (仅存储分隔键，不存数据)                           │   │
│  │                                                      │   │
│  │  叶子节点:                                           │   │
│  │  ┌────────┬────────┬────────┬────────┬────────┐    │   │
│  │  │  key0  │ value0 │  key1  │ value1 │ next   │    │   │
│  │  │        │        │        │        │ (ptr)  │    │   │
│  │  └────────┴────────┴────────┴────────┴────────┘    │   │
│  │  (所有数据在叶子，叶子链表连接)                     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  复杂度分析：                                                 │
│  • 高度: O(log_m n)                                         │
│  • 查找: O(log n) 磁盘I/O                                    │
│  • 插入: O(log n) 磁盘I/O                                    │
│  • 删除: O(log n) 磁盘I/O                                    │
│  • 范围查询: O(log n + k) (k为结果数)                        │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 B-Tree vs其他数据结构

```
┌──────────────────┬──────────┬──────────┬─────────────────────┐
│ 特性             │ B-Tree   │ B+Tree   │ LSM-Tree            │
├──────────────────┼──────────┼──────────┼─────────────────────┤
│ 读性能           │ 优秀     │ 优秀     │ 良好                │
│ 写性能           │ 良好     │ 良好     │ 优秀                │
│ 范围查询         │ 良好     │ 优秀     │ 良好                │
│ 空间利用率       │ ~67%     │ ~67%     │ 高(压缩)            │
│ 写放大           │ 中       │ 中       │ 高                  │
│ 读放大           │ 低       │ 低       │ 中-高               │
│ 适用场景         │ OLTP     │ OLTP     │ OLAP/写密集         │
│ 典型数据库       │ SQLite   │ MySQL    │ RocksDB             │
│                  │ Postgre  │ InnoDB   │ LevelDB             │
└──────────────────┴──────────┴──────────┴─────────────────────┘
```

---

## 2. 存储引擎架构

### 2.1 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│              B-Tree存储引擎架构                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    SQL/NoSQL API                     │   │
│  └──────────────────────┬──────────────────────────────┘   │
│                         │                                    │
│  ┌──────────────────────▼──────────────────────────────┐   │
│  │                 Query Processor                      │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐            │   │
│  │  │ Parser   │ │ Planner  │ │ Executor │            │   │
│  │  └──────────┘ └──────────┘ └──────────┘            │   │
│  └──────────────────────┬──────────────────────────────┘   │
│                         │                                    │
│  ┌──────────────────────▼──────────────────────────────┐   │
│  │               Storage Engine Layer                   │   │
│  │  ┌───────────────────────────────────────────────┐  │   │
│  │  │  B-Tree Index Manager                         │  │   │
│  │  │  ┌─────────┐ ┌─────────┐ ┌─────────┐         │  │   │
│  │  │  │ Root    │ │Internal │ │ Leaf    │         │  │   │
│  │  │  │ Node    │ │ Nodes   │ │ Nodes   │         │  │   │
│  │  │  └─────────┘ └─────────┘ └─────────┘         │  │   │
│  │  └───────────────────────────────────────────────┘  │   │
│  │                                                      │   │
│  │  ┌───────────────────────────────────────────────┐  │   │
│  │  │  Transaction Manager (MVCC)                   │  │   │
│  │  │  ┌─────────┐ ┌─────────┐ ┌─────────┐         │  │   │
│  │  │  │ WAL     │ │ Lock    │ │ Version │         │  │   │
│  │  │  │ Log     │ │ Manager │ │ Chain   │         │  │   │
│  │  │  └─────────┘ └─────────┘ └─────────┘         │  │   │
│  │  └───────────────────────────────────────────────┘  │   │
│  │                                                      │   │
│  │  ┌───────────────────────────────────────────────┐  │   │
│  │  │  Buffer Pool Manager                          │  │   │
│  │  │  ┌─────────┐ ┌─────────┐ ┌─────────┐         │  │   │
│  │  │  │ Page    │ │ LRU     │ │ Flush   │         │  │   │
│  │  │  │ Table   │ │ Cache   │ │ Control │         │  │   │
│  │  │  └─────────┘ └─────────┘ └─────────┘         │  │   │
│  │  └───────────────────────────────────────────────┘  │   │
│  └──────────────────────┬──────────────────────────────┘   │
│                         │                                    │
│  ┌──────────────────────▼──────────────────────────────┐   │
│  │                 File System Layer                    │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐            │   │
│  │  │ Data File│ │ WAL File │ │ Catalog  │            │   │
│  │  │ (.db)    │ │ (.log)   │ │ (.cat)   │            │   │
│  │  └──────────┘ └──────────┘ └──────────┘            │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. 核心数据结构

### 3.1 页(Page)布局

```c
/* ============================================================
 * B-Tree页布局设计
 * ============================================================ */

#include <stdint.h>
#include <stdbool.h>

/* 页大小通常为4KB，匹配磁盘块大小 */
#define PAGE_SIZE 4096
#define MAX_KEYS_PER_PAGE 200  /* 假设键大小20字节 */

/* 页类型 */
typedef enum {
    PAGE_TYPE_META = 0,      /* 元数据页 */
    PAGE_TYPE_INTERNAL = 1,  /* 内部节点页 */
    PAGE_TYPE_LEAF = 2,      /* 叶子节点页 */
    PAGE_TYPE_OVERFLOW = 3,  /* 溢出页(大值) */
} page_type_t;

/* 页头 (固定大小，通常64-128字节) */
typedef struct __attribute__((packed)) {
    uint32_t magic;           /* 魔数，用于识别 */
    uint16_t page_type;       /* 页类型 */
    uint16_t flags;           /* 标志位 */
    uint32_t page_id;         /* 页ID */
    uint32_t parent_id;       /* 父页ID (0表示无) */
    uint16_t num_keys;        /* 键数量 */
    uint16_t free_space;      /* 空闲空间偏移 */
    uint32_t right_sibling;   /* 右兄弟页ID (B+Tree叶子) */
    uint32_t left_sibling;    /* 左兄弟页ID (B+Tree叶子) */
    uint64_t lsn;             /* 日志序列号 (WAL) */
    uint16_t checksum;        /* 校验和 */
} page_header_t;

/* 单元格头 (每个键值对) */
typedef struct __attribute__((packed)) {
    uint16_t key_size;        /* 键大小 */
    uint16_t value_size;      /* 值大小 */
    uint32_t overflow_page;   /* 溢出页ID (0表示无) */
} cell_header_t;

/* 页布局示意：
 *
 * ┌─────────────────────────────────────────────────────┐
 * │ 页头 (128 bytes)                                    │
 * ├─────────────────────────────────────────────────────┤
 * │ 单元格指针数组 (从页头后增长)                         │
 * │ ┌─────┬─────┬─────┬─────┐                          │
 * │ │off0 │off1 │off2 │ ... │                          │
 * │ └─────┴─────┴─────┴─────┘                          │
 * │         ▲                   ▼                       │
 * │         │              ┌─────────┐                  │
 * │         └──────────────│ Free    │                  │
 * │                        │ Space   │                  │
 * │ ┌──────────────────────┴─────────┘                  │
 * │ │ 单元格数据 (从页尾向前增长)                         │
 * │ │ ┌────────┬────────┬────────┬──────┐              │
 * │ │ │ Cell 2 │ Cell 1 │ Cell 0 │ ...  │              │
 * │ │ │ (key,  │ (key,  │ (key,  │      │              │
 * │ │ │ value) │ value) │ value) │      │              │
 * │ │ └────────┴────────┴────────┴──────┘              │
 * └─────────────────────────────────────────────────────┘
 *
 * 这种布局允许高效的插入和删除：
 * - 插入: 在指针数组添加一项，数据从尾部向前存储
 * - 删除: 移除指针数组一项，可能需要紧凑数据
 */

/* 页结构 */
typedef struct {
    page_header_t header;           /* 页头 */
    uint8_t data[PAGE_SIZE - sizeof(page_header_t)]; /* 页数据 */
} page_t;

/* B-Tree句柄 */
typedef struct {
    int fd;                         /* 数据文件描述符 */
    int wal_fd;                     /* WAL文件描述符 */
    uint32_t root_page_id;          /* 根页ID */
    uint32_t next_page_id;          /* 下一个空闲页ID */
    uint32_t meta_page_id;          /* 元数据页ID */
    /* ... 其他管理数据 ... */
} btree_t;
```

### 3.2 游标结构

```c
/* ============================================================
 * B-Tree游标 (用于遍历和定位)
 * ============================================================ */

/* 游标栈帧 (记录遍历路径) */
typedef struct {
    uint32_t page_id;       /* 页ID */
    uint16_t cell_index;    /* 页内单元格索引 */
} cursor_frame_t;

/* B-Tree游标 */
typedef struct {
    btree_t* tree;                      /* B-Tree句柄 */
    cursor_frame_t path[32];            /* 从根到叶子的路径 */
    uint16_t depth;                     /* 当前深度 */
    bool is_valid;                      /* 游标是否有效 */
} btree_cursor_t;

/* 游标操作 */

/* 定位到指定键 */
int btree_find(btree_t* tree, const void* key, size_t key_len,
               btree_cursor_t* cursor);

/* 移动到第一个键 */
int btree_first(btree_t* tree, btree_cursor_t* cursor);

/* 移动到最后一个键 */
int btree_last(btree_t* tree, btree_cursor_t* cursor);

/* 移动到下一个键 */
int btree_next(btree_cursor_t* cursor);

/* 移动到上一个键 */
int btree_prev(btree_cursor_t* cursor);

/* 获取当前键值 */
int btree_get(const btree_cursor_t* cursor,
              void** key, size_t* key_len,
              void** value, size_t* value_len);

/* 插入键值对 (在游标位置) */
int btree_insert(btree_t* tree, const void* key, size_t key_len,
                 const void* value, size_t value_len);

/* 删除当前键 */
int btree_delete(btree_cursor_t* cursor);
```

---

## 4. 核心算法实现

### 4.1 查找算法

```c
/* ============================================================
 * B-Tree查找算法
 * ============================================================ */

/* 在页内二分查找 */
static int page_binary_search(const page_t* page,
                               const void* key, size_t key_len,
                               uint16_t* out_index, bool* found) {
    uint16_t left = 0;
    uint16_t right = page->header.num_keys;

    while (left < right) {
        uint16_t mid = left + (right - left) / 2;

        /* 获取mid位置的键 */
        void* mid_key;
        size_t mid_key_len;
        page_get_key_at(page, mid, &mid_key, &mid_key_len);

        /* 比较 */
        int cmp = memcmp(key, mid_key,
                        key_len < mid_key_len ? key_len : mid_key_len);
        if (cmp == 0 && key_len != mid_key_len) {
            cmp = key_len < mid_key_len ? -1 : 1;
        }

        if (cmp < 0) {
            right = mid;
        } else if (cmp > 0) {
            left = mid + 1;
        } else {
            *out_index = mid;
            *found = true;
            return 0;
        }
    }

    *out_index = left;
    *found = false;
    return 0;
}

/* B-Tree查找 */
int btree_find(btree_t* tree, const void* key, size_t key_len,
               btree_cursor_t* cursor) {
    cursor->tree = tree;
    cursor->depth = 0;

    uint32_t page_id = tree->root_page_id;

    while (1) {
        /* 读取页 */
        page_t* page = buffer_pool_get_page(tree->bp, page_id);
        if (!page) return -1;

        /* 在页内查找 */
        uint16_t index;
        bool found;
        page_binary_search(page, key, key_len, &index, &found);

        /* 记录路径 */
        cursor->path[cursor->depth].page_id = page_id;
        cursor->path[cursor->depth].cell_index = index;
        cursor->depth++;

        if (page->header.page_type == PAGE_TYPE_LEAF) {
            /* 叶子节点，查找完成 */
            cursor->is_valid = found;
            buffer_pool_unpin_page(tree->bp, page);
            return found ? 0 : -1;  /* 0: 找到, -1: 未找到 */
        }

        /* 内部节点，继续向下 */
        if (found) {
            /* 精确匹配，向右子树走 */
            index++;
        }

        /* 获取子页ID */
        page_id = page_get_child_at(page, index);
        buffer_pool_unpin_page(tree->bp, page);
    }
}
```

### 4.2 插入算法

```c
/* ============================================================
 * B-Tree插入算法 (含分裂)
 * ============================================================ */

/* 页分裂 */
static int page_split(page_t* page, page_t* new_page,
                      void* mid_key, size_t* mid_key_len) {
    uint16_t num_keys = page->header.num_keys;
    uint16_t mid = num_keys / 2;

    /* 新页继承原页后半部分 */
    new_page->header.page_type = page->header.page_type;
    new_page->header.num_keys = num_keys - mid - 1;
    new_page->header.parent_id = page->header.parent_id;

    /* 复制后半部分到新页 */
    for (uint16_t i = mid + 1; i < num_keys; i++) {
        /* 复制单元格 */
        page_move_cell(page, i, new_page, i - mid - 1);
    }

    /* 提取中间键 */
    page_get_key_at(page, mid, &mid_key, mid_key_len);

    /* 更新原页 */
    page->header.num_keys = mid;

    /* 如果是叶子页，维护链表 */
    if (page->header.page_type == PAGE_TYPE_LEAF) {
        new_page->header.right_sibling = page->header.right_sibling;
        new_page->header.left_sibling = page->header.page_id;
        page->header.right_sibling = new_page->header.page_id;
    }

    return 0;
}

/* 递归插入 */
static int btree_insert_recursive(btree_t* tree, uint32_t page_id,
                                   const void* key, size_t key_len,
                                   const void* value, size_t value_len,
                                   void** split_key, size_t* split_key_len,
                                   uint32_t* split_page_id, bool* need_split) {
    page_t* page = buffer_pool_get_page(tree->bp, page_id);
    if (!page) return -1;

    if (page->header.page_type == PAGE_TYPE_LEAF) {
        /* 叶子节点，直接插入 */
        if (page_has_space(page, key_len, value_len)) {
            page_insert_cell(page, key, key_len, value, value_len);
            page->header.flags |= PAGE_FLAG_DIRTY;
            *need_split = false;
        } else {
            /* 需要分裂 */
            *need_split = true;
        }
        buffer_pool_unpin_page(tree->bp, page);
        return 0;
    }

    /* 内部节点，找到合适的子节点 */
    uint16_t index;
    bool found;
    page_binary_search(page, key, key_len, &index, &found);
    if (found) index++;

    uint32_t child_id = page_get_child_at(page, index);

    /* 递归插入 */
    void* child_split_key = NULL;
    size_t child_split_key_len = 0;
    uint32_t child_split_page_id = 0;
    bool child_need_split = false;

    int rc = btree_insert_recursive(tree, child_id, key, key_len,
                                     value, value_len,
                                     &child_split_key, &child_split_key_len,
                                     &child_split_page_id, &child_need_split);

    if (rc != 0 || !child_need_split) {
        buffer_pool_unpin_page(tree->bp, page);
        return rc;
    }

    /* 子节点分裂，需要在本页插入分裂键 */
    if (page_has_space(page, child_split_key_len, sizeof(uint32_t))) {
        /* 插入分裂键和右子页指针 */
        page_insert_internal(page, index, child_split_key, child_split_key_len,
                            child_split_page_id);
        page->header.flags |= PAGE_FLAG_DIRTY;
        *need_split = false;
    } else {
        /* 本页也需要分裂 */
        *need_split = true;
        /* ... 处理本页分裂 ... */
    }

    buffer_pool_unpin_page(tree->bp, page);
    return 0;
}

/* 顶层插入接口 */
int btree_insert(btree_t* tree, const void* key, size_t key_len,
                 const void* value, size_t value_len) {
    /* 写入WAL (Write-Ahead Logging) */
    wal_append_insert(tree->wal, key, key_len, value, value_len);

    void* split_key = NULL;
    size_t split_key_len = 0;
    uint32_t split_page_id = 0;
    bool need_split = false;

    int rc = btree_insert_recursive(tree, tree->root_page_id,
                                     key, key_len, value, value_len,
                                     &split_key, &split_key_len,
                                     &split_page_id, &need_split);

    if (rc == 0 && need_split) {
        /* 根节点分裂，创建新根 */
        uint32_t new_root_id = alloc_page(tree);
        page_t* new_root = buffer_pool_get_page(tree->bp, new_root_id);

        new_root->header.page_type = PAGE_TYPE_INTERNAL;
        new_root->header.num_keys = 1;
        /* 设置新根的子节点 */
        page_set_child_at(new_root, 0, tree->root_page_id);
        page_set_key_at(new_root, 0, split_key, split_key_len);
        page_set_child_at(new_root, 1, split_page_id);

        /* 更新元数据 */
        tree->root_page_id = new_root_id;
        meta_page_update_root(tree, new_root_id);

        buffer_pool_unpin_page(tree->bp, new_root);
    }

    return rc;
}
```

---

## 5. 事务与并发控制

### 5.1 MVCC实现

```c
/* ============================================================
 * 多版本并发控制 (MVCC)
 * ============================================================ */

/* 版本链头 */
typedef struct version_header {
    uint64_t tx_id;             /* 创建此版本的事务ID */
    uint64_t begin_ts;          /* 版本开始时间戳 */
    uint64_t end_ts;            /* 版本结束时间戳 (0表示当前版本) */
    uint32_t next_version;      /* 下一个版本的溢出页 (0表示无) */
} version_header_t;

/* 事务状态 */
typedef enum {
    TX_ACTIVE = 0,      /* 活跃 */
    TX_COMMITTING = 1,  /* 提交中 */
    TX_COMMITTED = 2,   /* 已提交 */
    TX_ABORTED = 3,     /* 已回滚 */
} tx_state_t;

/* 事务 */
typedef struct {
    uint64_t tx_id;             /* 事务ID (单调递增) */
    uint64_t read_ts;           /* 读时间戳 */
    uint64_t commit_ts;         /* 提交时间戳 */
    tx_state_t state;           /* 事务状态 */
    /* 写集 */
    struct write_entry* write_set;
    uint32_t write_set_size;
} transaction_t;

/* MVCC读操作 */
int mvcc_get(transaction_t* tx, btree_t* tree,
             const void* key, size_t key_len,
             void** value, size_t* value_len) {
    /* 查找键 */
    btree_cursor_t cursor;
    int rc = btree_find(tree, key, key_len, &cursor);
    if (rc != 0) return -1;  /* 未找到 */

    /* 遍历版本链，找到对当前事务可见的版本 */
    page_t* page = buffer_pool_get_page(tree->bp,
                                        cursor.path[cursor.depth-1].page_id);
    uint16_t cell_idx = cursor.path[cursor.depth-1].cell_index;

    while (1) {
        void* cell_value;
        size_t cell_value_len;
        version_header_t* ver;

        page_get_value_at(page, cell_idx, &cell_value, &cell_value_len);
        ver = (version_header_t*)cell_value;

        /* 可见性判断 */
        if (ver->begin_ts <= tx->read_ts &&
            (ver->end_ts == 0 || ver->end_ts > tx->read_ts)) {
            /* 此版本对当前事务可见 */
            *value = (char*)cell_value + sizeof(version_header_t);
            *value_len = cell_value_len - sizeof(version_header_t);
            buffer_pool_unpin_page(tree->bp, page);
            return 0;
        }

        /* 跳到下一个版本 */
        if (ver->next_version == 0) break;
        /* ... 读取溢出页 ... */
    }

    buffer_pool_unpin_page(tree->bp, page);
    return -1;  /* 没有可见版本 */
}

/* MVCC写操作 */
int mvcc_put(transaction_t* tx, btree_t* tree,
             const void* key, size_t key_len,
             const void* value, size_t value_len) {
    /* 检查写写冲突 */
    btree_cursor_t cursor;
    int rc = btree_find(tree, key, key_len, &cursor);

    if (rc == 0) {
        /* 键已存在，需要更新 */
        /* 获取当前版本 */
        page_t* page = buffer_pool_get_page(tree->bp,
                                            cursor.path[cursor.depth-1].page_id);
        /* ... 检查冲突 ... */
        buffer_pool_unpin_page(tree->bp, page);

        /* 添加新版本 */
        /* ... */
    } else {
        /* 新键，直接插入 */
        /* ... */
    }

    /* 记录到写集 */
    write_set_add(tx, key, key_len, value, value_len);

    return 0;
}
```

### 5.2 WAL (Write-Ahead Logging)

```c
/* ============================================================
 * 预写日志 (WAL) 实现
 * ============================================================ */

/* WAL记录类型 */
typedef enum {
    WAL_BEGIN = 1,      /* 事务开始 */
    WAL_INSERT = 2,     /* 插入 */
    WAL_UPDATE = 3,     /* 更新 */
    WAL_DELETE = 4,     /* 删除 */
    WAL_COMMIT = 5,     /* 提交 */
    WAL_ROLLBACK = 6,   /* 回滚 */
    WAL_CHECKPOINT = 7, /* 检查点 */
} wal_record_type_t;

/* WAL记录头 */
typedef struct __attribute__((packed)) {
    uint32_t checksum;          /* CRC32校验 */
    uint32_t record_len;        /* 记录总长度 */
    uint64_t lsn;               /* 日志序列号 */
    uint64_t tx_id;             /* 事务ID */
    uint8_t type;               /* 记录类型 */
    /* 后跟变长数据 */
} wal_record_header_t;

/* WAL写入 */
int wal_append(wal_t* wal, uint8_t type, uint64_t tx_id,
               const void* data, size_t data_len) {
    /* 构造记录 */
    size_t record_size = sizeof(wal_record_header_t) + data_len;
    uint8_t* record = malloc(record_size);

    wal_record_header_t* header = (wal_record_header_t*)record;
    header->record_len = record_size;
    header->lsn = wal->next_lsn++;
    header->tx_id = tx_id;
    header->type = type;

    memcpy(record + sizeof(wal_record_header_t), data, data_len);
    header->checksum = crc32(record + sizeof(uint32_t),
                             record_size - sizeof(uint32_t));

    /* 写入文件 (使用O_DIRECT保证落盘) */
    ssize_t written = pwrite(wal->fd, record, record_size, wal->write_offset);
    if (written != record_size) {
        free(record);
        return -1;
    }

    wal->write_offset += written;

    /* 事务提交时强制刷盘 */
    if (type == WAL_COMMIT) {
        fsync(wal->fd);
    }

    free(record);
    return 0;
}

/* 崩溃恢复 */
int wal_recover(wal_t* wal, btree_t* tree) {
    off_t offset = 0;
    struct stat st;
    fstat(wal->fd, &st);

    while (offset < st.st_size) {
        /* 读取记录头 */
        wal_record_header_t header;
        pread(wal->fd, &header, sizeof(header), offset);

        /* 校验 */
        uint32_t checksum = crc32(&header + sizeof(uint32_t),
                                  header.record_len - sizeof(uint32_t));
        if (checksum != header.checksum) {
            /* 损坏的记录，停止恢复 */
            break;
        }

        /* 读取记录数据 */
        uint8_t* data = malloc(header.record_len - sizeof(header));
        pread(wal->fd, data, header.record_len - sizeof(header),
              offset + sizeof(header));

        /* 重放操作 */
        switch (header.type) {
            case WAL_INSERT:
                redo_insert(tree, data);
                break;
            case WAL_UPDATE:
                redo_update(tree, data);
                break;
            case WAL_DELETE:
                redo_delete(tree, data);
                break;
            /* ... */
        }

        free(data);
        offset += header.record_len;
    }

    return 0;
}
```

---

## 6. 性能优化

### 6.1 缓存管理

```c
/* ============================================================
 * 缓冲池管理 (LRU-K算法)
 * ============================================================ */

typedef struct buffer_frame {
    uint32_t page_id;           /* 页ID */
    page_t* page;               /* 页数据 */
    int pin_count;              /* 引用计数 */
    bool is_dirty;              /* 是否脏页 */
    uint64_t last_access[2];    /* 最近两次访问时间 */
    struct buffer_frame* next;  /* LRU链表 */
    struct buffer_frame* prev;
} buffer_frame_t;

typedef struct {
    buffer_frame_t* frames;     /* 帧数组 */
    int capacity;               /* 容量 */
    int size;                   /* 当前大小 */
    buffer_frame_t* lru_head;   /* LRU链表头 */
    buffer_frame_t* lru_tail;   /* LRU链表尾 */
    pthread_mutex_t lock;
} buffer_pool_t;

/* 选择牺牲页 (LRU-K) */
static buffer_frame_t* choose_victim(buffer_pool_t* bp) {
    buffer_frame_t* victim = NULL;
    uint64_t max_score = 0;

    for (int i = 0; i < bp->size; i++) {
        buffer_frame_t* frame = &bp->frames[i];
        if (frame->pin_count > 0) continue;  /* 被引用，不能淘汰 */

        /* LRU-K 评分 */
        uint64_t score = frame->last_access[1] * 2 + frame->last_access[0];
        if (score > max_score) {
            max_score = score;
            victim = frame;
        }
    }

    return victim;
}

/* 获取页 */
page_t* buffer_pool_get_page(buffer_pool_t* bp, uint32_t page_id) {
    pthread_mutex_lock(&bp->lock);

    /* 查找 */
    for (int i = 0; i < bp->size; i++) {
        if (bp->frames[i].page_id == page_id) {
            bp->frames[i].pin_count++;
            /* 更新访问时间 */
            bp->frames[i].last_access[1] = bp->frames[i].last_access[0];
            bp->frames[i].last_access[0] = get_time();
            pthread_mutex_unlock(&bp->lock);
            return bp->frames[i].page;
        }
    }

    /* 需要从磁盘加载 */
    buffer_frame_t* frame = NULL;
    if (bp->size < bp->capacity) {
        /* 还有空位 */
        frame = &bp->frames[bp->size++];
    } else {
        /* 需要淘汰 */
        frame = choose_victim(bp);
        if (frame->is_dirty) {
            /* 刷脏页 */
            flush_page(frame);
        }
    }

    /* 加载新页 */
    frame->page_id = page_id;
    frame->pin_count = 1;
    frame->is_dirty = false;
    frame->last_access[0] = get_time();
    frame->last_access[1] = 0;
    read_page_from_disk(bp->fd, page_id, frame->page);

    pthread_mutex_unlock(&bp->lock);
    return frame->page;
}
```

---

## 7. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                                                              │
│  B-Tree存储引擎设计原则：                                     │
│                                                              │
│  1. 页式存储 - 4KB对齐，顺序访问优化                         │
│  2. 平衡树 - 保证O(log n)的读写性能                         │
│  3. 延迟刷盘 - WAL保证持久性，批量写页                       │
│  4. MVCC - 读不阻塞写，高并发                                │
│  5. 缓存管理 - LRU-K减少磁盘I/O                              │
│                                                              │
│  生产级数据库特性：                                           │
│  □ 并发控制 (行锁、表锁、意向锁)                            │
│  □ 死锁检测与超时                                            │
│  □ 在线备份与恢复                                            │
│  □ 压缩与加密                                                │
│  □ 复制与高可用                                              │
│  □ 查询优化器                                                │
│                                                              │
│  学习资源：                                                   │
│  • SQLite源码 (最佳入门)                                     │
│  • MySQL InnoDB源码                                          │
│  • 《Database Internals》                                    │
│  • CMU 15-445 (Database Systems)                             │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

*存储引擎是数据库系统的核心，B-Tree是最经典、最实用的索引结构之一。*

# B-Tree实现深度解析

> **层级定位**: 03 System Technology Domains / 18 Database Internals
> **参考实现**: SQLite, InnoDB, LMDB
> **难度级别**: L4-L5

---

## 一、B-Tree理论基础

### 1.1 B-Tree vs B+Tree

```
B-Tree (原始)：
• 键和数据存储在所有节点
• 范围查询效率低（需要遍历树）
• 空间利用率问题

B+Tree (数据库标准)：
• 键分布在所有节点
• 数据只存储在叶子节点
• 叶子节点形成链表（范围查询优化）
• 更高的扇出（fanout），更矮的树

节点结构对比：

B-Tree节点：
┌────────┬────────┬────────┬────────┐
│ 键0    │ 数据0  │ 键1    │ 数据1  │ ...
└────────┴────────┴────────┴────────┘
占用更多空间，扇出低

B+Tree内部节点：
┌────────┬────────┬────────┬────────┐
│ 键0    │ 指针0  │ 键1    │ 指针1  │ ...
└────────┴────────┴────────┴────────┘
仅存储键，扇出高

B+Tree叶子节点：
┌────────┬────────┬────────┬────────┬────────┐
│ 键0    │ 数据0  │ 键1    │ 数据1  │ 下一个 │
└────────┴────────┴────────┴────────┴────────┘
数据紧凑，支持顺序扫描
```

### 1.2 B+Tree不变式

```
B+Tree必须维持的不变式：

┌─────────────────────────────────────────────────────────────┐
│  1. 平衡性：所有叶子节点在同一深度                           │
│     • 保证O(log n)的查询复杂度                                │
│     • 插入/删除通过分裂/合并维持                              │
├─────────────────────────────────────────────────────────────┤
│  2. 节点填充率：                                             │
│     • 最小填充率：通常50% (除根节点)                         │
│     • 最大键数：M-1 (M为阶数)                                │
│     • 保证空间利用率                                         │
├─────────────────────────────────────────────────────────────┤
│  3. 键的排序：                                               │
│     • 内部节点：左子树所有键 < 节点键 < 右子树所有键         │
│     • 叶子节点：按键排序，形成有序链表                        │
├─────────────────────────────────────────────────────────────┤
│  4. 指针数量：                                               │
│     • 内部节点：子指针数 = 键数 + 1                          │
│     • 叶子节点：键数 = 数据项数                              │
└─────────────────────────────────────────────────────────────┘

数学分析：
  设阶数为 M，树高为 h

  最大键数 = M^h - 1
  最小键数 = 2 × ⌈M/2⌉^(h-1) - 1

  对于 M=200, h=3:
  最大：8,000,000 键
  最小：20,000 键

  查询复杂度：
  磁盘I/O = h + 1 (叶子数据)
  通常 h ≤ 4，即使数十亿数据
```

---

## 二、B+Tree实现

### 2.1 节点布局

```c
/*
 * B+Tree节点布局 (磁盘友好)
 */

#define BTREE_PAGE_SIZE     4096    // 4KB页
#define BTREE_MAX_KEYS      200     // 阶数
#define BTREE_MIN_KEYS      100     // 50%填充

typedef enum {
    NODE_TYPE_INTERNAL = 0,
    NODE_TYPE_LEAF = 1
} node_type_t;

// 页头 (固定大小)
typedef struct {
    uint32_t magic;          // 魔数验证
    uint16_t type;           // 节点类型
    uint16_t num_keys;       // 当前键数
    uint32_t parent_page;    // 父节点页号 (0=根)
    uint32_t next_page;      // 右兄弟页号 (叶子用)
    uint32_t prev_page;      // 左兄弟页号
    uint16_t free_offset;    // 空闲空间偏移
    uint16_t flags;          // 标记位
} page_header_t;

// 单元指针数组 (从页尾向前增长)
// 每个单元：偏移量(2B) + 大小(2B)

typedef struct {
    page_header_t header;
    uint8_t data[BTREE_PAGE_SIZE - sizeof(page_header_t)];
} page_t;

// 单元格式：
// ┌──────────┬──────────┬──────────┐
// │ 键大小   │ 键数据   │ 值/指针  │
// │ (2B)     │ (变长)   │ (变长)   │
// └──────────┴──────────┴──────────┘

// 页内布局：
// ┌──────────┬──────────────────┬──────────────────┐
// │ 页头     │ 数据单元(向前增长) │ 指针数组(向后增长) │
// │ 固定大小  │                  │                  │
// └──────────┴──────────────────┴──────────────────┘
// 0          free_offset          BTREE_PAGE_SIZE
```

### 2.2 核心操作实现

```c
/*
 * 搜索操作
 */
int btree_search(btree_t *tree, const void *key, void *value) {
    uint32_t page_num = tree->root_page;
    page_t *page = load_page(tree->pager, page_num);

    // 从根向下遍历
    while (page->header.type == NODE_TYPE_INTERNAL) {
        int idx = find_key_index(page, key);
        uint32_t child_page = get_child_pointer(page, idx);
        page = load_page(tree->pager, child_page);
    }

    // 在叶子节点搜索
    int idx = find_key_index(page, key);
    if (idx >= 0 && compare_key(page, idx, key) == 0) {
        get_value(page, idx, value);
        return 0;  // 找到
    }
    return -1;  // 未找到
}

/*
 * 插入操作（含分裂）
 */
int btree_insert(btree_t *tree, const void *key, const void *value) {
    uint32_t page_num = tree->root_page;
    page_t *page = load_page(tree->pager, page_num);

    // 如果根满，先分裂根
    if (page->header.num_keys >= BTREE_MAX_KEYS) {
        split_root(tree);
        page = load_page(tree->pager, tree->root_page);
    }

    return insert_non_full(tree, page_num, key, value);
}

static int insert_non_full(btree_t *tree, uint32_t page_num,
                           const void *key, const void *value) {
    page_t *page = load_page(tree->pager, page_num);

    if (page->header.type == NODE_TYPE_LEAF) {
        // 直接插入叶子
        return insert_into_leaf(page, key, value);
    }

    // 找到合适的子节点
    int idx = find_insert_index(page, key);
    uint32_t child_page = get_child_pointer(page, idx);
    page_t *child = load_page(tree->pager, child_page);

    // 如果子节点满，先分裂
    if (child->header.num_keys >= BTREE_MAX_KEYS) {
        split_child(tree, page_num, idx);

        // 重新确定插入哪个子节点
        if (compare_key(key, get_key(page, idx)) > 0) {
            idx++;
        }
        child_page = get_child_pointer(page, idx);
    }

    return insert_non_full(tree, child_page, key, value);
}

/*
 * 节点分裂（核心操作）
 */
static void split_child(btree_t *tree, uint32_t parent_page,
                        int child_idx) {
    page_t *parent = load_page(tree->pager, parent_page);
    uint32_t full_child = get_child_pointer(parent, child_idx);
    page_t *child = load_page(tree->pager, full_child);

    // 分配新节点
    uint32_t new_child = allocate_page(tree);
    page_t *new_page = init_page(new_child, child->header.type);

    // 移动一半键到新节点
    int mid = child->header.num_keys / 2;
    move_cells(child, mid, new_page, 0, child->header.num_keys - mid);

    // 如果是内部节点，提升中间键到父节点
    if (child->header.type == NODE_TYPE_INTERNAL) {
        void *mid_key = get_key(child, mid);
        insert_into_internal(parent, child_idx, mid_key, new_child);
        child->header.num_keys = mid;  // 保留左半部分
    } else {
        // 叶子节点：复制中间键到父节点
        void *mid_key = get_key(child, mid);
        insert_into_internal(parent, child_idx, mid_key, new_child);

        // 维护叶子链表
        new_page->header.next_page = child->header.next_page;
        child->header.next_page = new_child;
        new_page->header.prev_page = full_child;

        child->header.num_keys = mid;
    }

    mark_dirty(tree->pager, parent_page);
    mark_dirty(tree->pager, full_child);
    mark_dirty(tree->pager, new_child);
}
```

### 2.3 并发控制

```c
/*
 * B-Tree并发策略：
 *
 * 选项1: 粗粒度锁 (简单但扩展性差)
 * 选项2: 乐观锁存 (latch coupling)
 * 选项3: B-Link树 (允许无锁遍历)
 *
 * 实现：乐观锁存
 */

typedef struct {
    pthread_rwlock_t rwlock;
    volatile uint32_t version;  // 乐观锁版本
} page_lock_t;

// 搜索时的锁获取（从上到下，获取-释放）
int btree_search_concurrent(btree_t *tree, const void *key, void *value) {
    uint32_t page_num = tree->root_page;
    page_lock_t *parent_lock = NULL;

    // 获取根节点读锁
    page_lock_t *lock = get_page_lock(tree, page_num);
    read_lock(lock);

    while (1) {
        page_t *page = load_page(tree->pager, page_num);

        if (page->header.type == NODE_TYPE_LEAF) {
            // 叶子节点，搜索完成
            int result = search_in_page(page, key, value);
            read_unlock(lock);
            return result;
        }

        // 找到子节点
        int idx = find_key_index(page, key);
        uint32_t child_page = get_child_pointer(page, idx);
        page_lock_t *child_lock = get_page_lock(tree, child_page);

        // 获取子节点锁
        read_lock(child_lock);

        // 释放父节点锁（锁耦合）
        read_unlock(lock);

        page_num = child_page;
        lock = child_lock;
    }
}
```

---

## 关联导航

- [内存管理](../01_System_Programming/07_Memory_Allocator_Design.md)
- [并发控制](../14_Concurrency_Parallelism/readme.md)

## 参考

- Database Internals, Alex Petrov
- SQLite Architecture (<https://sqlite.org/arch.html>)

/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_B_Tree_Index.md
 * Line: 151
 * Language: c
 * Block ID: 60c2dca4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * B+树索引实现 - C11标准
 * 完整功能：CRUD操作、迭代器、并发控制、内存管理
 */

#ifndef BTREE_H
#define BTREE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 配置常量 ==================== */

#define BTREE_ORDER        128          /* B+树阶数，决定节点容量 */
#define BTREE_MIN_KEYS     (BTREE_ORDER / 2)  /* 最小键数 */
#define BTREE_KEY_SIZE     32           /* 键最大长度 */
#define BTREE_VALUE_SIZE   256          /* 值最大长度 */
#define BTREE_MAX_HEIGHT   16           /* 最大树高度 */
#define BTREE_NODE_POOL_SIZE 1024       /* 节点池初始大小 */
#define BTREE_PAGE_SIZE    4096         /* 页大小 */

/* ==================== 错误码定义 ==================== */

typedef enum {
    BTREE_OK = 0,               /* 成功 */
    BTREE_ERR_NOMEM = -1,       /* 内存不足 */
    BTREE_ERR_NOTFOUND = -2,    /* 键不存在 */
    BTREE_ERR_DUPLICATE = -3,   /* 重复键 */
    BTREE_ERR_IO = -4,          /* IO错误 */
    BTREE_ERR_CORRUPT = -5,     /* 数据损坏 */
    BTREE_ERR_LOCK = -6,        /* 锁错误 */
    BTREE_ERR_INVALID = -7      /* 无效参数 */
} BTreeError;

/* ==================== 类型定义 ==================== */

typedef uint64_t PageId;        /* 页ID类型 */
typedef uint64_t BTreeVersion;  /* 版本号类型 */

/* 节点类型标志 */
typedef enum {
    NODE_TYPE_INTERNAL = 0,     /* 内部节点 */
    NODE_TYPE_LEAF = 1,         /* 叶子节点 */
    NODE_TYPE_ROOT = 2          /* 根节点 */
} NodeType;

/* ==================== 节点头部结构 ==================== */

/*
 * B+树节点头部 - 16字节对齐
 * 包含节点元数据，所有节点类型共用
 */
typedef struct __attribute__((aligned(16))) {
    uint32_t magic;             /* 魔数，用于验证 */
    uint16_t flags;             /* 节点标志 */
    uint16_t num_keys;          /* 当前键数量 */
    uint8_t  level;             /* 节点层级，0=叶子 */
    uint8_t  type;              /* 节点类型 */
    uint16_t padding;           /* 填充对齐 */
    PageId   parent;            /* 父节点页ID */
    PageId   next;              /* 下一个兄弟节点（叶子用） */
    PageId   prev;              /* 上一个兄弟节点（叶子用） */
    BTreeVersion version;       /* 版本号，用于乐观锁 */
} BTreeNodeHeader;

/* 魔数常量 */
#define BTREE_NODE_MAGIC    0x4254EE55U  /* "BTREE" */

/* ==================== 键值对结构 ==================== */

/*
 * 变长键结构
 * 支持前缀压缩的键存储
 */
typedef struct {
    uint16_t length;                    /* 键长度 */
    uint16_t prefix_len;                /* 前缀长度（压缩用） */
    uint8_t  data[BTREE_KEY_SIZE];      /* 键数据 */
} BTreeKey;

/*
 * 内部节点条目 - 键 + 子节点指针
 */
typedef struct {
    BTreeKey key;                       /* 分隔键 */
    PageId   child;                     /* 子节点页ID */
} InternalEntry;

/*
 * 叶子节点条目 - 键 + 值
 */
typedef struct {
    BTreeKey key;                       /* 键 */
    uint16_t value_len;                 /* 值长度 */
    uint8_t  value[BTREE_VALUE_SIZE];   /* 值数据 */
} LeafEntry;

/* ==================== 完整节点结构 ==================== */

/*
 * B+树节点 - 4KB页大小
 * 内部节点和叶子节点共用结构，通过header.type区分
 */
typedef struct __attribute__((aligned(64))) {
    BTreeNodeHeader header;             /* 头部信息 */
    union {
        InternalEntry internal[BTREE_ORDER];    /* 内部节点条目 */
        LeafEntry     leaf[BTREE_ORDER];        /* 叶子节点条目 */
    } entries;
} BTreeNode;

/* 验证节点大小 */
static_assert(sizeof(BTreeNode) <= BTREE_PAGE_SIZE,
              "BTreeNode size exceeds page size");

/* ==================== 内存管理结构 ==================== */

/*
 * 节点池 - 预分配节点缓存
 */
typedef struct {
    BTreeNode* nodes;                   /* 节点数组 */
    uint32_t*  free_list;               /* 空闲列表 */
    size_t     capacity;                /* 总容量 */
    size_t     used;                    /* 已使用数量 */
    pthread_mutex_t lock;               /* 池锁 */
} NodePool;

/*
 * 页分配器 - 管理页ID分配
 */
typedef struct {
    PageId    next_page_id;             /* 下一个可用页ID */
    PageId*   freed_pages;              /* 回收的页ID */
    size_t    freed_count;              /* 回收数量 */
    size_t    freed_capacity;           /* 回收数组容量 */
    pthread_mutex_t lock;               /* 分配器锁 */
} PageAllocator;

/* ==================== 并发控制结构 ==================== */

/*
 * 读写锁包装 - 支持统计和调试
 */
typedef struct {
    pthread_rwlock_t lock;              /* POSIX读写锁 */
    atomic_int       readers;           /* 当前读者数 */
    atomic_int       writers;           /* 当前写者数 */
} BTreeRwLock;

/*
 * 乐观锁 - 无锁读取
 */
typedef struct {
    atomic_ullong version;              /* 版本号 */
} OptimisticLock;

/* ==================== B+树主结构 ==================== */

typedef struct BTree BTree;

/*
 * B+树结构
 */
struct BTree {
    /* 根节点 */
    BTreeNode* root;                    /* 根节点指针 */
    PageId     root_page_id;            /* 根节点页ID */

    /* 内存管理 */
    NodePool       node_pool;           /* 节点池 */
    PageAllocator  page_allocator;      /* 页分配器 */

    /* 并发控制 */
    BTreeRwLock    tree_lock;           /* 树级锁 */
    bool           use_optimistic;      /* 是否使用乐观锁 */

    /* 统计信息 */
    struct {
        atomic_ullong inserts;          /* 插入次数 */
        atomic_ullong deletes;          /* 删除次数 */
        atomic_ullong searches;         /* 搜索次数 */
        atomic_ullong splits;           /* 分裂次数 */
        atomic_ullong merges;           /* 合并次数 */
        uint32_t      height;           /* 当前树高度 */
    } stats;

    /* 配置 */
    uint16_t order;                     /* B+树阶数 */
    uint16_t min_keys;                  /* 最小键数 */
};

/* ==================== 迭代器结构 ==================== */

/*
 * B+树迭代器 - 支持范围遍历
 */
typedef struct {
    BTree*      tree;                   /* 所属B+树 */
    BTreeNode*  current_node;           /* 当前节点 */
    int         current_index;          /* 当前索引 */
    BTreeKey    start_key;              /* 起始键 */
    BTreeKey    end_key;                /* 结束键 */
    bool        forward;                /* 正向遍历 */
    bool        valid;                  /* 迭代器是否有效 */
    BTreeVersion version;               /* 快照版本 */
} BTreeIterator;

/* ==================== 函数声明 ==================== */

/* 生命周期管理 */
BTree* btree_create(uint16_t order);
void   btree_destroy(BTree* tree);
int    btree_init(BTree* tree, uint16_t order);
void   btree_cleanup(BTree* tree);

/* CRUD操作 */
int btree_insert(BTree* tree, const uint8_t* key, uint16_t key_len,
                 const uint8_t* value, uint16_t value_len);
int btree_search(BTree* tree, const uint8_t* key, uint16_t key_len,
                 uint8_t* value, uint16_t* value_len);
int btree_delete(BTree* tree, const uint8_t* key, uint16_t key_len);
int btree_update(BTree* tree, const uint8_t* key, uint16_t key_len,
                 const uint8_t* value, uint16_t value_len);

/* 范围查询 */
int btree_range_search(BTree* tree,
                       const uint8_t* start_key, uint16_t start_len,
                       const uint8_t* end_key, uint16_t end_len,
                       int (*callback)(const uint8_t* key, uint16_t key_len,
                                      const uint8_t* value, uint16_t value_len,
                                      void* user_data),
                       void* user_data);

/* 迭代器 */
BTreeIterator* btree_iterator_create(BTree* tree,
                                     const uint8_t* start_key, uint16_t start_len,
                                     const uint8_t* end_key, uint16_t end_len,
                                     bool forward);
void           btree_iterator_destroy(BTreeIterator* iter);
bool           btree_iterator_next(BTreeIterator* iter);
bool           btree_iterator_prev(BTreeIterator* iter);
const LeafEntry* btree_iterator_get(BTreeIterator* iter);

/* 工具函数 */
void btree_print_stats(BTree* tree);
int  btree_validate(BTree* tree);
void btree_dump(BTree* tree);

#ifdef __cplusplus
}
#endif

#endif /* BTREE_H */

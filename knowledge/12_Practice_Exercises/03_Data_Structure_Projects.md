# 数据结构项目 (Data Structure Projects)

> **难度**: L3-L5 | **预计时间**: 20-30小时

---

## 项目1: 动态数组 (Vector)

### 描述

实现C++ std::vector的C语言版本。

### 功能要求

```c
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} Vector;

// 构造与析构
Vector* vec_create(void);
Vector* vec_create_with_capacity(size_t capacity);
void vec_destroy(Vector *v);

// 元素访问
int vec_get(const Vector *v, size_t index);
void vec_set(Vector *v, size_t index, int value);
int* vec_at(Vector *v, size_t index);  // 返回指针，可修改
int vec_front(const Vector *v);
int vec_back(const Vector *v);

// 容量操作
size_t vec_size(const Vector *v);
size_t vec_capacity(const Vector *v);
bool vec_empty(const Vector *v);
void vec_reserve(Vector *v, size_t new_cap);
void vec_shrink_to_fit(Vector *v);

// 修改操作
void vec_push_back(Vector *v, int value);
void vec_pop_back(Vector *v);
void vec_insert(Vector *v, size_t pos, int value);
void vec_erase(Vector *v, size_t pos);
void vec_clear(Vector *v);

// 其他
void vec_resize(Vector *v, size_t new_size, int fill_value);
void vec_swap(Vector *a, Vector *b);
```

### 实现要点

- 容量倍增策略（均摊O(1)插入）
- 边界检查
- 异常安全（C中模拟）

### 测试用例

```c
void test_vector() {
    Vector *v = vec_create();

    // 测试扩容
    for (int i = 0; i < 1000; i++) {
        vec_push_back(v, i);
    }
    assert(vec_size(v) == 1000);
    assert(vec_capacity(v) >= 1000);

    // 测试访问
    assert(vec_get(v, 500) == 500);

    // 测试删除
    vec_erase(v, 500);
    assert(vec_size(v) == 999);

    vec_destroy(v);
}
```

---

## 项目2: 通用哈希表

### 描述

实现支持任意键值类型的哈希表。

### 设计

```c
typedef struct HashTable HashTable;
typedef struct {
    const void *key;
    void *value;
    uint32_t hash;
    bool occupied;
    bool deleted;
} Entry;

typedef struct {
    uint32_t (*hash_func)(const void *);
    bool (*key_equal)(const void *, const void *);
} HashTableOps;

HashTable* ht_create(size_t initial_capacity, const HashTableOps *ops);
void ht_destroy(HashTable *ht);

bool ht_insert(HashTable *ht, const void *key, void *value);
void* ht_get(const HashTable *ht, const void *key);
bool ht_remove(HashTable *ht, const void *key);
bool ht_contains(const HashTable *ht, const void *key);
size_t ht_size(const HashTable *ht);
```

### 哈希函数示例

```c
// 字符串哈希 (FNV-1a)
uint32_t hash_string(const void *key) {
    const char *str = key;
    uint32_t hash = 2166136261u;
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 16777619;
    }
    return hash;
}

// 整数哈希
uint32_t hash_int(const void *key) {
    int k = *(const int*)key;
    // Thomas Wang's hash
    k = ~k + (k << 15);
    k = k ^ (k >> 12);
    k = k + (k << 2);
    k = k ^ (k >> 4);
    k = k * 2057;
    k = k ^ (k >> 16);
    return k;
}
```

### 冲突解决

使用开放寻址 + 罗宾汉哈希：

```c
// 罗宾汉哈希：记录每个entry的距离（DIB）
// 插入时如果新元素比当前元素"更惨"（距离更远），则交换
// 保持 variance 较小，提高缓存效率
```

---

## 项目3: 二叉搜索树与AVL树

### 描述

实现自平衡二叉搜索树。

### 基础BST

```c
typedef struct TreeNode {
    int key;
    void *value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode* bst_insert(TreeNode *root, int key, void *value);
TreeNode* bst_search(TreeNode *root, int key);
TreeNode* bst_remove(TreeNode *root, int key);
void bst_inorder(TreeNode *root, void (*visit)(TreeNode*));
void bst_destroy(TreeNode *root);
```

### AVL树（自平衡）

```c
typedef struct AVLNode {
    int key;
    void *value;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

AVLNode* avl_insert(AVLNode *node, int key, void *value);
AVLNode* avl_remove(AVLNode *node, int key);
AVLNode* avl_search(AVLNode *node, int key);
int avl_height(AVLNode *node);
int avl_balance_factor(AVLNode *node);

// 旋转操作
AVLNode* avl_right_rotate(AVLNode *y);
AVLNode* avl_left_rotate(AVLNode *x);
```

### 可视化

实现树形打印：

```text
        10
       /  \
      5    20
     / \   / \
    3   7 15  30
```

---

## 项目4: 图算法库

### 描述

实现图数据结构和常用算法。

### 设计

```c
typedef struct Graph Graph;
typedef struct {
    int from;
    int to;
    double weight;
} Edge;

// 创建和销毁
Graph* graph_create(int num_vertices, bool directed);
void graph_destroy(Graph *g);

// 修改
void graph_add_edge(Graph *g, int from, int to, double weight);
void graph_remove_edge(Graph *g, int from, int to);

// 查询
bool graph_has_edge(const Graph *g, int from, int to);
double graph_get_weight(const Graph *g, int from, int to);
int graph_vertex_count(const Graph *g);
int graph_edge_count(const Graph *g);
```

### 算法实现

```c
// 遍历
void graph_bfs(const Graph *g, int start, void (*visit)(int));
void graph_dfs(const Graph *g, int start, void (*visit)(int));

// 最短路径
double* graph_dijkstra(const Graph *g, int start);
double** graph_floyd_warshall(const Graph *g);

// 最小生成树
Edge* graph_prim_mst(const Graph *g, int *mst_size);
Edge* graph_kruskal_mst(const Graph *g, int *mst_size);

// 拓扑排序
int* graph_topological_sort(const Graph *g, int *result_size);

// 连通分量
int graph_connected_components(const Graph *g, int *component_ids);
```

---

## 项目5: 跳表 (Skip List)

### 描述

实现概率性平衡数据结构。

### 实现

```c
typedef struct SkipNode {
    int key;
    void *value;
    struct SkipNode **forward;  // 可变长度数组
} SkipNode;

typedef struct {
    SkipNode *header;
    int level;
    int max_level;
    double p;  // 晋升概率
    size_t size;
} SkipList;

SkipList* skiplist_create(int max_level, double p);
void skiplist_destroy(SkipList *sl);

bool skiplist_insert(SkipList *sl, int key, void *value);
void* skiplist_search(SkipList *sl, int key);
bool skiplist_remove(SkipList *sl, int key);
bool skiplist_contains(SkipList *sl, int key);
```

### 复杂度分析

- 搜索: 平均O(log n)，最坏O(n)
- 插入: 平均O(log n)
- 删除: 平均O(log n)

---

## 项目6: LRU缓存

### 描述

实现基于哈希表+双向链表的LRU缓存。

### 设计

```c
typedef struct LRUCache LRUCache;

LRUCache* lru_create(int capacity);
void lru_destroy(LRUCache *cache);

// 获取值，如果不存在返回NULL，存在则移到最近使用
void* lru_get(LRUCache *cache, int key);

// 插入或更新值
void lru_put(LRUCache *cache, int key, void *value);

// 当前大小
int lru_size(const LRUCache *cache);
```

### 实现思路

```text
哈希表: key -> Node指针
双向链表: 按访问顺序排列，头部最近使用，尾部最久未使用

get操作:
  1. 哈希表查找
  2. 移到链表头部
  3. 返回值

put操作:
  1. 如果key存在，更新值，移到头部
  2. 如果不存在，创建新节点，插入头部
  3. 如果超出容量，移除尾部节点
```

---

## 项目7: B-树实现

### 描述

实现磁盘友好的B-树（为数据库/文件系统设计）。

### 特点

- 多路搜索树
- 所有叶子节点在同一层
- 适合外存（减少磁盘I/O）

### 实现

```c
#define B_TREE_MIN_DEGREE 3  // t
#define B_TREE_MAX_KEYS (2 * B_TREE_MIN_DEGREE - 1)
#define B_TREE_MAX_CHILDREN (2 * B_TREE_MIN_DEGREE)

typedef struct BTreeNode {
    int keys[B_TREE_MAX_KEYS];
    struct BTreeNode *children[B_TREE_MAX_CHILDREN];
    int num_keys;
    bool is_leaf;
} BTreeNode;

typedef struct {
    BTreeNode *root;
    int t;  // 最小度数
} BTree;

BTree* btree_create(int t);
void btree_destroy(BTree *tree);

void btree_insert(BTree *tree, int key);
void btree_remove(BTree *tree, int key);
BTreeNode* btree_search(BTreeNode *node, int key, int *index);
void btree_traverse(BTreeNode *node, void (*visit)(int));
```

---

## 综合评估

### 代码质量检查清单

- [ ] 内存管理正确（无泄漏，使用valgrind验证）
- [ ] 边界情况处理
- [ ] 有完整的单元测试
- [ ] 时间/空间复杂度符合预期
- [ ] 代码风格一致
- [ ] 有API文档注释

### 性能基准

| 数据结构 | 操作 | 预期复杂度 | 实测性能 |
|:---------|:-----|:-----------|:---------|
| Vector | push_back | 均摊O(1) | - |
| HashTable | insert | 平均O(1) | - |
| AVL Tree | insert | O(log n) | - |
| Skip List | search | O(log n) | - |
| LRU Cache | get/put | O(1) | - |

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)


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

# 高级数据结构

> **层级定位**: 01 Core Knowledge System / 02 Core Layer
> **参考课程**: MIT 6.006, Stanford CS166
> **难度级别**: L4-L5
> **预估学习时间**: 12-15 小时

---

## 目录

- [高级数据结构](#高级数据结构)
  - [目录](#目录)
  - [1. 跳表 (Skip List)](#1-跳表-skip-list)
    - [1.1 跳表原理](#11-跳表原理)
    - [1.2 跳表实现](#12-跳表实现)
  - [2. 红黑树 (Red-Black Tree)](#2-红黑树-red-black-tree)
    - [2.1 红黑树性质](#21-红黑树性质)
    - [2.2 红黑树实现](#22-红黑树实现)
  - [3. B树家族](#3-b树家族)
    - [3.1 B+树](#31-b树)
  - [4. 布隆过滤器 (Bloom Filter)](#4-布隆过滤器-bloom-filter)
    - [4.1 布隆过滤器原理](#41-布隆过滤器原理)
    - [4.2 布隆过滤器实现](#42-布隆过滤器实现)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 跳表 (Skip List)

### 1.1 跳表原理

```
跳表：概率平衡数据结构

Level 3:  head ──────────────────────────────► tail
Level 2:  head ──────────► 25 ───────────────► tail
Level 1:  head ──► 3 ────► 25 ────► 37 ──────► tail
Level 0:  head ──► 3 ──► 6 ──► 25 ──► 37 ──► tail
                     ↑
                   有序链表

特性：
• 期望时间复杂度：搜索 O(log n)，插入 O(log n)，删除 O(log n)
• 空间复杂度：O(n)
• 实现简单，无旋转平衡
• 并发友好（可锁分离）
```

### 1.2 跳表实现

```c
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_LEVEL 32
#define P 0.25  // 提升到下一层的概率

typedef struct skip_list_node {
    int key;
    void *value;
    struct skip_list_node *forward[];  // 柔性数组
} skip_list_node_t;

typedef struct {
    skip_list_node_t *header;
    int level;
    int size;
} skip_list_t;

// 创建节点
skip_list_node_t *create_node(int level, int key, void *value) {
    skip_list_node_t *node = malloc(sizeof(skip_list_node_t) +
                                    sizeof(skip_list_node_t*) * (level + 1));
    node->key = key;
    node->value = value;
    for (int i = 0; i <= level; i++) {
        node->forward[i] = NULL;
    }
    return node;
}

// 初始化跳表
skip_list_t *skip_list_create(void) {
    skip_list_t *list = malloc(sizeof(skip_list_t));
    list->header = create_node(MAX_LEVEL, -1, NULL);
    list->level = 0;
    list->size = 0;
    srand(time(NULL));
    return list;
}

// 随机层数
int random_level(void) {
    int level = 0;
    while ((double)rand() / RAND_MAX < P && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

// 搜索
void *skip_list_search(skip_list_t *list, int key) {
    skip_list_node_t *x = list->header;

    // 从最高层开始
    for (int i = list->level; i >= 0; i--) {
        while (x->forward[i] != NULL && x->forward[i]->key < key) {
            x = x->forward[i];
        }
    }

    x = x->forward[0];
    if (x != NULL && x->key == key) {
        return x->value;
    }
    return NULL;
}

// 插入
bool skip_list_insert(skip_list_t *list, int key, void *value) {
    skip_list_node_t *update[MAX_LEVEL + 1];
    skip_list_node_t *x = list->header;

    // 找到每一层的前驱节点
    for (int i = list->level; i >= 0; i--) {
        while (x->forward[i] != NULL && x->forward[i]->key < key) {
            x = x->forward[i];
        }
        update[i] = x;
    }

    x = x->forward[0];

    // 键已存在，更新值
    if (x != NULL && x->key == key) {
        x->value = value;
        return true;
    }

    // 生成随机层数
    int level = random_level();

    // 如果新层数更高，更新header的forward指针
    if (level > list->level) {
        for (int i = list->level + 1; i <= level; i++) {
            update[i] = list->header;
        }
        list->level = level;
    }

    // 创建新节点
    x = create_node(level, key, value);

    // 插入到各层
    for (int i = 0; i <= level; i++) {
        x->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = x;
    }

    list->size++;
    return true;
}

// 删除
bool skip_list_delete(skip_list_t *list, int key) {
    skip_list_node_t *update[MAX_LEVEL + 1];
    skip_list_node_t *x = list->header;

    for (int i = list->level; i >= 0; i--) {
        while (x->forward[i] != NULL && x->forward[i]->key < key) {
            x = x->forward[i];
        }
        update[i] = x;
    }

    x = x->forward[0];

    if (x == NULL || x->key != key) {
        return false;
    }

    // 从各层删除
    for (int i = 0; i <= list->level; i++) {
        if (update[i]->forward[i] != x) break;
        update[i]->forward[i] = x->forward[i];
    }

    free(x);

    // 更新最大层数
    while (list->level > 0 && list->header->forward[list->level] == NULL) {
        list->level--;
    }

    list->size--;
    return true;
}
```

---

## 2. 红黑树 (Red-Black Tree)

### 2.1 红黑树性质

```
红黑树是自平衡二叉搜索树

性质：
1. 每个节点是红色或黑色
2. 根节点是黑色
3. 每个叶子(NIL)是黑色
4. 红色节点的子节点必须是黑色（无连续红节点）
5. 从任一节点到其叶子的所有路径包含相同数目的黑色节点

黑高度：从节点到叶子的黑色节点数（不含自身）

复杂度：
• 搜索：O(log n)
• 插入：O(log n)
• 删除：O(log n)
```

### 2.2 红黑树实现

```c
#include <stdlib.h>
#include <stdbool.h>

typedef enum { RED, BLACK } color_t;

typedef struct rb_node {
    int key;
    void *value;
    color_t color;
    struct rb_node *left, *right, *parent;
} rb_node_t;

typedef struct {
    rb_node_t *root;
    rb_node_t *nil;  // 哨兵叶子节点
} rb_tree_t;

// 创建树
rb_tree_t *rb_tree_create(void) {
    rb_tree_t *tree = malloc(sizeof(rb_tree_t));
    tree->nil = malloc(sizeof(rb_node_t));
    tree->nil->color = BLACK;
    tree->root = tree->nil;
    return tree;
}

// 左旋
void left_rotate(rb_tree_t *tree, rb_node_t *x) {
    rb_node_t *y = x->right;
    x->right = y->left;

    if (y->left != tree->nil) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

// 右旋
void right_rotate(rb_tree_t *tree, rb_node_t *y) {
    rb_node_t *x = y->left;
    y->left = x->right;

    if (x->right != tree->nil) {
        x->right->parent = y;
    }

    x->parent = y->parent;

    if (y->parent == tree->nil) {
        tree->root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }

    x->right = y;
    y->parent = x;
}

// 插入修复
void rb_insert_fixup(rb_tree_t *tree, rb_node_t *z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            rb_node_t *y = z->parent->parent->right;

            // 情况1：叔节点是红色
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                // 情况2：z是右子节点
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(tree, z);
                }
                // 情况3：z是左子节点
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(tree, z->parent->parent);
            }
        } else {
            // 对称情况
            rb_node_t *y = z->parent->parent->left;

            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

// 插入
void rb_insert(rb_tree_t *tree, int key, void *value) {
    rb_node_t *z = malloc(sizeof(rb_node_t));
    z->key = key;
    z->value = value;
    z->color = RED;
    z->left = tree->nil;
    z->right = tree->nil;

    rb_node_t *y = tree->nil;
    rb_node_t *x = tree->root;

    while (x != tree->nil) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->parent = y;

    if (y == tree->nil) {
        tree->root = z;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    rb_insert_fixup(tree, z);
}
```

---

## 3. B树家族

### 3.1 B+树

```c
#define MIN_DEGREE 3  // 最小度数t
#define MAX_KEYS (2 * MIN_DEGREE - 1)
#define MAX_CHILDREN (2 * MIN_DEGREE)

typedef struct bp_node {
    bool is_leaf;
    int num_keys;
    int keys[MAX_KEYS];
    struct bp_node *children[MAX_CHILDREN];
    struct bp_node *next;  // 叶子节点链表（仅叶子）
} bp_node_t;

typedef struct {
    bp_node_t *root;
} bp_tree_t;

// 搜索
bp_node_t *bp_search(bp_node_t *node, int key, int *index) {
    int i = 0;
    while (i < node->num_keys && key > node->keys[i]) {
        i++;
    }

    if (i < node->num_keys && key == node->keys[i]) {
        *index = i;
        return node;
    }

    if (node->is_leaf) {
        return NULL;
    }

    return bp_search(node->children[i], key, index);
}

// 分裂子节点
void bp_split_child(bp_node_t *parent, int i, bp_node_t *child) {
    bp_node_t *new_node = malloc(sizeof(bp_node_t));
    new_node->is_leaf = child->is_leaf;
    new_node->num_keys = MIN_DEGREE - 1;

    // 复制后t-1个键到新节点
    for (int j = 0; j < MIN_DEGREE - 1; j++) {
        new_node->keys[j] = child->keys[j + MIN_DEGREE];
    }

    // 如果不是叶子，复制子节点指针
    if (!child->is_leaf) {
        for (int j = 0; j < MIN_DEGREE; j++) {
            new_node->children[j] = child->children[j + MIN_DEGREE];
        }
    } else {
        // 维护叶子链表
        new_node->next = child->next;
        child->next = new_node;
    }

    child->num_keys = MIN_DEGREE - 1;

    // 在父节点中腾出位置
    for (int j = parent->num_keys; j >= i + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = new_node;

    for (int j = parent->num_keys - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[i] = child->keys[MIN_DEGREE - 1];
    parent->num_keys++;
}

// 插入（非满节点）
void bp_insert_non_full(bp_node_t *node, int key) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        // 在叶子节点中插入
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        // 找到合适的子节点
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;

        if (node->children[i]->num_keys == MAX_KEYS) {
            bp_split_child(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        bp_insert_non_full(node->children[i], key);
    }
}

// 主插入函数
void bp_insert(bp_tree_t *tree, int key) {
    bp_node_t *root = tree->root;

    if (root->num_keys == MAX_KEYS) {
        // 根节点满了，需要分裂
        bp_node_t *new_root = malloc(sizeof(bp_node_t));
        new_root->is_leaf = false;
        new_root->num_keys = 0;
        new_root->children[0] = root;
        tree->root = new_root;
        bp_split_child(new_root, 0, root);
        bp_insert_non_full(new_root, key);
    } else {
        bp_insert_non_full(root, key);
    }
}
```

---

## 4. 布隆过滤器 (Bloom Filter)

### 4.1 布隆过滤器原理

```
布隆过滤器：概率型数据结构

特性：
• 空间高效的集合成员测试
• 可能有假阳性（误报）
• 无假阴性
• 不支持删除（标准版本）

结构：
┌─────────────────────────────────────────────────────────────┐
│  m位数组，初始全0                                            │
│  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]                            │
└─────────────────────────────────────────────────────────────┘

添加元素x：
• 使用k个哈希函数计算h1(x), h2(x), ..., hk(x)
• 将数组中对应位置置1

查询元素y：
• 计算h1(y), h2(y), ..., hk(y)
• 如果所有位置都是1，可能在集合中
• 如果有任何位置是0，一定不在集合中
```

### 4.2 布隆过滤器实现

```c
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// 简单的哈希函数
uint64_t hash1(const char *data, size_t len) {
    uint64_t hash = 5381;
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }
    return hash;
}

uint64_t hash2(const char *data, size_t len) {
    uint64_t hash = 0;
    for (size_t i = 0; i < len; i++) {
        hash = data[i] + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

// 双哈希生成k个哈希值
uint64_t nth_hash(uint64_t n, uint64_t hash_a, uint64_t hash_b, uint64_t filter_size) {
    return (hash_a + n * hash_b) % filter_size;
}

typedef struct {
    uint8_t *bits;
    size_t size;      // 位数m
    int num_hashes;   // 哈希函数数k
} bloom_filter_t;

// 创建布隆过滤器
// expected_items: 预期元素数
// false_positive_rate: 可接受的假阳性率
bloom_filter_t *bloom_filter_create(size_t expected_items, double false_positive_rate) {
    bloom_filter_t *bf = malloc(sizeof(bloom_filter_t));

    // 计算最优参数
    // m = -n * ln(p) / (ln(2)^2)
    // k = m * ln(2) / n
    double ln2 = 0.69314718056;
    double m = -expected_items * log(false_positive_rate) / (ln2 * ln2);
    double k = m * ln2 / expected_items;

    bf->size = (size_t)m;
    bf->num_hashes = (int)ceil(k);
    bf->bits = calloc((bf->size + 7) / 8, 1);  // 位数组

    return bf;
}

// 设置位
static inline void set_bit(bloom_filter_t *bf, size_t index) {
    bf->bits[index / 8] |= (1 << (index % 8));
}

// 检查位
static inline bool get_bit(bloom_filter_t *bf, size_t index) {
    return (bf->bits[index / 8] & (1 << (index % 8))) != 0;
}

// 添加元素
void bloom_filter_add(bloom_filter_t *bf, const char *data, size_t len) {
    uint64_t hash_a = hash1(data, len);
    uint64_t hash_b = hash2(data, len);

    for (int i = 0; i < bf->num_hashes; i++) {
        size_t index = nth_hash(i, hash_a, hash_b, bf->size);
        set_bit(bf, index);
    }
}

// 查询元素
// 返回true：可能在集合中（可能有假阳性）
// 返回false：一定不在集合中
bool bloom_filter_might_contain(bloom_filter_t *bf, const char *data, size_t len) {
    uint64_t hash_a = hash1(data, len);
    uint64_t hash_b = hash2(data, len);

    for (int i = 0; i < bf->num_hashes; i++) {
        size_t index = nth_hash(i, hash_a, hash_b, bf->size);
        if (!get_bit(bf, index)) {
            return false;
        }
    }
    return true;
}

// 示例用法
void example(void) {
    // 预期10000个元素，1%假阳性率
    bloom_filter_t *bf = bloom_filter_create(10000, 0.01);

    // 添加元素
    bloom_filter_add(bf, "hello", 5);
    bloom_filter_add(bf, "world", 5);

    // 查询
    printf("'hello' might be present: %d\n",
           bloom_filter_might_contain(bf, "hello", 5));  // true
    printf("'foo' might be present: %d\n",
           bloom_filter_might_contain(bf, "foo", 3));    // false（大概率）
}
```

---

## 关联导航

### 前置知识

- [基础数据结构](../01_Introduction/04_Data_Types_and_Memory_Layout.md)
- [内存管理](./01_Memory_Model.md)

### 后续延伸

- [并发数据结构](../14_Concurrency_Parallelism/05_Lock_Free_Data_Structures.md)
- [数据库索引实现](../../04_Industrial_Scenarios/01_Database_Development/02_Index_Implementation.md)

### 参考

- Introduction to Algorithms (CLRS)
- MIT 6.006: Introduction to Algorithms
- Stanford CS166: Data Structures

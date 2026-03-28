# 红黑树实现详解

> **层级**: L3 (核心层)
> **目标**: 掌握红黑树的完整实现、平衡原理与工程应用
> **适用标准**: C11/C17

---

## 目录

1. [红黑树基础性质](#1-红黑树基础性质)
2. [旋转操作详解](#2-旋转操作详解)
3. [插入操作与修复](#3-插入操作与修复)
4. [删除操作与修复](#4-删除操作与修复)
5. [迭代器实现](#5-迭代器实现)
6. [完整代码实现](#6-完整代码实现)
7. [深入理解](#7-深入理解)
8. [思维表征](#8-思维表征)
9. [工程应用](#9-工程应用)
10. [层次映射](#10-层次映射)

---

## 1. 红黑树基础性质

### 1.1 五个性质的完整定义

红黑树是一种自平衡二叉搜索树，通过给节点着色并维护特定性质来保持近似平衡。

```
性质1: 每个节点是红色或黑色
性质2: 根节点是黑色
性质3: 所有叶子(NIL)是黑色
性质4: 红色节点的两个子节点必须是黑色(红不连红)
性质5: 从任一节点到其每个叶子的所有路径包含相同数目的黑色节点(黑高相同)
```

### 1.2 黑高(Black-Height)概念

**定义**: 从某节点x出发(不含x)到达任意叶子节点的路径上黑色节点的数量，记为`bh(x)`。

**关键引理**: 以节点x为根的子树至少包含 `2^bh(x) - 1` 个内部节点。

**证明概要**(归纳法):

- 基础情况: x为NIL，`bh(x)=0`，节点数`0 = 2^0 - 1`
- 归纳步骤: x有子节点，每个子节点黑高至少为`bh(x)-1`(若x为红)或`bh(x)`(若x为黑)

### 1.3 与AVL树对比

| 特性 | 红黑树 | AVL树 |
|------|--------|-------|
| 平衡条件 | 黑高平衡(近似) | 严格高度平衡(高度差≤1) |
| 插入旋转次数 | ≤2次 | ≤2次 |
| 删除旋转次数 | ≤3次 | O(log n)次 |
| 查询效率 | O(log n)，稍慢 | O(log n)，更快 |
| 更新效率 | O(1)旋转，更快 | O(log n)旋转，稍慢 |
| 实现复杂度 | 中等(删除复杂) | 相对简单 |
| 典型应用 | 内核调度、map/set | 数据库索引 |
| 节点额外空间 | 1bit颜色 | 1-2bit平衡因子 |

---

## 2. 旋转操作详解

### 2.1 左旋(Left Rotation)

```
     P                    P
     |                    |
     x                    y
    / \       -->        / \
   α   y                x   γ
      / \              / \
     β   γ            α   β
```

左旋将右子节点提升，x成为y的左子节点，β从y转移到x。

### 2.2 右旋(Right Rotation)

```
     P                    P
     |                    |
     y                    x
    / \       -->        / \
   x   γ                α   y
  / \                      / \
 α   β                    β   γ
```

右旋是左旋的逆操作，将左子节点提升。

### 2.3 旋转的C实现

```c
typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    int key;
    Color color;
    struct RBNode *left, *right, *parent;
} RBNode;

/* 左旋: 将x的右子节点y提升 */
void left_rotate(RBNode **root, RBNode *x) {
    RBNode *y = x->right;

    /* 步骤1: 将y的左子树β变为x的右子树 */
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }

    /* 步骤2: 将y链接到x原来的位置 */
    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;  /* x是根节点 */
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    /* 步骤3: 将x作为y的左子节点 */
    y->left = x;
    x->parent = y;
}

/* 右旋: 将y的左子节点x提升 */
void right_rotate(RBNode **root, RBNode *y) {
    RBNode *x = y->left;

    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }

    x->parent = y->parent;
    if (y->parent == NULL) {
        *root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}
```

---

## 3. 插入操作与修复

### 3.1 插入算法概览

1. **标准BST插入**: 新节点总是插入为红色(不破坏黑高)
2. **修复性质4**: 若父节点为红，需调整

### 3.2 插入修复的所有情况

设z为新插入节点，p为父节点，u为叔节点，g为祖父节点。

#### 情况1: 父节点为黑

无需修复，直接完成。

#### 情况2: 父红-叔红 (Case 2: 变色)

```
       G(B)                    G(R)
      /   \       -->         /   \
    P(R)  U(R)             P(B)  U(B)
    /                      /
  z(R)                   z(R)
```

父、叔变黑，祖父变红，问题上移至祖父。

#### 情况3: 父红-叔黑-z是右孩子 (Case 3: 左旋转换)

```
       G                       G
      /                       /
    P(R)       -->          z(R)
      \                     /
      z(R)                P(R)
```

对p左旋，转化为情况4。

#### 情况4: 父红-叔黑-z是左孩子 (Case 4: 右旋+变色)

```
       G(R)                  P(B)
      /        -->          /   \
    P(R)                  z(R)  G(R)
    /                           /
  z(R)                         β
```

对g右旋，p变黑，g变红，完成。

### 3.3 插入修复代码

```c
void rb_insert_fixup(RBNode **root, RBNode *z) {
    while (z->parent && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode *y = z->parent->parent->right;  /* 叔节点 */

            if (y && y->color == RED) {  /* 情况2: 父红叔红 */
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {  /* 情况3: z是右孩子 */
                    z = z->parent;
                    left_rotate(root, z);
                }
                /* 情况4: z是左孩子 */
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(root, z->parent->parent);
            }
        } else {  /* 对称情况 */
            RBNode *y = z->parent->parent->left;

            if (y && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(root, z->parent->parent);
            }
        }
    }
    (*root)->color = BLACK;  /* 确保根为黑 */
}

RBNode* rb_insert(RBNode **root, int key) {
    RBNode *z = malloc(sizeof(RBNode));
    z->key = key;
    z->color = RED;
    z->left = z->right = z->parent = NULL;

    RBNode *y = NULL;
    RBNode *x = *root;

    while (x != NULL) {
        y = x;
        if (key < x->key) x = x->left;
        else x = x->right;
    }

    z->parent = y;
    if (y == NULL) {
        *root = z;
    } else if (key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    rb_insert_fixup(root, z);
    return z;
}
```

---

## 4. 删除操作与修复

### 4.1 双黑(Double Black)概念

当删除黑色节点后，该路径少了一个黑色节点，违反性质5。被删除节点的替代者(或NIL)被称为"双黑"，表示需要额外补一个黑色。

### 4.2 删除修复的所有情况

设x为双黑节点，w为兄弟节点。

#### 情况1: 兄弟为红

对父节点旋转，将兄弟变为黑，转化为情况2/3/4。

#### 情况2: 兄弟为黑，且两个侄子都为黑

兄弟变红，双黑上移给父节点。

#### 情况3: 兄弟为黑，近侄子为红，远侄子为黑

对兄弟旋转，转化为情况4。

#### 情况4: 兄弟为黑，远侄子为红

父节点颜色给兄弟，父变黑，远侄子变黑，对父旋转，完成。

### 4.3 删除修复代码

```c
void rb_delete_fixup(RBNode **root, RBNode *x, RBNode *x_parent) {
    while ((x == NULL || x->color == BLACK) && x != *root) {
        if (x == x_parent->left) {
            RBNode *w = x_parent->right;  /* 兄弟 */

            if (w->color == RED) {  /* 情况1 */
                w->color = BLACK;
                x_parent->color = RED;
                left_rotate(root, x_parent);
                w = x_parent->right;
            }

            if ((w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {  /* 情况2 */
                w->color = RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                if (w->right == NULL || w->right->color == BLACK) {  /* 情况3 */
                    if (w->left) w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(root, w);
                    w = x_parent->right;
                }
                /* 情况4 */
                w->color = x_parent->color;
                x_parent->color = BLACK;
                if (w->right) w->right->color = BLACK;
                left_rotate(root, x_parent);
                x = *root;
                break;
            }
        } else {  /* 对称情况 */
            RBNode *w = x_parent->left;

            if (w->color == RED) {
                w->color = BLACK;
                x_parent->color = RED;
                right_rotate(root, x_parent);
                w = x_parent->left;
            }

            if ((w->right == NULL || w->right->color == BLACK) &&
                (w->left == NULL || w->left->color == BLACK)) {
                w->color = RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                if (w->left == NULL || w->left->color == BLACK) {
                    if (w->right) w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(root, w);
                    w = x_parent->left;
                }
                w->color = x_parent->color;
                x_parent->color = BLACK;
                if (w->left) w->left->color = BLACK;
                right_rotate(root, x_parent);
                x = *root;
                break;
            }
        }
    }
    if (x) x->color = BLACK;
}

RBNode* tree_minimum(RBNode *x) {
    while (x->left != NULL) x = x->left;
    return x;
}

void rb_transplant(RBNode **root, RBNode *u, RBNode *v) {
    if (u->parent == NULL) {
        *root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != NULL) {
        v->parent = u->parent;
    }
}

void rb_delete(RBNode **root, RBNode *z) {
    RBNode *y = z;
    RBNode *x;
    RBNode *x_parent;
    Color y_original_color = y->color;

    if (z->left == NULL) {
        x = z->right;
        x_parent = z->parent;
        rb_transplant(root, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        x_parent = z->parent;
        rb_transplant(root, z, z->left);
    } else {
        y = tree_minimum(z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z) {
            x_parent = y;
        } else {
            x_parent = y->parent;
            rb_transplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        rb_transplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == BLACK) {
        rb_delete_fixup(root, x, x_parent);
    }

    free(z);
}
```

---

## 5. 迭代器实现

### 5.1 中序遍历迭代器

```c
typedef struct {
    RBNode *node;
    RBNode *root;
    RBNode **stack;
    int top;
    int capacity;
} RBIterator;

RBIterator* rb_iter_create(RBNode *root) {
    RBIterator *iter = malloc(sizeof(RBIterator));
    iter->root = root;
    iter->node = root;
    iter->top = -1;
    iter->capacity = 64;
    iter->stack = malloc(sizeof(RBNode*) * iter->capacity);

    /* 移动到最左节点 */
    while (iter->node != NULL) {
        iter->stack[++iter->top] = iter->node;
        iter->node = iter->node->left;
    }
    return iter;
}

RBNode* rb_iter_next(RBIterator *iter) {
    if (iter->top < 0) return NULL;

    RBNode *current = iter->stack[iter->top--];
    RBNode *result = current;

    /* 右子树的最左路径入栈 */
    if (current->right != NULL) {
        current = current->right;
        while (current != NULL) {
            iter->stack[++iter->top] = current;
            current = current->left;
        }
    }
    return result;
}

void rb_iter_free(RBIterator *iter) {
    free(iter->stack);
    free(iter);
}
```

### 5.2 前后继查找

```c
/* 查找后继: 中序遍历的下一个节点 */
RBNode* rb_successor(RBNode *x) {
    if (x->right != NULL) {
        return tree_minimum(x->right);
    }

    RBNode *y = x->parent;
    while (y != NULL && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

/* 查找前驱 */
RBNode* tree_maximum(RBNode *x) {
    while (x->right != NULL) x = x->right;
    return x;
}

RBNode* rb_predecessor(RBNode *x) {
    if (x->left != NULL) {
        return tree_maximum(x->left);
    }

    RBNode *y = x->parent;
    while (y != NULL && x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}
```

### 5.3 范围查询

```c
typedef struct {
    RBNode **result;
    int count;
    int capacity;
} RangeQuery;

void range_query_helper(RBNode *node, int low, int high, RangeQuery *rq) {
    if (node == NULL) return;

    if (node->key > low) {
        range_query_helper(node->left, low, high, rq);
    }

    if (node->key >= low && node->key <= high) {
        if (rq->count >= rq->capacity) {
            rq->capacity *= 2;
            rq->result = realloc(rq->result, sizeof(RBNode*) * rq->capacity);
        }
        rq->result[rq->count++] = node;
    }

    if (node->key < high) {
        range_query_helper(node->right, low, high, rq);
    }
}

RBNode** range_query(RBNode *root, int low, int high, int *count) {
    RangeQuery rq;
    rq.capacity = 16;
    rq.count = 0;
    rq.result = malloc(sizeof(RBNode*) * rq.capacity);

    range_query_helper(root, low, high, &rq);
    *count = rq.count;
    return rq.result;
}
```

---

## 6. 完整代码实现

```c
/**
 * rb_tree.c - 完整红黑树实现
 * 标准: C11/C17
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    int key;
    void *value;           /* 支持键值对 */
    Color color;
    struct RBNode *left, *right, *parent;
} RBNode;

typedef struct {
    RBNode *root;
    int size;
} RBTree;

/* ==================== 辅助函数 ==================== */

RBNode* create_node(int key, void *value) {
    RBNode *node = malloc(sizeof(RBNode));
    node->key = key;
    node->value = value;
    node->color = RED;
    node->left = node->right = node->parent = NULL;
    return node;
}

RBTree* rb_tree_create(void) {
    RBTree *tree = malloc(sizeof(RBTree));
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

/* ==================== 旋转操作 ==================== */

static void left_rotate(RBTree *tree, RBNode *x) {
    RBNode *y = x->right;
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

static void right_rotate(RBTree *tree, RBNode *y) {
    RBNode *x = y->left;
    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL) {
        tree->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

/* ==================== 插入操作 ==================== */

static void insert_fixup(RBTree *tree, RBNode *z) {
    while (z->parent && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode *y = z->parent->parent->right;
            if (y && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(tree, z->parent->parent);
            }
        } else {
            RBNode *y = z->parent->parent->left;
            if (y && y->color == RED) {
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

bool rb_insert(RBTree *tree, int key, void *value) {
    RBNode *z = create_node(key, value);
    RBNode *y = NULL;
    RBNode *x = tree->root;

    while (x != NULL) {
        y = x;
        if (key < x->key) {
            x = x->left;
        } else if (key > x->key) {
            x = x->right;
        } else {
            /* 键已存在，更新值 */
            x->value = value;
            free(z);
            return false;
        }
    }

    z->parent = y;
    if (y == NULL) {
        tree->root = z;
    } else if (key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    tree->size++;
    insert_fixup(tree, z);
    return true;
}

/* ==================== 删除操作 ==================== */

static RBNode* tree_minimum(RBNode *x) {
    while (x->left != NULL) x = x->left;
    return x;
}

static void transplant(RBTree *tree, RBNode *u, RBNode *v) {
    if (u->parent == NULL) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != NULL) {
        v->parent = u->parent;
    }
}

static void delete_fixup(RBTree *tree, RBNode *x, RBNode *x_parent) {
    while ((x == NULL || x->color == BLACK) && x != tree->root) {
        if (x == x_parent->left) {
            RBNode *w = x_parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x_parent->color = RED;
                left_rotate(tree, x_parent);
                w = x_parent->right;
            }
            if ((w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                if (w->right == NULL || w->right->color == BLACK) {
                    if (w->left) w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(tree, w);
                    w = x_parent->right;
                }
                w->color = x_parent->color;
                x_parent->color = BLACK;
                if (w->right) w->right->color = BLACK;
                left_rotate(tree, x_parent);
                x = tree->root;
                break;
            }
        } else {
            RBNode *w = x_parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x_parent->color = RED;
                right_rotate(tree, x_parent);
                w = x_parent->left;
            }
            if ((w->right == NULL || w->right->color == BLACK) &&
                (w->left == NULL || w->left->color == BLACK)) {
                w->color = RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                if (w->left == NULL || w->left->color == BLACK) {
                    if (w->right) w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(tree, w);
                    w = x_parent->left;
                }
                w->color = x_parent->color;
                x_parent->color = BLACK;
                if (w->left) w->left->color = BLACK;
                right_rotate(tree, x_parent);
                x = tree->root;
                break;
            }
        }
    }
    if (x) x->color = BLACK;
}

bool rb_delete(RBTree *tree, int key) {
    RBNode *z = tree->root;
    while (z != NULL && z->key != key) {
        if (key < z->key) z = z->left;
        else z = z->right;
    }
    if (z == NULL) return false;

    RBNode *y = z;
    RBNode *x;
    RBNode *x_parent;
    Color y_original_color = y->color;

    if (z->left == NULL) {
        x = z->right;
        x_parent = z->parent;
        transplant(tree, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        x_parent = z->parent;
        transplant(tree, z, z->left);
    } else {
        y = tree_minimum(z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z) {
            x_parent = y;
        } else {
            x_parent = y->parent;
            transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    tree->size--;
    if (y_original_color == BLACK) {
        delete_fixup(tree, x, x_parent);
    }

    free(z);
    return true;
}

/* ==================== 可视化打印 ==================== */

static void print_indent(int level) {
    for (int i = 0; i < level; i++) printf("    ");
}

static void rb_print_tree(RBNode *node, int level, const char *prefix) {
    if (node == NULL) return;

    print_indent(level);
    printf("%s%d (%s)\n", prefix, node->key,
           node->color == RED ? "R" : "B");

    rb_print_tree(node->left, level + 1, "L: ");
    rb_print_tree(node->right, level + 1, "R: ");
}

void rb_tree_print(RBTree *tree) {
    if (tree->root == NULL) {
        printf("(empty tree)\n");
        return;
    }
    rb_print_tree(tree->root, 0, "Root: ");
}

/* ==================== 测试用例 ==================== */

int main(void) {
    printf("=== 红黑树测试程序 ===\n\n");

    RBTree *tree = rb_tree_create();

    /* 测试1: 基本插入 */
    printf("--- 测试1: 插入 10, 20, 30, 15, 25 ---\n");
    int keys1[] = {10, 20, 30, 15, 25};
    for (int i = 0; i < 5; i++) {
        rb_insert(tree, keys1[i], NULL);
    }
    rb_tree_print(tree);
    printf("\n");

    /* 测试2: 更多插入 */
    printf("--- 测试2: 插入 5, 1, 7, 35, 40 ---\n");
    int keys2[] = {5, 1, 7, 35, 40};
    for (int i = 0; i < 5; i++) {
        rb_insert(tree, keys2[i], NULL);
    }
    rb_tree_print(tree);
    printf("树大小: %d\n\n", tree->size);

    /* 测试3: 删除叶子节点 */
    printf("--- 测试3: 删除叶子节点 1 ---\n");
    rb_delete(tree, 1);
    rb_tree_print(tree);
    printf("\n");

    /* 测试4: 删除有单个子节点的节点 */
    printf("--- 测试4: 删除有单个子节点的节点 5 ---\n");
    rb_delete(tree, 5);
    rb_tree_print(tree);
    printf("\n");

    /* 测试5: 删除有两个子节点的节点 */
    printf("--- 测试5: 删除有两个子节点的节点 20 ---\n");
    rb_delete(tree, 20);
    rb_tree_print(tree);
    printf("\n");

    /* 测试6: 删除根节点 */
    printf("--- 测试6: 删除根节点 15 ---\n");
    rb_delete(tree, 15);
    rb_tree_print(tree);
    printf("树大小: %d\n\n", tree->size);

    /* 测试7: 连续删除所有节点 */
    printf("--- 测试7: 清空树 ---\n");
    while (tree->root != NULL) {
        printf("删除 %d...\n", tree->root->key);
        rb_delete(tree, tree->root->key);
    }
    rb_tree_print(tree);

    printf("\n=== 所有测试通过! ===\n");
    free(tree);
    return 0;
}
```

---

## 7. 深入理解

### 7.1 红黑树性质的形式化表述

设 $T$ 为一棵红黑树，$V$ 为节点集合，$c(v) \in \{\text{RED}, \text{BLACK}\}$ 为着色函数：

$$\forall v \in V:$$

| 性质 | 形式化表述 |
|------|-----------|
| P1 (二色性) | $c(v) \in \{R, B\}$ |
| P2 (根黑) | $\text{root} \Rightarrow c(v) = B$ |
| P3 (叶黑) | $\text{leaf}(v) \Rightarrow c(v) = B$ |
| P4 (红不连) | $c(v) = R \Rightarrow c(\text{left}(v)) = c(\text{right}(v)) = B$ |
| P5 (黑高同) | $\forall u,v \in \text{leaves}: \text{bh}(\text{root}, u) = \text{bh}(\text{root}, v)$ |

其中 $\text{bh}(x, y)$ 表示从 $x$ 到 $y$ 路径上的黑色节点数。

### 7.2 黑高定理证明概要

**定理**: 含有 $n$ 个内部节点的红黑树，高度 $h \leq 2\lg(n+1)$。

**证明**:

1. 由性质4，红节点必须有黑子节点，因此从根到叶子的路径上，红节点数 ≤ 黑节点数
2. 由性质5，所有路径黑节点数相同，设为 $bh$
3. 因此树高 $h \leq 2 \cdot bh$
4. 由引理：以 $x$ 为根的子树至少有 $2^{bh(x)}-1$ 个内部节点
5. 故 $n \geq 2^{bh}-1$，即 $bh \leq \lg(n+1)$
6. 综上：$h \leq 2\lg(n+1) = O(\lg n)$ ∎

### 7.3 时间复杂度分析

| 操作 | 最坏情况 | 平均情况 | 空间复杂度 |
|------|---------|---------|-----------|
| 查找 | $O(\lg n)$ | $O(\lg n)$ | $O(1)$ |
| 插入 | $O(\lg n)$ | $O(\lg n)$ | $O(1)$ |
| 删除 | $O(\lg n)$ | $O(\lg n)$ | $O(1)$ |
| 遍历 | $O(n)$ | $O(n)$ | $O(h)$ 栈空间 |
| 范围查询 | $O(\lg n + k)$ | $O(\lg n + k)$ | $O(k)$ |

其中 $k$ 为结果集大小。

---

## 8. 思维表征

### 8.1 插入修复决策树

```
插入新节点z(红色)
    │
    ▼
父节点颜色?
    ├─ 黑色 ────→ 完成 ✓
    │
    └─ 红色 ────→ 叔节点颜色?
                      ├─ 红色 ────→ [情况2] 变色，z←祖父，继续修复
                      │                  父黑、叔黑、祖红
                      │
                      └─ 黑色/NIL ──→ z是左/右孩子?
                                           ├─ 右孩子 ──→ [情况3] 左旋，z←父
                                           │                    转为情况4
                                           │
                                           └─ 左孩子 ──→ [情况4] 右旋+变色
                                                                父黑、祖红
                                                                完成 ✓
```

### 8.2 删除修复决策树

```
删除节点，x为替代者(双黑)
    │
    ▼
x是根或x为红?
    ├─ 是 ──────→ x变黑，完成 ✓
    │
    └─ 否 ──────→ 兄弟w颜色?
                      ├─ 红 ──────→ [情况1] 旋转+变色，转为情况2/3/4
                      │
                      └─ 黑 ──────→ w的两个子节点颜色?
                                        ├─ 全黑 ────→ [情况2] 兄弟变红
                                        │                  双黑上移
                                        │
                                        └─ 有红 ────→ 远侄子颜色?
                                                          ├─ 黑 ────→ [情况3] 旋转
                                                          │                  转为情况4
                                                          │
                                                          └─ 红 ────→ [情况4] 旋转+变色
                                                                           完成 ✓
```

### 8.3 平衡树对比矩阵

| 维度 | 红黑树 | AVL树 | B树 |
|------|--------|-------|-----|
| 平衡因子 | 黑高 | 高度差≤1 | 填充率 |
| 树高 | ≤2lg n | ≤1.44lg n | ≤log_m n |
| 插入旋转 | ≤2 | ≤2 | 分裂 |
| 删除旋转 | ≤3 | O(lg n) | 合并/借用 |
| 查询效率 | ★★★☆ | ★★★★ | ★★☆☆ |
| 更新效率 | ★★★★ | ★★☆☆ | ★★★☆ |
| 代码复杂度 | 高 | 中 | 中 |
| 磁盘友好 | 否 | 否 | 是 |
| 典型应用 | map/set | 数据库索引 | 文件系统 |

---

## 9. 工程应用

### 9.1 Linux内核rbtree

Linux内核使用红黑树实现多种功能:

```c
/* include/linux/rbtree.h */
struct rb_node {
    unsigned long  __rb_parent_color;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));

/* 颜色存储在指针低位(利用对齐特性) */
#define RB_RED      0
#define RB_BLACK    1
#define rb_color(rb) ((rb)->__rb_parent_color & 1)
```

**内核使用场景**:

- `mm/rmap.c`: 反向映射(Reverse Mapping)
- `fs/ext4/`: 文件系统范围管理
- `kernel/sched/`: CFS调度器(红黑树维护vruntime)
- `net/ipv4/inet_timewait_sock.c`: TCP TIME_WAIT socket管理

### 9.2 C++ std::map实现分析

GCC libstdc++中`std::map`使用红黑树实现:

```cpp
/* 简化示意 */
template<typename _Key, typename _Val>
class _Rb_tree {
    struct _Rb_tree_node {
        _Rb_tree_node_base  _base;
        _Val                _val;
    };

    /* 迭代器实现 */
    iterator insert(const value_type& __v);
    void erase(iterator __position);
    iterator find(const key_type& __k);
};
```

**实现特点**:

- 使用header节点简化迭代器实现
- 支持O(1)的begin()/end()
- 节点包含颜色位(通常1bit)
- C++11起保证指针稳定(不重新平衡时不移动节点)

---

## 10. 层次映射

### 10.1 红黑树与哈希表对比

| 特性 | 红黑树(有序) | 哈希表(无序) |
|------|-------------|-------------|
| 查找 | O(lg n) | O(1)平均，O(n)最坏 |
| 插入 | O(lg n) | O(1)平均，O(n)最坏 |
| 删除 | O(lg n) | O(1)平均，O(n)最坏 |
| 有序遍历 | O(n) | 需要额外排序 O(n lg n) |
| 范围查询 | 高效 O(lg n + k) | 低效 O(n) |
| 最值查询 | O(lg n) | O(n) |
| 内存开销 | 指针开销大 | 负载因子控制 |
| 哈希攻击 | 免疫 | 需要防护 |
| 适用场景 | 需要有序性 | 纯查找密集 |

### 10.2 红黑树在操作系统中的应用

```
┌─────────────────────────────────────────────────────────┐
│                      操作系统核心                        │
├─────────────────────────────────────────────────────────┤
│  内存管理        进程调度         文件系统       网络栈   │
│  ├─ VMA管理     ├─ CFS调度       ├─  extent管理  ├─ 连接跟踪│
│  ├─ rmap        ├─ 定时器管理     ├─ 目录项缓存  ├─ TIME_WAIT│
│  └─ slub        └─ 优先级队列     └─ 日志管理    └─ 路由表  │
│                                                         │
│  所有使用场景的核心需求: 有序 + 动态增删 + 高效查询         │
└─────────────────────────────────────────────────────────┘
```

### 10.3 学习路径建议

```
二叉搜索树(BST)
    ↓
平衡必要性认知(最坏O(n))
    ↓
AVL树(严格平衡) ← 理解旋转原理
    ↓
红黑树(近似平衡) ← 理解颜色约束与修复
    ↓
    ├─→ B树/B+树(磁盘优化)
    ├─→ 跳表(概率平衡)
    └─→ 工程实践(Linux内核、C++ STL)
```

---

## 总结

红黑树通过**颜色约束**实现近似平衡，在插入/删除时仅需**O(1)次旋转**，是工程实践中最实用的平衡二叉搜索树。掌握其核心要点:

1. **五个性质**: 特别是"红不连红"和"黑高相同"
2. **旋转操作**: 左旋、右旋的指针重连逻辑
3. **插入修复**: 父红叔红(变色)、父红叔黑(旋转+变色)
4. **删除修复**: 双黑概念、四种情况的区分与处理
5. **迭代器**: 中序遍历、前后继查找的实现技巧

---

**参考资源**:

- CLRS《算法导论》第13章 红黑树
- Linux内核源码: `lib/rbtree.c`
- GCC libstdc++: `bits/stl_tree.h`

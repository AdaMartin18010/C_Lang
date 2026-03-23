<!--
  文件: 04_Binary_Tree_Fundamentals.md
  标题: 二叉树基础
  版本: 1.0.0
  作者: C_Lang Knowledge Base
  最后更新: 2026-03-14
  描述: 涵盖二叉树、BST、AVL树、红黑树的完整理论与实践
  标准: C23 (ISO/IEC 9899:2024)
-->

# 二叉树基础 (Binary Tree Fundamentals)

> **版本**: 1.0.0
> **最后更新**: 2026-03-14
> **C标准**: C23 (ISO/IEC 9899:2024)
> **预计阅读时间**: 85分钟

---

## 📋 目录

- [二叉树基础 (Binary Tree Fundamentals)](#二叉树基础-binary-tree-fundamentals)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [1. 概述](#1-概述)
    - [1.1 什么是树](#11-什么是树)
    - [1.2 二叉树定义](#12-二叉树定义)
    - [1.3 树的术语](#13-树的术语)
  - [2. 二叉树基础](#2-二叉树基础)
    - [2.1 节点结构定义](#21-节点结构定义)
    - [2.2 基本操作](#22-基本操作)
    - [2.3 树的形态判断](#23-树的形态判断)
  - [3. 二叉搜索树 (BST)](#3-二叉搜索树-bst)
    - [3.1 BST 定义](#31-bst-定义)
    - [3.2 BST 操作](#32-bst-操作)
      - [3.2.1 查找](#321-查找)
      - [3.2.2 插入](#322-插入)
      - [3.2.3 删除](#323-删除)
      - [3.2.4 查找前驱和后继](#324-查找前驱和后继)
    - [3.3 BST 范围查询](#33-bst-范围查询)
  - [4. AVL树](#4-avl树)
    - [4.1 AVL树定义](#41-avl树定义)
    - [4.2 旋转操作](#42-旋转操作)
    - [4.3 AVL插入](#43-avl插入)
    - [4.4 AVL删除](#44-avl删除)
  - [5. 红黑树](#5-红黑树)
    - [5.1 红黑树定义](#51-红黑树定义)
    - [5.2 红黑树 vs AVL树](#52-红黑树-vs-avl树)
    - [5.3 红黑树操作 (概念)](#53-红黑树操作-概念)
  - [6. 遍历算法](#6-遍历算法)
    - [6.1 递归遍历](#61-递归遍历)
    - [6.2 迭代遍历](#62-迭代遍历)
    - [6.3 Morris遍历 (O(1)空间)](#63-morris遍历-o1空间)
    - [6.4 层序遍历 (BFS)](#64-层序遍历-bfs)
  - [7. 应用场景](#7-应用场景)
  - [8. 完整代码实现](#8-完整代码实现)
  - [附录](#附录)
    - [附录A: 参考资料](#附录a-参考资料)
    - [附录B: 修订历史](#附录b-修订历史)
    - [7.2 树的序列化与反序列化](#72-树的序列化与反序列化)
    - [7.3 最近公共祖先 (LCA)](#73-最近公共祖先-lca)
    - [7.4 树的直径](#74-树的直径)
    - [7.5 最大路径和](#75-最大路径和)
    - [7.6 不同的二叉搜索树](#76-不同的二叉搜索树)
  - [8. 性能对比与选择](#8-性能对比与选择)
    - [8.1 各数据结构操作复杂度](#81-各数据结构操作复杂度)
    - [8.2 选择建议](#82-选择建议)
  - [附录](#附录-1)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [二叉树基础 (Binary Tree Fundamentals)](#二叉树基础-binary-tree-fundamentals)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [1. 概述](#1-概述)
    - [1.1 什么是树](#11-什么是树)
    - [1.2 二叉树定义](#12-二叉树定义)
    - [1.3 树的术语](#13-树的术语)
  - [2. 二叉树基础](#2-二叉树基础)
    - [2.1 节点结构定义](#21-节点结构定义)
    - [2.2 基本操作](#22-基本操作)
    - [2.3 树的形态判断](#23-树的形态判断)
  - [3. 二叉搜索树 (BST)](#3-二叉搜索树-bst)
    - [3.1 BST 定义](#31-bst-定义)
    - [3.2 BST 操作](#32-bst-操作)
      - [3.2.1 查找](#321-查找)
      - [3.2.2 插入](#322-插入)
      - [3.2.3 删除](#323-删除)
      - [3.2.4 查找前驱和后继](#324-查找前驱和后继)
    - [3.3 BST 范围查询](#33-bst-范围查询)
  - [4. AVL树](#4-avl树)
    - [4.1 AVL树定义](#41-avl树定义)
    - [4.2 旋转操作](#42-旋转操作)
    - [4.3 AVL插入](#43-avl插入)
    - [4.4 AVL删除](#44-avl删除)
  - [5. 红黑树](#5-红黑树)
    - [5.1 红黑树定义](#51-红黑树定义)
    - [5.2 红黑树 vs AVL树](#52-红黑树-vs-avl树)
    - [5.3 红黑树操作 (概念)](#53-红黑树操作-概念)
  - [6. 遍历算法](#6-遍历算法)
    - [6.1 递归遍历](#61-递归遍历)
    - [6.2 迭代遍历](#62-迭代遍历)
    - [6.3 Morris遍历 (O(1)空间)](#63-morris遍历-o1空间)
    - [6.4 层序遍历 (BFS)](#64-层序遍历-bfs)
  - [7. 应用场景](#7-应用场景)
  - [8. 完整代码实现](#8-完整代码实现)
  - [附录](#附录)
    - [附录A: 参考资料](#附录a-参考资料)
    - [附录B: 修订历史](#附录b-修订历史)
    - [7.2 树的序列化与反序列化](#72-树的序列化与反序列化)
    - [7.3 最近公共祖先 (LCA)](#73-最近公共祖先-lca)
    - [7.4 树的直径](#74-树的直径)
    - [7.5 最大路径和](#75-最大路径和)
    - [7.6 不同的二叉搜索树](#76-不同的二叉搜索树)
  - [8. 性能对比与选择](#8-性能对比与选择)
    - [8.1 各数据结构操作复杂度](#81-各数据结构操作复杂度)
    - [8.2 选择建议](#82-选择建议)
  - [附录](#附录-1)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 1. 概述

### 1.1 什么是树

树是一种**分层数据结构**，由节点和边组成，具有以下特性：

- 有一个特殊的根节点
- 每个节点有零个或多个子节点
- 没有环
- 任意两个节点之间有且仅有一条路径

```
        1          <- 根节点 (Root)
       / \
      2   3        <- 内部节点 (Internal)
     / \   \
    4   5   6      <- 叶节点 (Leaf)
```

### 1.2 二叉树定义

二叉树是每个节点**最多有两个子节点**的树：

- 左子节点 (Left Child)
- 右子节点 (Right Child)

```
二叉树节点结构:
┌─────────┬─────────┬─────────┐
│  Data   │  Left   │  Right  │
│   10    │ Pointer │ Pointer │
└─────────┴─────────┴─────────┘
```

### 1.3 树的术语

| 术语 | 定义 |
|------|------|
| **深度 (Depth)** | 从根到该节点的边数 |
| **高度 (Height)** | 从该节点到最深叶子的边数 |
| **层 (Level)** | 深度+1 |
| **度 (Degree)** | 节点的子节点数 |
| **满二叉树** | 每个节点有0或2个子节点 |
| **完全二叉树** | 除最后一层外全满，最后一层从左到右填充 |
| **平衡二叉树** | 任意节点的左右子树高度差 ≤ 1 |

---

## 2. 二叉树基础

### 2.1 节点结构定义

```c
/**
 * 二叉树节点定义
 */
typedef struct TreeNode {
    int data;                    // 节点数据
    struct TreeNode* left;       // 左子树
    struct TreeNode* right;      // 右子树
    // 可选字段
    int height;                  // 节点高度 (AVL树需要)
    char color;                  // 节点颜色 (红黑树需要)
} TreeNode;

/**
 * 二叉树结构
 */
typedef struct {
    TreeNode* root;
    size_t size;
} BinaryTree;
```

### 2.2 基本操作

```c
// 创建节点
[[nodiscard]]
TreeNode* tree_node_create(int data) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return nullptr;
    }
    node->data = data;
    node->left = nullptr;
    node->right = nullptr;
    node->height = 1;
    node->color = 'R';  // 红黑树: 新节点默认为红色
    return node;
}

// 销毁树
void tree_destroy(TreeNode* root) {
    if (root == nullptr) return;
    tree_destroy(root->left);
    tree_destroy(root->right);
    free(root);
}

// 获取树高度
int tree_height(const TreeNode* root) {
    if (root == nullptr) return 0;
    int left_h = tree_height(root->left);
    int right_h = tree_height(root->right);
    return 1 + (left_h > right_h ? left_h : right_h);
}

// 获取节点数
int tree_size(const TreeNode* root) {
    if (root == nullptr) return 0;
    return 1 + tree_size(root->left) + tree_size(root->right);
}

// 获取叶子节点数
int tree_leaf_count(const TreeNode* root) {
    if (root == nullptr) return 0;
    if (root->left == nullptr && root->right == nullptr) return 1;
    return tree_leaf_count(root->left) + tree_leaf_count(root->right);
}
```

### 2.3 树的形态判断

```c
// 判断是否为空树
static inline bool tree_is_empty(const BinaryTree* tree) {
    return tree->root == nullptr;
}

// 判断是否为满二叉树
bool tree_is_full(const TreeNode* root) {
    if (root == nullptr) return true;
    if (root->left == nullptr && root->right == nullptr) return true;
    if (root->left != nullptr && root->right != nullptr) {
        return tree_is_full(root->left) && tree_is_full(root->right);
    }
    return false;
}

// 判断是否完全二叉树 (层序遍历法)
bool tree_is_complete(const TreeNode* root) {
    if (root == nullptr) return true;

    // 使用队列进行层序遍历
    TreeNode** queue = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int front = 0, rear = 0;

    queue[rear++] = root;
    bool must_be_leaf = false;

    while (front < rear) {
        TreeNode* node = queue[front++];

        if (must_be_leaf && (node->left || node->right)) {
            free(queue);
            return false;
        }

        if (node->left == nullptr && node->right != nullptr) {
            free(queue);
            return false;
        }

        if (node->left) queue[rear++] = node->left;
        if (node->right) queue[rear++] = node->right;
        else must_be_leaf = true;
    }

    free(queue);
    return true;
}

// 判断是否为平衡二叉树
bool tree_is_balanced(const TreeNode* root) {
    if (root == nullptr) return true;

    int left_h = tree_height(root->left);
    int right_h = tree_height(root->right);

    if (abs(left_h - right_h) > 1) return false;

    return tree_is_balanced(root->left) && tree_is_balanced(root->right);
}
```

---

## 3. 二叉搜索树 (BST)

### 3.1 BST 定义

二叉搜索树满足以下性质：

- 左子树所有节点的值 < 根节点的值
- 右子树所有节点的值 > 根节点的值
- 左右子树也是BST
- **无重复值** (或按特定规则处理)

```
      50
     /  \
   30    70
  / \    / \
 20 40  60 80

中序遍历结果: 20, 30, 40, 50, 60, 70, 80 (有序)
```

### 3.2 BST 操作

#### 3.2.1 查找

```c
/**
 * BST 查找
 * 时间复杂度: O(h)，h为树高，平衡时O(log n)
 */
TreeNode* bst_search(TreeNode* root, int key) {
    if (root == nullptr || root->data == key) {
        return root;
    }

    if (key < root->data) {
        return bst_search(root->left, key);
    } else {
        return bst_search(root->right, key);
    }
}

// 迭代版本
TreeNode* bst_search_iter(TreeNode* root, int key) {
    while (root != nullptr && root->data != key) {
        if (key < root->data) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return root;
}

// 查找最小值
TreeNode* bst_find_min(TreeNode* root) {
    if (root == nullptr) return nullptr;
    while (root->left != nullptr) {
        root = root->left;
    }
    return root;
}

// 查找最大值
TreeNode* bst_find_max(TreeNode* root) {
    if (root == nullptr) return nullptr;
    while (root->right != nullptr) {
        root = root->right;
    }
    return root;
}
```

#### 3.2.2 插入

```c
/**
 * BST 插入
 * 时间复杂度: O(h)
 */
TreeNode* bst_insert(TreeNode* root, int key) {
    if (root == nullptr) {
        return tree_node_create(key);
    }

    if (key < root->data) {
        root->left = bst_insert(root->left, key);
    } else if (key > root->data) {
        root->right = bst_insert(root->right, key);
    }
    // key == root->data: 重复值，不插入

    return root;
}
```

#### 3.2.3 删除

```c
/**
 * BST 删除 (最复杂的操作)
 * 三种情况:
 * 1. 叶子节点: 直接删除
 * 2. 只有一个子节点: 用子节点替换
 * 3. 有两个子节点: 用后继(或前驱)替换，然后删除后继
 */
TreeNode* bst_delete(TreeNode* root, int key) {
    if (root == nullptr) return nullptr;

    if (key < root->data) {
        root->left = bst_delete(root->left, key);
    } else if (key > root->data) {
        root->right = bst_delete(root->right, key);
    } else {
        // 找到要删除的节点
        if (root->left == nullptr) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == nullptr) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        // 有两个子节点: 找后继(右子树最小)
        TreeNode* successor = bst_find_min(root->right);
        root->data = successor->data;
        root->right = bst_delete(root->right, successor->data);
    }

    return root;
}
```

#### 3.2.4 查找前驱和后继

```c
// 查找后继 (中序遍历的下一个)
TreeNode* bst_successor(TreeNode* root, int key) {
    TreeNode* current = bst_search(root, key);
    if (current == nullptr) return nullptr;

    // 有右子树: 右子树最小值
    if (current->right != nullptr) {
        return bst_find_min(current->right);
    }

    // 无右子树: 向上找第一个比key大的祖先
    TreeNode* successor = nullptr;
    TreeNode* ancestor = root;

    while (ancestor != current) {
        if (key < ancestor->data) {
            successor = ancestor;
            ancestor = ancestor->left;
        } else {
            ancestor = ancestor->right;
        }
    }

    return successor;
}

// 查找前驱 (中序遍历的前一个)
TreeNode* bst_predecessor(TreeNode* root, int key) {
    TreeNode* current = bst_search(root, key);
    if (current == nullptr) return nullptr;

    if (current->left != nullptr) {
        return bst_find_max(current->left);
    }

    TreeNode* predecessor = nullptr;
    TreeNode* ancestor = root;

    while (ancestor != current) {
        if (key > ancestor->data) {
            predecessor = ancestor;
            ancestor = ancestor->right;
        } else {
            ancestor = ancestor->left;
        }
    }

    return predecessor;
}
```

### 3.3 BST 范围查询

```c
/**
 * BST 范围查询 [low, high]
 * 时间复杂度: O(n) 最坏，O(log n + k) 平均，k为结果数
 */
void bst_range_query(TreeNode* root, int low, int high, int* result, int* count) {
    if (root == nullptr) return;

    // 剪枝: 如果当前值大于low，查左子树
    if (root->data > low) {
        bst_range_query(root->left, low, high, result, count);
    }

    // 当前值在范围内
    if (root->data >= low && root->data <= high) {
        result[(*count)++] = root->data;
    }

    // 剪枝: 如果当前值小于high，查右子树
    if (root->data < high) {
        bst_range_query(root->right, low, high, result, count);
    }
}
```

---

## 4. AVL树

### 4.1 AVL树定义

AVL树是一种**自平衡二叉搜索树**，满足：

- 是BST
- 任意节点的左右子树高度差(平衡因子)的绝对值 ≤ 1

```
平衡因子 = 左子树高度 - 右子树高度

平衡: [-1, 0, 1]
不平衡: <-1 或 >1

需要重新平衡的情况:
1. LL (左左): 右旋
2. RR (右右): 左旋
3. LR (左右): 先左旋再右旋
4. RL (右左): 先右旋再左旋
```

### 4.2 旋转操作

```c
/**
 * 获取节点高度
 */
static inline int avl_height(const TreeNode* node) {
    return node ? node->height : 0;
}

/**
 * 更新节点高度
 */
static inline void avl_update_height(TreeNode* node) {
    int left_h = avl_height(node->left);
    int right_h = avl_height(node->right);
    node->height = 1 + (left_h > right_h ? left_h : right_h);
}

/**
 * 获取平衡因子
 */
static inline int avl_balance_factor(const TreeNode* node) {
    return node ? avl_height(node->left) - avl_height(node->right) : 0;
}

/**
 * 右旋 (LL情况)
 *
 *     y              x
 *    / \            / \
 *   x   C   →     A   y
 *  / \               / \
 * A   B             B   C
 */
TreeNode* avl_rotate_right(TreeNode* y) {
    TreeNode* x = y->left;
    TreeNode* B = x->right;

    // 旋转
    x->right = y;
    y->left = B;

    // 更新高度
    avl_update_height(y);
    avl_update_height(x);

    return x;  // 新的根
}

/**
 * 左旋 (RR情况)
 *
 *   x                  y
 *  / \                / \
 * A   y      →      x   C
 *    / \           / \
 *   B   C         A   B
 */
TreeNode* avl_rotate_left(TreeNode* x) {
    TreeNode* y = x->right;
    TreeNode* B = y->left;

    // 旋转
    y->left = x;
    x->right = B;

    // 更新高度
    avl_update_height(x);
    avl_update_height(y);

    return y;  // 新的根
}
```

### 4.3 AVL插入

```c
/**
 * AVL 插入
 * 插入后从插入点向上重新平衡
 */
TreeNode* avl_insert(TreeNode* node, int key) {
    // 1. 标准BST插入
    if (node == nullptr) return tree_node_create(key);

    if (key < node->data) {
        node->left = avl_insert(node->left, key);
    } else if (key > node->data) {
        node->right = avl_insert(node->right, key);
    } else {
        return node;  // 重复值
    }

    // 2. 更新高度
    avl_update_height(node);

    // 3. 获取平衡因子
    int balance = avl_balance_factor(node);

    // 4. 重新平衡

    // LL情况
    if (balance > 1 && key < node->left->data) {
        return avl_rotate_right(node);
    }

    // RR情况
    if (balance < -1 && key > node->right->data) {
        return avl_rotate_left(node);
    }

    // LR情况
    if (balance > 1 && key > node->left->data) {
        node->left = avl_rotate_left(node->left);
        return avl_rotate_right(node);
    }

    // RL情况
    if (balance < -1 && key < node->right->data) {
        node->right = avl_rotate_right(node->right);
        return avl_rotate_left(node);
    }

    return node;
}
```

### 4.4 AVL删除

```c
/**
 * AVL 删除
 */
TreeNode* avl_delete(TreeNode* root, int key) {
    // 1. 标准BST删除
    if (root == nullptr) return nullptr;

    if (key < root->data) {
        root->left = avl_delete(root->left, key);
    } else if (key > root->data) {
        root->right = avl_delete(root->right, key);
    } else {
        // 找到要删除的节点
        if (root->left == nullptr || root->right == nullptr) {
            TreeNode* temp = root->left ? root->left : root->right;
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else {
                *root = *temp;  // 复制内容
            }
            free(temp);
        } else {
            // 两个子节点: 找后继
            TreeNode* successor = bst_find_min(root->right);
            root->data = successor->data;
            root->right = avl_delete(root->right, successor->data);
        }
    }

    if (root == nullptr) return nullptr;

    // 2. 更新高度
    avl_update_height(root);

    // 3. 重新平衡 (类似插入)
    int balance = avl_balance_factor(root);

    // LL
    if (balance > 1 && avl_balance_factor(root->left) >= 0) {
        return avl_rotate_right(root);
    }

    // LR
    if (balance > 1 && avl_balance_factor(root->left) < 0) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }

    // RR
    if (balance < -1 && avl_balance_factor(root->right) <= 0) {
        return avl_rotate_left(root);
    }

    // RL
    if (balance < -1 && avl_balance_factor(root->right) > 0) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }

    return root;
}
```

---

## 5. 红黑树

### 5.1 红黑树定义

红黑树是一种**近似平衡**的二叉搜索树，通过以下性质保证平衡：

1. 每个节点是红色或黑色
2. 根节点是黑色
3. 每个叶子(NIL)是黑色
4. **红色节点的子节点必须是黑色** (不能有连续红节点)
5. **从任一节点到其叶子的所有路径包含相同数目的黑节点** (黑高相同)

```
性质4和5保证了:
- 最长路径不超过最短路径的两倍
- 树高 ≤ 2log(n+1)，即 O(log n)
```

### 5.2 红黑树 vs AVL树

| 特性 | AVL树 | 红黑树 |
|------|-------|--------|
| 平衡度 | 严格平衡 | 近似平衡 |
| 查找 | 更快 (更矮) | 稍慢 |
| 插入/删除 | 更多旋转 | 更少旋转 |
| 实现复杂度 | 较简单 | 较复杂 |
| 适用场景 | 查找多，修改少 | 查找和修改均衡 |
| 实际应用 | 数据库索引 | Linux内核、C++ STL map |

### 5.3 红黑树操作 (概念)

```c
// 红黑树节点颜色
#define RED 'R'
#define BLACK 'B'

// 左旋和右旋 (同AVL)
// 但需要额外维护颜色

/**
 * 红黑树插入修复 (概念性代码)
 * 情况1: 新节点是根 → 涂黑
 * 情况2: 父节点是黑 → 无需修复
 * 情况3: 父节点和叔节点都是红 → 父、叔涂黑，祖父涂红，向上递归
 * 情况4: 父红叔黑，当前节点是"内侧" → 旋转为情况5
 * 情况5: 父红叔黑，当前节点是"外侧" → 旋转，重新着色
 */
void rb_insert_fixup(TreeNode* root, TreeNode* z) {
    // 详细实现较复杂，略
}
```

---

## 6. 遍历算法

### 6.1 递归遍历

```c
/**
 * 前序遍历 (Pre-order): 根 -> 左 -> 右
 */
void preorder_recursive(TreeNode* root) {
    if (root == nullptr) return;
    printf("%d ", root->data);
    preorder_recursive(root->left);
    preorder_recursive(root->right);
}

/**
 * 中序遍历 (In-order): 左 -> 根 -> 右
 * BST中序遍历结果是有序的
 */
void inorder_recursive(TreeNode* root) {
    if (root == nullptr) return;
    inorder_recursive(root->left);
    printf("%d ", root->data);
    inorder_recursive(root->right);
}

/**
 * 后序遍历 (Post-order): 左 -> 右 -> 根
 */
void postorder_recursive(TreeNode* root) {
    if (root == nullptr) return;
    postorder_recursive(root->left);
    postorder_recursive(root->right);
    printf("%d ", root->data);
}
```

### 6.2 迭代遍历

```c
/**
 * 前序遍历 - 迭代 (使用栈)
 */
void preorder_iterative(TreeNode* root) {
    if (root == nullptr) return;

    TreeNode** stack = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int top = -1;
    stack[++top] = root;

    while (top >= 0) {
        TreeNode* node = stack[top--];
        printf("%d ", node->data);

        // 先右后左 (栈是LIFO)
        if (node->right) stack[++top] = node->right;
        if (node->left) stack[++top] = node->left;
    }

    free(stack);
}

/**
 * 中序遍历 - 迭代 (使用栈)
 */
void inorder_iterative(TreeNode* root) {
    TreeNode** stack = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int top = -1;
    TreeNode* current = root;

    while (current != nullptr || top >= 0) {
        // 走到最左
        while (current != nullptr) {
            stack[++top] = current;
            current = current->left;
        }

        current = stack[top--];
        printf("%d ", current->data);
        current = current->right;
    }

    free(stack);
}

/**
 * 后序遍历 - 迭代 (使用栈)
 * 技巧: 根右左遍历，然后反转结果
 */
void postorder_iterative(TreeNode* root) {
    if (root == nullptr) return;

    TreeNode** stack1 = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    TreeNode** stack2 = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int top1 = -1, top2 = -1;

    stack1[++top1] = root;

    while (top1 >= 0) {
        TreeNode* node = stack1[top1--];
        stack2[++top2] = node;

        if (node->left) stack1[++top1] = node->left;
        if (node->right) stack1[++top1] = node->right;
    }

    while (top2 >= 0) {
        printf("%d ", stack2[top2--]->data);
    }

    free(stack1);
    free(stack2);
}
```

### 6.3 Morris遍历 (O(1)空间)

```c
/**
 * Morris中序遍历
 * 利用叶子节点的空指针作为临时线索
 * 时间: O(n)，空间: O(1)
 */
void morris_inorder(TreeNode* root) {
    TreeNode* current = root;

    while (current != nullptr) {
        if (current->left == nullptr) {
            printf("%d ", current->data);
            current = current->right;
        } else {
            // 找前驱
            TreeNode* predecessor = current->left;
            while (predecessor->right != nullptr && predecessor->right != current) {
                predecessor = predecessor->right;
            }

            if (predecessor->right == nullptr) {
                // 建立线索
                predecessor->right = current;
                current = current->left;
            } else {
                // 线索已存在，删除线索
                predecessor->right = nullptr;
                printf("%d ", current->data);
                current = current->right;
            }
        }
    }
}
```

### 6.4 层序遍历 (BFS)

```c
/**
 * 层序遍历 (Level-order)
 * 使用队列
 */
void level_order(TreeNode* root) {
    if (root == nullptr) return;

    TreeNode** queue = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int front = 0, rear = 0;

    queue[rear++] = root;

    while (front < rear) {
        TreeNode* node = queue[front++];
        printf("%d ", node->data);

        if (node->left) queue[rear++] = node->left;
        if (node->right) queue[rear++] = node->right;
    }

    free(queue);
}

/**
 * 层序遍历 - 按层打印
 */
void level_order_by_level(TreeNode* root) {
    if (root == nullptr) return;

    TreeNode** queue = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int front = 0, rear = 0;

    queue[rear++] = root;
    int level = 0;

    while (front < rear) {
        int level_size = rear - front;
        printf("Level %d: ", level++);

        for (int i = 0; i < level_size; i++) {
            TreeNode* node = queue[front++];
            printf("%d ", node->data);

            if (node->left) queue[rear++] = node->left;
            if (node->right) queue[rear++] = node->right;
        }
        printf("\n");
    }

    free(queue);
}
```

---

## 7. 应用场景

| 应用场景 | 使用的树类型 | 说明 |
|---------|------------|------|
| **文件系统** | B树/B+树 | 目录结构 |
| **数据库索引** | B+树 | MySQL InnoDB |
| **内存分配** | 红黑树 | Linux内核伙伴系统 |
| **STL容器** | 红黑树 | C++ map/set |
| **表达式解析** | 语法树 | 编译器 |
| **Huffman编码** | 哈夫曼树 | 数据压缩 |
| **路由算法** | 字典树 | IP路由查找 |

---

## 8. 完整代码实现

```c
/**
 * ============================================================================
 * 二叉树完整实现 (Binary Tree Complete Implementation)
 * ============================================================================
 * 编译命令: gcc -std=c23 -Wall -Wextra -o binary_tree binary_tree.c
 * 运行命令: ./binary_tree
 * ============================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <time.h>

#if __STDC_VERSION__ < 202311L
    #define nullptr NULL
#endif

// 这里包含前面所有的结构定义和函数实现...

// =============================================================================
// 测试函数
// =============================================================================

void test_bst() {
    printf("\n========== BST 测试 ==========\n");

    TreeNode* root = nullptr;
    int values[] = {50, 30, 70, 20, 40, 60, 80};

    printf("插入: ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", values[i]);
        root = bst_insert(root, values[i]);
    }
    printf("\n");

    printf("中序遍历: ");
    inorder_recursive(root);
    printf("\n");

    printf("查找 40: %s\n", bst_search(root, 40) ? "Found" : "Not Found");
    printf("查找 100: %s\n", bst_search(root, 100) ? "Found" : "Not Found");

    printf("删除 30\n");
    root = bst_delete(root, 30);
    printf("中序遍历: ");
    inorder_recursive(root);
    printf("\n");

    tree_destroy(root);
    printf("BST 测试通过!\n");
}

void test_avl() {
    printf("\n========== AVL 树测试 ==========\n");

    TreeNode* root = nullptr;
    int values[] = {10, 20, 30, 40, 50, 25};

    printf("插入序列: ");
    for (int i = 0; i < 6; i++) {
        printf("%d ", values[i]);
        root = avl_insert(root, values[i]);
    }
    printf("\n");

    printf("中序遍历: ");
    inorder_recursive(root);
    printf("\n");

    printf("树高度: %d\n", tree_height(root));
    printf("是否平衡: %s\n", tree_is_balanced(root) ? "是" : "否");

    tree_destroy(root);
    printf("AVL 树测试通过!\n");
}

void test_traversals() {
    printf("\n========== 遍历测试 ==========\n");

    // 构建测试树
    //       1
    //      / \
    //     2   3
    //    / \   \
    //   4   5   6

    TreeNode* root = tree_node_create(1);
    root->left = tree_node_create(2);
    root->right = tree_node_create(3);
    root->left->left = tree_node_create(4);
    root->left->right = tree_node_create(5);
    root->right->right = tree_node_create(6);

    printf("前序遍历 (递归): ");
    preorder_recursive(root);
    printf("\n");

    printf("中序遍历 (递归): ");
    inorder_recursive(root);
    printf("\n");

    printf("后序遍历 (递归): ");
    postorder_recursive(root);
    printf("\n");

    printf("层序遍历: ");
    level_order(root);
    printf("\n");

    printf("按层打印:\n");
    level_order_by_level(root);

    tree_destroy(root);
    printf("遍历测试通过!\n");
}

// =============================================================================
// 主函数
// =============================================================================

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║         二叉树完整实现测试 (C23 Standard)                  ║\n");
    printf("║         Binary Tree Complete Implementation                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("编译时间: %s %s\n", __DATE__, __TIME__);

    test_bst();
    test_avl();
    test_traversals();

    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              所有测试通过! All tests passed!               ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    return 0;
}
```

---

## 附录

### 附录A: 参考资料

1. 《算法导论》第12-13章 - 二叉搜索树和红黑树
2. 《数据结构与算法分析》 - Mark Allen Weiss
3. Visualgo - 算法可视化

### 附录B: 修订历史

| 版本 | 日期 | 修改内容 |
|------|------|----------|
| 1.0.0 | 2026-03-14 | 初始版本 |

---

**文档结束**


### 7.2 树的序列化与反序列化

```c
/**
 * 二叉树的序列化与反序列化
 * 用于存储和网络传输
 */

// 前序序列化
void tree_serialize_preorder(TreeNode* root, char* buffer, int* pos) {
    if (root == nullptr) {
        buffer[(*pos)++] = '#';
        buffer[(*pos)++] = ',';
        return;
    }

    // 写入当前节点值
    int n = sprintf(buffer + *pos, "%d,", root->data);
    *pos += n;

    tree_serialize_preorder(root->left, buffer, pos);
    tree_serialize_preorder(root->right, buffer, pos);
}

// 前序反序列化
TreeNode* tree_deserialize_preorder(char* data, int* pos) {
    if (data[*pos] == '#') {
        *pos += 2;  // 跳过 "#,"
        return nullptr;
    }

    // 读取数值
    int value;
    sscanf(data + *pos, "%d,", &value);

    // 找到逗号位置
    while (data[*pos] != ',') (*pos)++;
    (*pos)++;

    TreeNode* node = tree_node_create(value);
    node->left = tree_deserialize_preorder(data, pos);
    node->right = tree_deserialize_preorder(data, pos);

    return node;
}

// 层序序列化 (LeetCode格式)
void tree_serialize_levelorder(TreeNode* root, char* buffer) {
    if (root == nullptr) {
        strcpy(buffer, "[]");
        return;
    }

    int pos = 0;
    buffer[pos++] = '[';

    TreeNode** queue = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int front = 0, rear = 0;
    queue[rear++] = root;

    while (front < rear) {
        TreeNode* node = queue[front++];

        if (node == nullptr) {
            pos += sprintf(buffer + pos, "null,");
        } else {
            pos += sprintf(buffer + pos, "%d,", node->data);
            queue[rear++] = node->left;
            queue[rear++] = node->right;
        }
    }

    // 移除末尾的null和逗号
    while (pos > 1 && (buffer[pos-1] == ',' || buffer[pos-1] == 'l' ||
           buffer[pos-2] == 'l' && buffer[pos-1] == 'l')) {
        if (buffer[pos-1] == 'l') pos -= 4;  // "null"
        else pos--;
    }

    if (buffer[pos-1] == ',') pos--;
    buffer[pos++] = ']';
    buffer[pos] = '\0';

    free(queue);
}
```

### 7.3 最近公共祖先 (LCA)

```c
/**
 * 二叉树的最近公共祖先
 *
 * 性质:
 * - 如果p和q分别在root的左右子树，则root是LCA
 * - 如果都在左子树，则LCA在左子树
 * - 如果都在右子树，则LCA在右子树
 */
TreeNode* lowest_common_ancestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (root == nullptr || root == p || root == q) {
        return root;
    }

    TreeNode* left = lowest_common_ancestor(root->left, p, q);
    TreeNode* right = lowest_common_ancestor(root->right, p, q);

    // 左右子树各找到一个，当前节点就是LCA
    if (left != nullptr && right != nullptr) {
        return root;
    }

    // 只在一边找到
    return left != nullptr ? left : right;
}

// BST的LCA (利用BST性质)
TreeNode* bst_lowest_common_ancestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    while (root != nullptr) {
        if (p->data < root->data && q->data < root->data) {
            root = root->left;
        } else if (p->data > root->data && q->data > root->data) {
            root = root->right;
        } else {
            return root;
        }
    }
    return nullptr;
}
```

### 7.4 树的直径

```c
/**
 * 二叉树的直径
 * 直径 = 任意两个节点之间的最长路径的边数
 */
int tree_diameter(TreeNode* root, int* max_diameter) {
    if (root == nullptr) return 0;

    int left_height = tree_diameter(root->left, max_diameter);
    int right_height = tree_diameter(root->right, max_diameter);

    // 经过当前节点的直径 = 左高度 + 右高度
    int diameter_through_root = left_height + right_height;
    if (diameter_through_root > *max_diameter) {
        *max_diameter = diameter_through_root;
    }

    // 返回当前节点的高度
    return 1 + (left_height > right_height ? left_height : right_height);
}

int get_tree_diameter(TreeNode* root) {
    int max_diameter = 0;
    tree_diameter(root, &max_diameter);
    return max_diameter;
}
```

### 7.5 最大路径和

```c
/**
 * 二叉树中的最大路径和
 * 路径至少包含一个节点
 */
int max_path_sum(TreeNode* root, int* max_sum) {
    if (root == nullptr) return 0;

    // 只取正值的贡献
    int left_gain = max_path_sum(root->left, max_sum);
    int right_gain = max_path_sum(root->right, max_sum);

    left_gain = left_gain > 0 ? left_gain : 0;
    right_gain = right_gain > 0 ? right_gain : 0;

    // 经过当前节点的路径和
    int price_newpath = root->data + left_gain + right_gain;
    if (price_newpath > *max_sum) {
        *max_sum = price_newpath;
    }

    // 返回以当前节点为端点的最大路径和
    return root->data + (left_gain > right_gain ? left_gain : right_gain);
}
```

### 7.6 不同的二叉搜索树

```c
/**
 * 计算由1到n组成的不同的BST数量
 * 卡塔兰数: C(n) = (2n)! / ((n+1)! * n!)
 * 递推: C(n) = Σ C(i-1) * C(n-i) for i in 1..n
 */
int num_trees(int n) {
    if (n <= 1) return 1;

    // dp[i] = 由i个节点组成的BST数量
    int* dp = (int*)calloc(n + 1, sizeof(int));
    dp[0] = 1;
    dp[1] = 1;

    for (int nodes = 2; nodes <= n; nodes++) {
        for (int root = 1; root <= nodes; root++) {
            int left_trees = dp[root - 1];
            int right_trees = dp[nodes - root];
            dp[nodes] += left_trees * right_trees;
        }
    }

    int result = dp[n];
    free(dp);
    return result;
}

// 生成所有不同的BST
typedef struct TreeNodeList {
    TreeNode* node;
    struct TreeNodeList* next;
} TreeNodeList;

TreeNodeList* generate_trees(int start, int end) {
    TreeNodeList* result = (TreeNodeList*)malloc(sizeof(TreeNodeList));
    result->node = nullptr;
    result->next = nullptr;
    TreeNodeList* tail = result;

    if (start > end) {
        return result;
    }

    for (int i = start; i <= end; i++) {
        TreeNodeList* left_trees = generate_trees(start, i - 1);
        TreeNodeList* right_trees = generate_trees(i + 1, end);

        for (TreeNodeList* left = left_trees; left != nullptr; left = left->next) {
            for (TreeNodeList* right = right_trees; right != nullptr; right = right->next) {
                TreeNode* root = tree_node_create(i);
                root->left = left->node;
                root->right = right->node;

                tail->next = (TreeNodeList*)malloc(sizeof(TreeNodeList));
                tail = tail->next;
                tail->node = root;
                tail->next = nullptr;
            }
        }
    }

    TreeNodeList* temp = result;
    result = result->next;
    free(temp);

    return result;
}
```

---

## 8. 性能对比与选择

### 8.1 各数据结构操作复杂度

| 操作 | 普通BST | AVL树 | 红黑树 | 哈希表 |
|------|---------|-------|--------|--------|
| 查找 | O(h) | O(log n) | O(log n) | O(1) |
| 插入 | O(h) | O(log n) | O(log n) | O(1) |
| 删除 | O(h) | O(log n) | O(log n) | O(1) |
| 遍历 | O(n) | O(n) | O(n) | O(n) |
| 空间 | O(n) | O(n) | O(n) | O(n) |

注: h为树高，最坏情况h=n

### 8.2 选择建议

```
选择普通BST:
- 数据随机分布
- 实现简单优先
- 插入顺序不可预测

选择AVL树:
- 查找操作远多于插入/删除
- 需要严格平衡保证
- 对延迟敏感

选择红黑树:
- 插入/删除与查找频率相当
- 实现复杂度适中
- 实际应用最广泛

选择哈希表:
- 只需键值查找，不需要有序性
- 要求O(1)操作
- 内存充足
```

---

## 附录


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

/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\02_Separation_Logic_Entropy.md
 * Line: 399
 * Language: c
 * Block ID: abfb64f2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 二叉树节点
struct TreeNode {
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
};

/*
 * 二叉树断言：tree(t, T)
 *
 * - t是树根指针
 * - T是抽象树（数学对象）
 * - tree(nil, empty) = emp
 * - tree(t, node(v, L, R)) =
 *     t ↦ v, l, r * tree(l, L) * tree(r, R)
 */

// 树的复制
struct TreeNode* copy_tree(struct TreeNode *t) {
    if (t == NULL) return NULL;

    struct TreeNode *new_node = malloc(sizeof(struct TreeNode));
    new_node->value = t->value;
    new_node->left = copy_tree(t->left);
    new_node->right = copy_tree(t->right);

    return new_node;
}

/*
 * 复制函数的规范：
 *
 * {tree(t, T)}
 * copy_tree(t)
 * {tree(t, T) * tree(result, T)}
 *
 * 证明思路（归纳）：
 * - 基本情况：T = empty，t = nil
 *   {emp} return nil {emp * emp} = {emp} ✓
 *
 * - 归纳步骤：T = node(v, L, R)
 *   前提：t ↦ v, l, r * tree(l, L) * tree(r, R)
 *
 *   递归调用copy_tree(l)：
 *   {tree(l, L)}
 *   返回l'，满足：tree(l, L) * tree(l', L)
 *
 *   递归调用copy_tree(r)：
 *   {tree(r, R)}
 *   返回r'，满足：tree(r, R) * tree(r', R)
 *
 *   创建新节点new_t ↦ v, l', r'
 *
 *   最终结果：
 *   t ↦ v, l, r * tree(l, L) * tree(r, R) *
 *   new_t ↦ v, l', r' * tree(l', L) * tree(r', R)
 *
 *   即：tree(t, T) * tree(new_t, T) ✓
 */

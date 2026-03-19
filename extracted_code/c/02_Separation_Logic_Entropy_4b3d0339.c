/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\02_Separation_Logic_Entropy.md
 * Line: 342
 * Language: c
 * Block ID: 4b3d0339
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 链表节点结构
struct Node {
    int data;
    struct Node *next;
};

/*
 * 链表反转的分离逻辑规范：
 *
 * 前提：{list(x, α)}
 * 代码：reverse(x)
 * 后置：{list(y, rev(α))}
 *
 * 其中：
 * - list(x, α) 表示从x开始的链表存储序列α
 * - rev(α) 是α的反转
 * - y是返回的头指针
 */

// 迭代反转实现
struct Node* reverse_iterative(struct Node *x) {
    struct Node *y = NULL;
    struct Node *t;

    while (x != NULL) {
        t = x;
        x = x->next;
        t->next = y;
        y = t;
    }

    return y;
}

/*
 * 循环不变式：
 *
 * ∃α, β.
 *   α ++ rev(β) = 输入序列 ∧
 *   list(x, α) * list(y, β)
 *
 * 初始：α = 输入序列, β = []
 * 终止：α = [], β = rev(输入序列)
 *
 * 每次迭代：
 *   x = a :: α'
 *   y = β
 *   执行后：
 *   x = α'
 *   y = a :: β
 *   所以：α' ++ rev(a :: β) = α' ++ rev(β) ++ [a] = α ++ rev(β) ✓
 */

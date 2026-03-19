/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\02_Separation_Logic_Entropy.md
 * Line: 186
 * Language: c
 * Block ID: 9812374c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 分离逻辑断言：
 *
 * emp              - 空堆
 * l ↦ v            - 地址l存储值v（单点）
 * P * Q            - 分离合取：存在不相交堆h1,h2使P(h1)和Q(h2)
 * P -* Q           - 分离蕴含：与任何满足P的堆组合都满足Q
 * l ↦ _            - 存在某个值
 * l ↦ v1, v2       - 连续存储（l↦v1 * (l+1)↦v2）
 *
 * 派生断言：
 * true             - 任意堆（包括空）
 * false            - 不可能
 * P ∧ Q            - 逻辑与（同一堆）
 * P ∨ Q            - 逻辑或
 * ∃x.P             - 存在量词
 * ∀x.P             - 全称量词
 *
 * 列表断言：
 * list(x, α)       - 从x开始的链表存储序列α
 * tree(t)          - 从t开始的二叉树
 */

// 链表段断言：从start到end的链表段存储序列xs
bool sl_lseg(Heap *h, Addr start, Addr end, GList *xs) {
    if (xs == NULL) {
        // 空序列：start必须等于end，且堆必须为空
        return start == end && sl_emp(h);
    } else {
        // 非空：start必须指向第一个元素和下一个节点
        Value x = GPOINTER_TO_INT(xs->data);

        // 检查start ↦ x, next
        if (g_hash_table_size(h->mapping) < 2) return FALSE;

        Value stored_val = GPOINTER_TO_INT(g_hash_table_lookup(h->mapping,
                                                                GUINT_TO_POINTER(start)));
        if (stored_val != x) return FALSE;

        Value next = GPOINTER_TO_INT(g_hash_table_lookup(h->mapping,
                                                          GUINT_TO_POINTER(start + 1)));

        // 剩余部分应该是lseg(next, end, xs->next)
        return TRUE;
    }
}

// 完整链表断言
bool sl_list(Heap *h, Addr x, GList *xs) {
    // 等价于 lseg(x, NULL, xs)
    return sl_lseg(h, x, 0, xs);
}

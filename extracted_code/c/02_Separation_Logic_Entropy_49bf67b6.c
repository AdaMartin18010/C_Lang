/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\02_Separation_Logic_Entropy.md
 * Line: 507
 * Language: c
 * Block ID: 49bf67b6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 锁的分离逻辑规范：
 *
 * 锁不变式：lock(l, P)
 * - l是锁的地址
 * - P是保护资源的断言
 *
 * acquire(l)：
 *   {emp} acquire(l) {P}
 *
 * release(l)：
 *   {P} release(l) {emp}
 */

// 细粒度锁示例：链表节点锁
struct LockedNode {
    int data;
    struct LockedNode *next;
    pthread_mutex_t lock;
};

/*
 * 带锁链表的断言：
 *
 * llist(x, α) =
 *   x = nil ∧ emp
 *   ∨ ∃v, y. x ↦ v, y, l * lock(l, node_inv(v)) * llist(y, α')
 *   其中 α = v :: α'
 *
 * node_inv(v) = ∃w. data ↦ w * (w = v)
 *   // 节点保护其数据值
 */

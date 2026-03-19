/*
 * Auto-generated from: 03_System_Technology_Domains\12_Persistent_Memory\02_PM_Optimized_DataStructures.md
 * Line: 165
 * Language: c
 * Block ID: ddcba84e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 事务宏 */
TX_BEGIN(PMEMobjpool *pop) {
    /* 事务体 */
} TX_ONCOMMIT {
    /* 提交后执行 */
} TX_ONABORT {
    /* 回滚后执行 */
} TX_FINALLY {
    /* 始终执行 */
} TX_END

/* 事务内存操作 */
TX_ADD(TOID obj)           /* 添加对象到事务 */
TX_ADD_FIELD(TOID obj, field)  /* 添加字段 */
TX_SET(TOID obj, field, value) /* 安全赋值 */
TX_NEW(type)               /* 事务内分配 */
TX_ALLOC(type, size)       /* 事务内分配指定大小 */
TX_FREE(TOID obj)          /* 事务内释放 */

/* 示例：事务更新 */
void update_counter(PMEMobjpool *pop, TOID(struct counter) c, int delta) {
    TX_BEGIN(pop) {
        TX_ADD(c);
        D_RW(c)->value += delta;
        D_RW(c)->last_update = time(NULL);
    } TX_ONABORT {
        fprintf(stderr, "Transaction aborted\n");
    } TX_END
}

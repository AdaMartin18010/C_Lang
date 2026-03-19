/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 387
 * Language: c
 * Block ID: a8d9b34b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 额外宏定义
#define GET_PTR(p)      (*(unsigned long *)(p))
#define SET_PTR(p, ptr) (*(unsigned long *)(p) = (unsigned long)(ptr))

#define PRED_PTR(bp)    ((char *)(bp))
#define SUCC_PTR(bp)    ((char *)(bp) + WSIZE)
#define PRED(bp)        ((char *)GET_PTR(PRED_PTR(bp)))
#define SUCC(bp)        ((char *)GET_PTR(SUCC_PTR(bp)))

// 空闲链表头尾指针
static char *free_list_head = NULL;
static char *free_list_tail = NULL;

// 插入到空闲链表头部
static void insert_free_block(void *bp) {
    SET_PTR(PRED_PTR(bp), NULL);
    SET_PTR(SUCC_PTR(bp), free_list_head);
    if (free_list_head != NULL)
        SET_PTR(PRED_PTR(free_list_head), bp);
    free_list_head = bp;
}

// 从空闲链表删除
static void remove_free_block(void *bp) {
    if (PRED(bp) != NULL)
        SET_PTR(SUCC_PTR(PRED(bp)), SUCC(bp));
    else
        free_list_head = SUCC(bp);
    
    if (SUCC(bp) != NULL)
        SET_PTR(PRED_PTR(SUCC(bp)), PRED(bp));
}

/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 491
 * Language: c
 * Block ID: 69de2679
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define ALIGNMENT 16
#define MIN_BLOCK_SIZE 32
#define SIZE_CLASS_COUNT 15

static char *heap_listp;
static char *size_class_heads[SIZE_CLASS_COUNT];

// 获取大小类
static int get_size_class(size_t size) {
    int i = 0;
    size_t sc = 32;
    while (size > sc && i < SIZE_CLASS_COUNT - 1) {
        sc <<= 1;
        i++;
    }
    return i;
}

// 插入到对应大小类
static void insert_block(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    int class = get_size_class(size);
    
    void *head = size_class_heads[class];
    PRED(bp) = NULL;
    SUCC(bp) = head;
    if (head != NULL)
        PRED(head) = bp;
    size_class_heads[class] = bp;
}

// 删除块
static void remove_block(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    int class = get_size_class(size);
    
    if (PRED(bp) != NULL)
        SUCC(PRED(bp)) = SUCC(bp);
    else
        size_class_heads[class] = SUCC(bp);
    
    if (SUCC(bp) != NULL)
        PRED(SUCC(bp)) = PRED(bp);
}

// 在大小类中搜索
static void *find_fit(size_t asize) {
    int class = get_size_class(asize);
    
    for (int i = class; i < SIZE_CLASS_COUNT; i++) {
        void *bp = size_class_heads[i];
        while (bp != NULL) {
            if (GET_SIZE(HDRP(bp)) >= asize)
                return bp;
            bp = SUCC(bp);
        }
    }
    return NULL;
}

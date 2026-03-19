/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 154
 * Language: c
 * Block ID: 0d50defc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * mm_init - 初始化malloc包
 * 创建带空闲块的堆
 */
int mm_init(void) {
    /* 创建初始空堆 */
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    
    PUT(heap_listp, 0);                           // 对齐填充
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); // 序言块头部
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); // 序言块脚部
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));     // 结尾块头部
    heap_listp += (2 * WSIZE);
    
    /* 用CHUNKSIZE字节扩展空堆 */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    
    return 0;
}

/*
 * extend_heap - 扩展堆
 */
static void *extend_heap(size_t words) {
    char *bp;
    size_t size;
    
    /* 分配偶数个字以保持对齐 */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    
    /* 初始化空闲块头部/脚部和结尾块头部 */
    PUT(HDRP(bp), PACK(size, 0));         // 空闲块头部
    PUT(FTRP(bp), PACK(size, 0));         // 空闲块脚部
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // 新结尾块头部
    
    /* 如果前一个块空闲，则合并 */
    return coalesce(bp);
}

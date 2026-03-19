/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 246
 * Language: c
 * Block ID: a1bfd188
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * mm_malloc - 分配大小至少为size字节的块
 */
void *mm_malloc(size_t size) {
    size_t asize;      // 调整后的块大小
    size_t extendsize; // 如果没有合适块则扩展堆
    char *bp;
    
    /* 忽略虚假请求 */
    if (size == 0)
        return NULL;
    
    /* 调整为对齐要求 */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    
    /* 搜索空闲块 */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }
    
    /* 没有找到合适的块，获取更多内存 */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - 释放块
 */
void mm_free(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * find_fit - 首次适配搜索
 */
static void *find_fit(size_t asize) {
    void *bp;
    
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL;  // 没有找到合适的块
}

/*
 * place - 将块放在空闲块的开始处，如果剩余部分足够大则分割
 */
static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));
    
    if ((csize - asize) >= (2 * DSIZE)) {
        // 分割块
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    } else {
        // 不分割
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

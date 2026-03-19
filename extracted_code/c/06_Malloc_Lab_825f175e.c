/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 568
 * Language: c
 * Block ID: 825f175e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 分割策略 - 只在剩余足够大时分割
// 2. 边界标记合并 - 立即合并相邻空闲块
// 3. 考虑重新分配优化 - 原地扩展如果可能

// 优化的realloc
void *mm_realloc(void *ptr, size_t size) {
    if (ptr == NULL) return mm_malloc(size);
    if (size == 0) { mm_free(ptr); return NULL; }
    
    size_t old_size = GET_SIZE(HDRP(ptr));
    size_t asize = ALIGN(size + DSIZE);
    
    // 如果新大小更小或相同，可能不需要移动
    if (asize <= old_size) {
        // 如果可以分割
        if (old_size - asize >= MIN_BLOCK_SIZE) {
            PUT(HDRP(ptr), PACK(asize, 1));
            PUT(FTRP(ptr), PACK(asize, 1));
            void *next = NEXT_BLKP(ptr);
            PUT(HDRP(next), PACK(old_size - asize, 0));
            PUT(FTRP(next), PACK(old_size - asize, 0));
            coalesce(next);
        }
        return ptr;
    }
    
    // 检查下一块是否空闲且足够大
    void *next = NEXT_BLKP(ptr);
    if (!GET_ALLOC(HDRP(next)) && 
        old_size + GET_SIZE(HDRP(next)) >= asize) {
        remove_block(next);
        size_t total = old_size + GET_SIZE(HDRP(next));
        PUT(HDRP(ptr), PACK(total, 1));
        PUT(FTRP(ptr), PACK(total, 1));
        if (total - asize >= MIN_BLOCK_SIZE) {
            // 分割
            PUT(HDRP(ptr), PACK(asize, 1));
            PUT(FTRP(ptr), PACK(asize, 1));
            next = NEXT_BLKP(ptr);
            PUT(HDRP(next), PACK(total - asize, 0));
            PUT(FTRP(next), PACK(total - asize, 0));
            insert_block(next);
        }
        return ptr;
    }
    
    // 否则分配新块并复制
    void *new_ptr = mm_malloc(size);
    memcpy(new_ptr, ptr, old_size - DSIZE);
    mm_free(ptr);
    return new_ptr;
}

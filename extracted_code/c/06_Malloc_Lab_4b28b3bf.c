/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 327
 * Language: c
 * Block ID: 4b28b3bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * mm_realloc - 重新分配块
 */
void *mm_realloc(void *ptr, size_t size) {
    size_t oldsize;
    void *newptr;
    
    /* 如果ptr为NULL，等价于malloc */
    if (ptr == NULL) {
        return mm_malloc(size);
    }
    
    /* 如果size为0，等价于free */
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }
    
    /* 否则，分配新块并复制数据 */
    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    
    /* 复制旧数据 */
    oldsize = GET_SIZE(HDRP(ptr));
    if (size < oldsize)
        oldsize = size;
    memcpy(newptr, ptr, oldsize);
    
    /* 释放旧块 */
    mm_free(ptr);
    
    return newptr;
}

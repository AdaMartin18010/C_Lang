/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 158
 * Language: c
 * Block ID: e6f1749c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * malloc(size) 分配流程：
 *
 * 1. 检查fast bins（大小 <= 0x80）
 *    - 精确匹配，LIFO
 *
 * 2. 检查small bins（0x80 < 大小 <= 0x400）
 *    - 精确匹配，FIFO
 *
 * 3. 检查large bins（大小 > 0x400）
 *    - 最佳适配或首次适配
 *
 * 4. 检查unsorted bin
 *    - 遍历，可能触发合并
 *
 * 5. 从top chunk分配
 *    - 切割top chunk
 *
 * 6. 系统调用扩展内存
 *    - brk() 或 mmap()
 */

// 模拟malloc实现
void* my_malloc(size_t size) {
    // 对齐到8/16字节
    size_t aligned_size = ALIGN(size + SIZE_SZ, MALLOC_ALIGNMENT);

    // 1. 检查fast bins（小块）
    if (aligned_size <= MAX_FAST_SIZE) {
        int idx = fastbin_index(aligned_size);
        mchunk *chunk = arena->fastbinsY[idx];

        if (chunk != NULL) {
            // 从fast bin取出
            arena->fastbinsY[idx] = chunk->fd;

            // 标记为使用
            set_inuse(chunk, aligned_size);

            return chunk2mem(chunk);
        }
    }

    // 2. 检查small bins
    if (aligned_size < MIN_LARGE_SIZE) {
        int idx = smallbin_index(aligned_size);
        mchunk *victim = last(arena->bins, idx);

        if (victim != first(arena->bins, idx)) {
            // 找到精确匹配
            unlink_chunk(victim);
            set_inuse(victim, aligned_size);
            return chunk2mem(victim);
        }
    }

    // 3. 检查unsorted bin和large bins
    // ... 复杂逻辑

    // 4. 从top chunk分配
    mchunk *top = arena->top;
    size_t top_size = chunksize(top);

    if (top_size >= aligned_size + MINSIZE) {
        // 切割top chunk
        arena->top = chunk_at_offset(top, aligned_size);
        set_head(arena->top, top_size - aligned_size | PREV_INUSE);

        set_head(top, aligned_size | PREV_INUSE);
        return chunk2mem(top);
    }

    // 5. 系统调用扩展
    return sysmalloc(aligned_size, arena);
}

// 释放流程
void my_free(void *mem) {
    if (mem == NULL) return;

    mchunk *chunk = mem2chunk(mem);
    size_t size = chunksize(chunk);

    // 检查是否mmap分配
    if (chunk_is_mmapped(chunk)) {
        munmap_chunk(chunk);
        return;
    }

    // 检查是否可以放入fast bins
    if (size <= MAX_FAST_SIZE && get_fastchunks(arena)) {
        // 放入fast bin（LIFO）
        int idx = fastbin_index(size);
        chunk->fd = arena->fastbinsY[idx];
        arena->fastbinsY[idx] = chunk;
        return;
    }

    // 尝试合并相邻空闲块
    mchunk *next = chunk_at_offset(chunk, size);

    if (!inuse(next)) {
        // 合并后一个块
        size += chunksize(next);
        unlink_chunk(next);
    }

    if (!prev_inuse(chunk)) {
        // 合并前一个块
        size_t prev_size = chunk->prev_size;
        mchunk *prev = chunk_at_offset(chunk, -((long)prev_size));
        size += prev_size;
        chunk = prev;
        unlink_chunk(prev);
    }

    // 设置合并后的大小
    set_head(chunk, size | PREV_INUSE);
    set_foot(chunk, size);

    // 放入unsorted bin
    if (chunk != arena->top) {
        insert_unsorted(chunk);
    }
}

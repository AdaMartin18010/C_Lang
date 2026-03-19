/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 358
 * Language: c
 * Block ID: e571af1a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ptmalloc的延迟合并策略：
 *
 * 1. Fast bins：不合并，快速分配释放
 * 2. 小释放：可能放入unsorted bin，延迟合并
 * 3. 大分配前：合并unsorted bin中的块
 * 4. 内存紧缩：malloc_trim()时完全合并
 */

// 触发合并的时机
void malloc_consolidate(mstate *av) {
    // 合并fast bins到普通bins
    for (int i = 0; i < NFASTBINS; i++) {
        mchunk *p = av->fastbinsY[i];

        while (p != NULL) {
            mchunk *next = p->fd;
            size_t size = chunksize(p);

            // 合并后一个块
            mchunk *nextchunk = chunk_at_offset(p, size);

            if (!inuse(nextchunk)) {
                size += chunksize(nextchunk);
                unlink_chunk(nextchunk);
            }

            // 合并前一个块
            if (!prev_inuse(p)) {
                size_t prevsize = p->prev_size;
                mchunk *prev = chunk_at_offset(p, -((long)prevsize));
                size += prevsize;
                p = prev;
                unlink_chunk(prev);
            }

            // 放入unsorted bin
            set_head(p, size | PREV_INUSE);
            set_foot(p, size);

            if (size >= MIN_LARGE_SIZE) {
                // 大 Chunk 可能需要放入 large bins
                insert_large_bin(av, p, size);
            } else {
                insert_unsorted(av, p);
            }

            p = next;
        }

        av->fastbinsY[i] = NULL;
    }
}

// 内存紧缩：归还内存给系统
int malloc_trim(size_t pad) {
    mstate *av = &main_arena;

    // 尝试收缩top chunk
    size_t top_size = chunksize(av->top);

    // 保留 pad 大小的缓冲
    if (top_size > pad + MINSIZE) {
        size_t release_size = top_size - pad;

        // 使用brk收缩或munmap释放
        if (av == &main_arena) {
            // 尝试收缩brk
            sbrk(-release_size);
        }
    }

    // 释放mmap的块
    // ...

    return 0;
}

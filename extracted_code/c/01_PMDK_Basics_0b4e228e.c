/*
 * Auto-generated from: 03_System_Technology_Domains\12_Persistent_Memory\01_PMDK_Basics.md
 * Line: 262
 * Language: c
 * Block ID: 0b4e228e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 安全的持久化复制 */
void pmem_memcpy_persist(void *pmem_dest, const void *src, size_t len) {
    /* 使用libpmem优化的复制 */
    pmem_memcpy_persist(pmem_dest, src, len);
}

/* 带持久化的内存设置 */
void pmem_memset_persist(void *pmem_dest, int c, size_t len) {
    pmem_memset_persist(pmem_dest, c, len);
}

/* 显式刷新范围 */
void pmem_flush(const void *addr, size_t len) {
    /* 刷新缓存行 */
    uintptr_t ptr = (uintptr_t)addr;
    uintptr_t end = ptr + len;

    /* 64字节对齐的缓存行刷盘 */
    for (; ptr < end; ptr += 64) {
        pmem_clwb((void *)ptr);
    }

    /* 内存屏障 */
    _mm_sfence();
}

/* 无刷盘的写入（延迟持久化） */
void pmem_memcpy_nodrain(void *pmem_dest, const void *src, size_t len) {
    /* 仅复制，不执行sfence */
    pmem_memcpy_nodrain(pmem_dest, src, len);
}

/* 手动执行持久化屏障 */
void pmem_drain(void) {
    pmem_drain();
}

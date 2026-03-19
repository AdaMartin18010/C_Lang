/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 442
 * Language: c
 * Block ID: 43cd9e4d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 内存获取策略：
 *
 * brk()：
 * - 优点：连续地址空间，低TLB压力
 * - 缺点：不能独立释放中间块
 * - 适用：小分配，长期持有的内存
 *
 * mmap()：
 * - 优点：独立释放，可归还系统
 * - 缺点：更高的开销，更多TLB条目
 * - 适用：大分配，短期使用的内存
 */

// ptmalloc的阈值策略
#define DEFAULT_MMAP_THRESHOLD (128 * 1024)  // 128KB
#define DEFAULT_MMAP_THRESHOLD_MAX (512 * 1024)

void* sysmalloc(size_t nb, mstate *av) {
    // 检查是否应该使用mmap
    if (av == NULL ||
        (nb > mp_.mmap_threshold && mp_.n_mmaps < mp_.n_mmaps_max)) {

        // 使用mmap
        size_t size = ALIGN_UP(nb + SIZE_SZ, pagesize);
        char *mm = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (mm != MAP_FAILED) {
            // 设置mmap标志
            set_head((mchunk*)mm, size | IS_MMAPPED);

            mp_.n_mmaps++;
            mp_.mmapped_mem += size;

            return chunk2mem((mchunk*)mm);
        }
    }

    // 使用brk扩展堆
    // ...
}

// 动态调整mmap阈值
void adjust_mmap_threshold(size_t released_size) {
    // 如果释放了大块，降低阈值以更多使用mmap
    if (released_size > DEFAULT_MMAP_THRESHOLD) {
        mp_.mmap_threshold =
            (mp_.mmap_threshold + released_size) / 2;

        // 限制上限
        if (mp_.mmap_threshold > DEFAULT_MMAP_THRESHOLD_MAX) {
            mp_.mmap_threshold = DEFAULT_MMAP_THRESHOLD_MAX;
        }
    }
}

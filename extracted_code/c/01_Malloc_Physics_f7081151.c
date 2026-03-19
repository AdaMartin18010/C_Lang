/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 84
 * Language: c
 * Block ID: f7081151
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ptmalloc（glibc默认分配器）架构：
 *
 * 1. Arena：独立的内存分配区域
 *    - 主arena：使用brk()
 *    - 副arena：使用mmap()
 *    - 每线程一个arena（减少竞争）
 *
 * 2. Heap：arena内的内存块
 *    - 通过sub-heap管理
 *
 * 3. Chunk：实际分配的内存单元
 *    - 包含元数据（大小、标志位）
 *    - 相邻空闲块合并
 *
 * 4. Bins：空闲块分类存储
 *    - Fast bins：小块快速分配
 *    - Small bins：精确大小匹配
 *    - Large bins：范围匹配
 *    - Unsorted bin：合并缓冲区
 */

// ptmalloc核心数据结构（简化）

typedef struct malloc_chunk {
    size_t prev_size;      // 前一个块大小（如果前一个块空闲）
    size_t size;           // 当前块大小 + 标志位

    struct malloc_chunk *fd;  // 前向指针（空闲时）
    struct malloc_chunk *bk;  // 后向指针（空闲时）
} mchunk;

// 标志位
#define PREV_INUSE 0x1     // 前一个块是否使用
#define IS_MMAPPED 0x2     // 是否mmap分配
#define NON_MAIN_ARENA 0x4 // 是否非主arena

#define SIZE_BITS (PREV_INUSE | IS_MMAPPED | NON_MAIN_ARENA)
#define chunksize(p) ((p)->size & ~SIZE_BITS)

// Arena结构
typedef struct malloc_state {
    int mutex;                    // 锁

    // Fast bins
    mchunk *fastbinsY[NFASTBINS];

    // Base bins（包含small/large/unsorted）
    mchunk *bins[NBINS * 2 - 2];

    // Top chunk（ wilderness ）
    mchunk *top;

    // Last remainder
    mchunk *last_remainder;

    // 统计
    size_t max_system_mem;
    size_t system_mem;
} mstate;

// Fast bin索引计算（8字节对齐，大小/8 - 2）
#define fastbin_index(sz) \
    ((((unsigned int)(sz)) >> (SIZE_SZ == 8 ? 4 : 3)) - 2)

// Small bin索引（64位系统）
#define smallbin_index(sz) \
    ((SMALLBIN_WIDTH == 16 ? (((unsigned)(sz)) >> 4) \
                           : (((unsigned)(sz)) >> 3)) + SMALLBIN_CORRECTION)

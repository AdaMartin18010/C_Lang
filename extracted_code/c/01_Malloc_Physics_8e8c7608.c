/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 290
 * Language: c
 * Block ID: 8e8c7608
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 碎片类型：
 *
 * 内部碎片：分配的块大于请求大小
 * - 对齐要求导致
 * - 元数据开销
 *
 * 外部碎片：空闲内存分散，无法满足大块请求
 * - 频繁分配释放不同大小块
 * - 需要合并策略
 */

// 内部碎片计算
size_t internal_fragmentation(void *ptr) {
    mchunk *chunk = mem2chunk(ptr);
    size_t allocated = chunksize(chunk);
    size_t requested = allocated - SIZE_SZ;  // 减去元数据

    return allocated - requested;
}

// 外部碎片度量
double external_fragmentation_ratio(Heap *heap) {
    size_t total_free = 0;
    size_t largest_free = 0;

    // 遍历所有空闲块
    for (mchunk *p = heap->free_list; p != NULL; p = p->fd) {
        size_t sz = chunksize(p);
        total_free += sz;
        if (sz > largest_free) {
            largest_free = sz;
        }
    }

    if (total_free == 0) return 0.0;

    // 碎片率 = 1 - (最大空闲块 / 总空闲内存)
    return 1.0 - ((double)largest_free / total_free);
}

// 减少碎片的分配策略
void* malloc_low_fragmentation(size_t size) {
    // 使用大小分级分配
    // 将请求向上取整到标准大小

    static const size_t size_classes[] = {
        8, 16, 24, 32, 48, 64, 96, 128,
        192, 256, 384, 512, 768, 1024,
        1536, 2048, 3072, 4096
    };

    // 找到合适的大小类
    size_t alloc_size = size;
    for (int i = 0; i < sizeof(size_classes)/sizeof(size_classes[0]); i++) {
        if (size_classes[i] >= size + SIZE_SZ) {
            alloc_size = size_classes[i];
            break;
        }
    }

    return my_malloc(alloc_size);
}

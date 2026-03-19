/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\04_Linux_Kernel_Toolchain.md
 * Line: 453
 * Language: c
 * Block ID: 192eb48e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 内核内存分配 */

#include <linux/slab.h>
#include <linux/gfp.h>

/* 创建 slab 缓存 */
struct kmem_cache *my_cache;

static int __init cache_init(void)
{
    my_cache = kmem_cache_create("my_cache",
                                  sizeof(struct my_struct),
                                  0,                    /* 对齐 */
                                  SLAB_HWCACHE_ALIGN,   /* 标志 */
                                  NULL);                /* 构造函数 */
    if (!my_cache)
        return -ENOMEM;
    return 0;
}

/* 分配对象 */
struct my_struct *obj;

/* GFP_KERNEL - 可能睡眠 */
obj = kmem_cache_alloc(my_cache, GFP_KERNEL);
if (!obj)
    return -ENOMEM;

/* GFP_ATOMIC - 不睡眠，用于中断上下文 */
obj = kmem_cache_alloc(my_cache, GFP_ATOMIC);

/* GFP_DMA - 分配 DMA 可用内存 */
void *dma_buffer = kmalloc(size, GFP_DMA);

/* 释放对象 */
kmem_cache_free(my_cache, obj);

/* 销毁缓存 */
kmem_cache_destroy(my_cache);

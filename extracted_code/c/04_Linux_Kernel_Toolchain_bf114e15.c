/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\04_Linux_Kernel_Toolchain.md
 * Line: 497
 * Language: c
 * Block ID: bf114e15
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 页级分配 */

#include <linux/mm.h>

/* 分配单页 */
struct page *page = alloc_page(GFP_KERNEL);
if (!page)
    return -ENOMEM;
void *addr = page_address(page);

/* 分配多页 (2^order 页) */
unsigned int order = 3;  /* 8 页 */
struct page *pages = alloc_pages(GFP_KERNEL, order);
if (!pages)
    return -ENOMEM;

/* 释放页 */
__free_page(page);
__free_pages(pages, order);

/* 高端内存映射 */
void *vaddr = kmap(page);   /* 临时映射 */
kunmap(page);

void *vaddr = kmap_atomic(page);  /* 原子映射 */
kunmap_atomic(vaddr);

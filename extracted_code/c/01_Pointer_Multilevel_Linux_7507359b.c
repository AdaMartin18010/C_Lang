/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 257
 * Language: c
 * Block ID: 7507359b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux内核: mm/slab.c (简化示意)
// kmalloc用于分配内核内存，返回void*

void *kmalloc(size_t size, gfp_t flags);

// 使用一级指针
void *ptr = kmalloc(100, GFP_KERNEL);
if (ptr) {
    memset(ptr, 0, 100);
    kfree(ptr);
}

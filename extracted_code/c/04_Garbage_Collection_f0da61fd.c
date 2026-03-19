/*
 * Auto-generated from: 03_System_Technology_Domains\04_Garbage_Collection.md
 * Line: 174
 * Language: c
 * Block ID: f0da61fd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void configure_gc() {
    GC_set_max_heap_size(100 * 1024 * 1024);  // 最大100MB
    GC_enable_incremental();                   // 启用增量GC
}
